#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "../input/camera.hpp"
#include "../graphic/uniformbuffer.hpp"
#include <iostream>

using namespace Math;

namespace Voxel {

	Model::Model() :
		m_numVoxels(0),
		m_position(0.0f),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f),
		m_voxelTree(VoxelType::UNDEFINED, this)
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
		int numVoxels;

		DrawParam(const Input::Camera& _camera, Model* _model,
				Graphic::UniformBuffer& _objectConstants,
				const Math::Mat4x4& _modelViewProjection,
				const Math::Mat4x4& _modelTransform) :
			camera(_camera), model(_model),
			objectConstants(_objectConstants),
			modelTransform(_modelTransform),
			modelViewProjection(_modelViewProjection),
			numVoxels(0)
		{}
	};

	bool Model::DecideToDraw(const Math::IVec4& _position, VoxelType _type, DrawParam* _param)
	{
		if( !IsSolid( _type ) ) return false;

		// Compute a world space position
		float chunkLength = float(1 << _position[3]);
		Vec3 chunkPos(_position[0] * chunkLength, _position[1] * chunkLength, _position[2] * chunkLength);
		chunkPos = chunkPos * _param->modelTransform;

		// TODO: culling

		// LOD - calculate a target level. If the current level is less or
		// equal the target draw.
		float distance = pow((chunkPos - _param->camera.GetPosition()).Length(), 0.25f);
		int targetLOD = max(5, Math::ceil(distance / 1.0f));
//		std::cout << targetLOD << '\n';
		if( _position[3] <= targetLOD )
		{
			auto chunk = _param->model->m_chunks.find(_position);
			if( chunk == _param->model->m_chunks.end() )
			{
				// Chunk does not exist -> create
				chunk = _param->model->m_chunks.insert(
					std::make_pair(_position, std::move(Chunk(_param->model, _position)))
					).first;
			}
			// There are empty inner chunks
			if( chunk->second.NumVoxels() > 0 )
			{
				_param->numVoxels += chunk->second.NumVoxels();
				chunk->second.Draw( _param->objectConstants, _param->modelViewProjection );
			}
			return false;
		}
		return true;
	}

	int Model::Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera )
	{

		// Create a new model space transformation
		Math::Mat4x4 modelTransform = Mat4x4::Translation(-m_center) * Mat4x4::Rotation(m_rotation) * Mat4x4::Translation( m_position + m_center );
		Math::Mat4x4 modelViewProjection = modelTransform * _camera.GetViewProjection();

		// Iterate through the octree and render chunks depending on the lod.
		DrawParam param(_camera, this, _objectConstants, modelViewProjection, modelTransform);
		m_voxelTree.Traverse( DecideToDraw, &param );

		return param.numVoxels;
	}

	// ********************************************************************* //
	VoxelType Model::Get( const Math::IVec3& _position, int _level ) const
	{
		VoxelType type = m_voxelTree.Get(_position, _level);

		// TODO Search in blue print.
		if( type == VoxelType::UNDEFINED ) type = VoxelType::NONE;

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