#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "input/camera.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"

using namespace Math;

namespace RenderStat {
	extern int g_numVoxels;
	extern int g_numChunks;
}

namespace Voxel {

	Model::Model() :
		m_numVoxels(0),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f),
		m_voxelTree(this)
	{
		auto x = IVec3(3) * 0.5f;
	}

	Model::~Model()
	{
	}

	// ********************************************************************* //
	struct DecideToDraw: public Model::ModelData::SVOProcessor
	{
		const Input::Camera& camera;					// Required for culling and LOD
		Model::ModelData* model;						// Operate on this data.
		std::unordered_map<Math::IVec4, Chunk>* chunks;	// Create or find chunks here.
		const Math::Mat4x4& modelView;
		ChunkBuilder* builder;
		double gameTime;

		DecideToDraw(const Input::Camera& _camera,
				Model::ModelData* _model,
				std::unordered_map<Math::IVec4, Chunk>* _chunks,
				const Math::Mat4x4& _modelView,
				ChunkBuilder* _builder,
				double _gameTime) :
			camera(_camera), model(_model), chunks(_chunks),
			modelView(_modelView),
			builder(_builder),
			gameTime(_gameTime)
		{}

		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
	//		if( !IsSolid( _type ) ) return false;

			// Compute a world space position
			float chunkLength = float(1 << _position[3]);
			Sphere boundingSphere(Vec3(_position[0] * chunkLength, _position[1] * chunkLength, _position[2] * chunkLength), 0.87f * chunkLength);
			boundingSphere.m_center += chunkLength * 0.5f;
			boundingSphere.m_center = boundingSphere.m_center * modelView;

			// View frustum culling
			if( !camera.IsVisible( boundingSphere ) )
				return false;

			// LOD - calculate a target level. If the current level is less or
			// equal the target draw. Where targetLOD is the level of the
			// required root node level. The resulting chunk will be up to 5
			// levels more tessellated
			//float detailResolution = 0.31f * log( lengthSq(boundingSphere.m_center - camera.GetPosition()) );
			float detailResolution = 0.021f * sq(log( lengthSq(boundingSphere.m_center) ));
			//float detailResolution = 0.045f * pow(log( lengthSq(boundingSphere.m_center - camera.GetPosition()) ), 1.65f);
			int targetLOD = max(2, Math::ceil(detailResolution));
			if( _position[3] <= targetLOD )
			{
				// For very far objects a chunk might be too detailed. In this case
				// a coarser level is used (usually 5 -> 32^3 chunks)
				int levels = max(0, 5 - (targetLOD - _position[3]));
				// Encode in position -> part of the key / hash
				IVec4 position(_position);
				position[3] |= levels << 16;
				auto chunk = chunks->find(position);
				if( chunk == chunks->end() )
				{
					// Chunk does not exist -> create
					chunk = chunks->insert(
						std::make_pair(position, std::move(Chunk(model, _position, levels)))
						).first;
					builder->RecomputeVertexBuffer(chunk->second);
				} else Assert( !_node->Data().IsDirty(), "Node-data was not changed."); //if( _node->Data().IsDirty() )
					//builder->RecomputeVertexBuffer(chunk->second);
				// There are empty inner chunks
				if( chunk->second.NumVoxels() > 0 )
				{
					RenderStat::g_numVoxels += chunk->second.NumVoxels();
					RenderStat::g_numChunks++;
					chunk->second.Draw( modelView, camera.GetProjection(), gameTime );
				}
				return false;
			}
			return true;
		}
	};

	// ********************************************************************* //
	void Model::Draw( const Input::Camera& _camera, double _gameTime )
	{
		// Delete all invalid and old chunks
		ClearChunkCache( _gameTime );

		// Create a new model space transformation
		Math::Mat4x4 modelView;
		GetModelMatrix( modelView, _camera );

		// Iterate through the octree and render chunks depending on the lod.
		ChunkBuilder* builder = new ChunkBuilder(); // TEMP -> in job verschieben
		DecideToDraw param( _camera, &this->m_voxelTree, &this->m_chunks, modelView, builder, _gameTime );
		m_voxelTree.Traverse( param );
		delete builder;
	}

	// ********************************************************************* //
	VoxelType Model::Get( const Math::IVec3& _position, int _level ) const
	{
		auto node = m_voxelTree.Get(_position, _level);
		if( node ) return node->Data().type;
		
		return VoxelType::UNDEFINED;
	}



	// ********************************************************************* //
	Math::Mat4x4& Model::GetModelMatrix( Math::Mat4x4& _out, const Math::Transformation& _reference ) const
	{
		_out = Mat4x4::Translation(-m_center) * GetTransformation(_reference);
		return _out;
	}

	Math::Mat4x4& Model::GetModelMatrix( Math::Mat4x4& _out, const Input::Camera& _reference ) const
	{
		_out = Mat4x4::Translation(-m_center) * GetTransformation(_reference);
		return _out;
	}


	// ********************************************************************* //
	void Model::Update( const Math::IVec4& _position, const Component& _oldType, const Component& _newType )
	{
		// Compute real volume from logarithmic size
		int size = 1 << _position[3];
		Vec3 center = Math::IVec3(_position) + size * 0.5f;
		size = size * size * size;

		// Remove the old voxel
		if( TypeInfo::GetMass(_oldType.type) > 0.0f )
		{
			float oldMass = TypeInfo::GetMass(_oldType.type) * size;
			m_center = (m_center * m_mass - center * oldMass) / (m_mass - oldMass);
			m_mass -= oldMass;
			m_numVoxels -= size;
		}

		// Add new voxel
		if( TypeInfo::GetMass(_newType.type) )
		{
			float newMass = TypeInfo::GetMass(_newType.type) * size;
			m_center = (m_center * m_mass + center * newMass) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += size;
		}

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		// TODO remove Math::Vector if replaced by template
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, Math::length(Math::Vector<3,float>(m_center) - Math::Vector<3,int>(_position)) );
	}

	// ********************************************************************* //
	bool Model::RayCast( const Math::WorldRay& _ray, int _targetLevel, ModelData::HitResult& _hit ) const
	{
		// Convert ray to model space
		Ray ray( _ray, *this );
		ray.m_origin += GetCenter();
		// TODO: Mat4x4::Scaling(m_scale) translation relevant?
		return m_voxelTree.RayCast(ray, _targetLevel, _hit);
	}

	// ********************************************************************* //
	void Model::ClearChunkCache( double _gameTime )
	{
		for( auto it = m_chunks.begin(); it != m_chunks.end(); )
		{
			// Delete a chunk if it is not used in the last time
			Chunk& chunk = it->second;
			if( chunk.IsNotUsedLately(_gameTime) ) it = m_chunks.erase( it );
			else {
				// Get the node to check if it is dirty
				Model::ModelData::SVON* node = m_voxelTree.Get( IVec3(chunk.m_root), chunk.m_root[3] );
				if( !node || node->Data().IsDirty() )
					it = m_chunks.erase( it );
				// Increase iterator only if nothing was deleted - deleting sets
				// the iterator to the next element anyway.
				else ++it;
			}
		}
	}

};