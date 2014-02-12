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
	struct DrawParam
	{
		const Input::Camera& camera;	// Required for culling and LOD
		Model* model;					// Create or find chunks here.
		Graphic::UniformBuffer& objectConstants;
		const Math::Mat4x4& modelTransform;
		const Math::Mat4x4& modelViewProjection;
		ChunkBuilder* builder;

		DrawParam(const Input::Camera& _camera, Model* _model,
				Graphic::UniformBuffer& _objectConstants,
				const Math::Mat4x4& _modelViewProjection,
				const Math::Mat4x4& _modelTransform,
				ChunkBuilder* _builder) :
			camera(_camera), model(_model),
			objectConstants(_objectConstants),
			modelTransform(_modelTransform),
			modelViewProjection(_modelViewProjection),
			builder(_builder)
		{}
	};

	bool Model::DecideToDraw(const Math::IVec4& _position, VoxelType _type, bool _hasChildren, DrawParam* _param)
	{
//		if( !IsSolid( _type ) ) return false;

		// Compute a world space position
		float chunkLength = float(1 << _position[3]);
		Sphere boundingSphere(Vec3(_position[0] * chunkLength, _position[1] * chunkLength, _position[2] * chunkLength), 0.87f * chunkLength);
		boundingSphere.m_center += chunkLength * 0.5f;
		boundingSphere.m_center = boundingSphere.m_center * _param->modelTransform;

		// View frustum culling
		if( !_param->camera.IsVisible( boundingSphere ) )
			return false;

		// LOD - calculate a target level. If the current level is less or
		// equal the target draw.
		float detailResolution = 0.35f * log( lengthSq(boundingSphere.m_center - _param->camera.GetPosition()) );
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
			auto chunk = _param->model->m_chunks.find(position);
			if( chunk == _param->model->m_chunks.end() )
			{
				// Chunk does not exist -> create
				chunk = _param->model->m_chunks.insert(
					std::make_pair(position, std::move(Chunk(_param->model, _position, levels)))
					).first;
				_param->builder->RecomputeVertexBuffer(chunk->second);
			}
			// There are empty inner chunks
			if( chunk->second.NumVoxels() > 0 )
			{
				RenderStat::g_numVoxels += chunk->second.NumVoxels();
				RenderStat::g_numChunks++;
				chunk->second.Draw( _param->objectConstants, _param->modelViewProjection );
			}
			return false;
		}
		return true;
	}

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera )
	{

		// Create a new model space transformation
		Math::Mat4x4 modelTransform = Mat4x4::Translation(-m_center) * Mat4x4::Rotation(m_rotation) * Mat4x4::Translation( m_position + m_center );
		Math::Mat4x4 modelViewProjection = modelTransform * _camera.GetViewProjection();

		// Iterate through the octree and render chunks depending on the lod.
		ChunkBuilder* builder = new ChunkBuilder(); // TEMP -> in job verschieben
		DrawParam param(_camera, this, _objectConstants, modelViewProjection, modelTransform, builder);
		m_voxelTree.Traverse( DecideToDraw, &param );
		delete builder;
	}

	// ********************************************************************* //
	VoxelType Model::Get( const Math::IVec3& _position, int _level ) const
	{
		VoxelType type;
		if( m_voxelTree.Get(_position, _level, type) ) return type;
		else return VoxelType::NONE;

		return type;		
	}


	// ********************************************************************* //
	void Model::Update( const Math::IVec4& _position, VoxelType _oldType, VoxelType _newType )
	{
		// Compute real volume from logarithmic size
		int size = 1 << _position[3];
		size = size * size * size;

		// Remove the old voxel
		if( IsSolid(_oldType) )
		{
			float oldMass = VOXEL_INFO[int(_oldType)].mass * size;
			// TODO remove Math::Vec3 if replaced by template
			m_center = (m_center * m_mass - Math::Vec3(_position * oldMass)) / m_mass;
			m_mass -= oldMass;
			m_numVoxels -= size;
		}

		// Add new voxel
		if( IsSolid(_newType) )
		{
			float newMass = VOXEL_INFO[int(_newType)].mass * size;
			m_center = (m_center * m_mass + Math::Vec3(_position * newMass)) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += size;
		}

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		// TODO remove Math::Vector if replaced by template
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, Math::length(Math::Vector<3,float>(m_center) - Math::Vector<3,int>(_position)) );
	}

};