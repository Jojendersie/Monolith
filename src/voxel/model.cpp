#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "../input/camera.hpp"
#include "../graphic/uniformbuffer.hpp"
#include <iostream>

using namespace Math;

namespace RenderStat {
	extern int g_numVoxels;
	extern int g_numChunks;
}

namespace Voxel {

	Model::Model() :
		m_numVoxels(0),
		m_position(0.0f),
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
		Graphic::UniformBuffer& objectConstants;
		const Math::Mat4x4& modelTransform;
		const Math::Mat4x4& modelView;
		ChunkBuilder* builder;

		DecideToDraw(const Input::Camera& _camera,
				Model::ModelData* _model,
				std::unordered_map<Math::IVec4, Chunk>* _chunks,
				Graphic::UniformBuffer& _objectConstants,
				const Math::Mat4x4& _modelView,
				const Math::Mat4x4& _modelTransform,
				ChunkBuilder* _builder) :
			camera(_camera), model(_model), chunks(_chunks),
			objectConstants(_objectConstants),
			modelTransform(_modelTransform),
			modelView(_modelView),
			builder(_builder)
		{}

		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
	//		if( !IsSolid( _type ) ) return false;

			// Compute a world space position
			float chunkLength = float(1 << _position[3]);
			Sphere boundingSphere(Vec3(_position[0] * chunkLength, _position[1] * chunkLength, _position[2] * chunkLength), 0.87f * chunkLength);
			boundingSphere.m_center += chunkLength * 0.5f;
			boundingSphere.m_center = boundingSphere.m_center * modelTransform;

			// View frustum culling
			if( !camera.IsVisible( boundingSphere ) )
				return false;

			// LOD - calculate a target level. If the current level is less or
			// equal the target draw.
			float detailResolution = 0.35f * log( lengthSq(boundingSphere.m_center - camera.GetPosition()) );
			//float detailResolution = 0.025f * sqr(log( lengthSq(boundingSphere.m_center - _param->camera.GetPosition()) ));
				//pow((chunkPos - _param->camera.GetPosition()).Length(), 0.25f);
			int targetLOD = max(5, Math::ceil(detailResolution));
	//		std::cout << targetLOD << '\n';
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
				} else if( _node->Data().IsDirty() )
					builder->RecomputeVertexBuffer(chunk->second);
				// There are empty inner chunks
				if( chunk->second.NumVoxels() > 0 )
				{
					RenderStat::g_numVoxels += chunk->second.NumVoxels();
					RenderStat::g_numChunks++;
					chunk->second.Draw( objectConstants, modelView, camera.GetProjection() );
				}
				return false;
			}
			return true;
		}
	};

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera )
	{
		// Create a new model space transformation
		Math::Mat4x4 modelTransform = Mat4x4::Translation(-m_center) * Mat4x4::Rotation(m_rotation) * Mat4x4::Translation( m_position + m_center );
		Math::Mat4x4 modelView = modelTransform * _camera.GetView();

		// Iterate through the octree and render chunks depending on the lod.
		ChunkBuilder* builder = new ChunkBuilder(); // TEMP -> in job verschieben
		DecideToDraw param(_camera, &this->m_voxelTree, &this->m_chunks, _objectConstants, modelView, modelTransform, builder);
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
	void Model::Update( const Math::IVec4& _position, const Component& _oldType, const Component& _newType )
	{
		// Compute real volume from logarithmic size
		int size = 1 << _position[3];
		Vec3 center = Math::IVec3(_position) + size * 0.5f;
		size = size * size * size;

		// Remove the old voxel
		if( IsSolid(_oldType.type) )
		{
			float oldMass = VOXEL_INFO[int(_oldType.type)].mass * size;
			m_center = (m_center * m_mass - center * oldMass) / (m_mass - oldMass);
			m_mass -= oldMass;
			m_numVoxels -= size;
		}

		// Add new voxel
		if( IsSolid(_newType.type) )
		{
			float newMass = VOXEL_INFO[int(_newType.type)].mass * size;
			m_center = (m_center * m_mass + center * newMass) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += size;
		}

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		// TODO remove Math::Vector if replaced by template
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, Math::length(Math::Vector<3,float>(m_center) - Math::Vector<3,int>(_position)) );
	}

	// ********************************************************************* //
	bool Model::RayCast( const Math::Ray& _ray, int _targetLevel, ModelData::HitResult& _hit ) const
	{
		// Convert ray to model space
		Math::Mat4x4 inverseModelTransform = Mat4x4::Translation( -m_position - m_center ) * Mat4x4::Rotation(-m_rotation) * Mat4x4::Translation(m_center);
		Math::Ray ray = _ray * inverseModelTransform;
		return m_voxelTree.RayCast(ray, _targetLevel, _hit);
	}

};