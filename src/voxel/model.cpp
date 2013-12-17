#include "model.hpp"
#include "chunk.hpp"
#include <cstdlib>
#include "../input/camera.hpp"

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
	}

	Model::~Model()
	{
	}

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Mat4x4 mModelViewProjection = Mat4x4::Translation(-m_center) * Mat4x4::Rotation(m_rotation) * Mat4x4::Translation( m_position + m_center ) * _camera.GetViewProjection();

		// Iterate through the octree and render chunks depending on the lod.
		// TODO

		// Draw all chunks
	/*	for( int i=0; i<m_numChunks; ++i )
		{
			m_chunks[i]->Draw( _objectConstants, mModelViewProjection, _camera, m_position );
		}*/
	}

	// ********************************************************************* //
	void Model::Update( const Math::IVec3& _position, int _size, VoxelType _oldType, VoxelType _newType )
	{
		// Compute real volume from logarithmic size
		_size = 1 << _size;
		_size = _size * _size * _size;

		// Remove the old voxel
		if( IsSolid(_oldType) )
		{
			float oldMass = VOXEL_INFO[int(_oldType)].mass * _size;
			m_center = (m_center * m_mass - _position * oldMass) / m_mass;
			m_mass -= oldMass;
			m_numVoxels -= _size;
		}

		// Add new voxel
		if( IsSolid(_newType) )
		{
			float newMass = VOXEL_INFO[int(_newType)].mass * _size;
			m_center = (m_center * m_mass + _position * newMass) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += _size;
		}

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, (m_center - _position).Length() );
	}

};