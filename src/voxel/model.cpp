#include "model.hpp"
#include "voxel.hpp"
#include <cstdlib>
#include "../input/camera.hpp"

using namespace Math;

namespace Voxel {

	Model::Model() :
		m_chunks(nullptr),
		m_numChunks(0),
		m_position(0.0f),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f)
	{
	}

	Model::~Model()
	{
		for( int i=0; i<m_numChunks; ++i )
			delete m_chunks[i];
		free(m_chunks);
	}

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Matrix mModelViewProjection = MatrixTranslation(-m_center) * MatrixRotation(m_rotation) * MatrixTranslation( m_position + m_center ) * _camera.GetViewProjection();

		// Draw all chunks
		for( int i=0; i<m_numChunks; ++i )
		{
			m_chunks[i]->Draw( _objectConstants, mModelViewProjection, _camera, m_position );
		}
	}

	void Model::Set( const Math::IVec3& _position, int _level, VoxelType _type )
	{
		// Compute which chunk is searched. The chunk position is in units of
		// the smallest voxels but rounded to 32 voxel alignment.
		IVec3 chunkPos = _position / (1<<_level);
		IVec3 posInsideChunk = _position - chunkPos * (1<<_level);
		chunkPos *= 32;

		Chunk* targetChunk = nullptr;

		// Search the correct chunk
		for( int i=0; i<m_numChunks; ++i )
		{
			if( m_chunks[i]->GetPosition() == chunkPos )
			{
				// Add, update and ready
				targetChunk = m_chunks[i];
				break;
			}
		}

		// Nothing found create a new chunk
		if( !targetChunk )
		{
			m_chunks = (Chunk**)realloc(m_chunks, sizeof(Chunk*) * (m_numChunks+1));
			targetChunk = m_chunks[m_numChunks] = new Chunk();
			targetChunk->SetPosition( chunkPos );
			++m_numChunks;
		}

		targetChunk->Set(posInsideChunk, _level, _type, nullptr);

		// Update mass center
		if( m_mass == 0.0f )
			m_center = Vec3(_position);
		else
			m_center = (m_center * m_mass + _position * VOXEL_INFO[int(_type)].mass) / (m_mass + VOXEL_INFO[int(_type)].mass);
		m_mass += VOXEL_INFO[int(_type)].mass;

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, (m_center - _position).Length() );
	}


	// ********************************************************************* //
	static const IVec3 CHILD_OFFSETS[8] = { IVec3(0,0,0), IVec3(0,0,1), IVec3(0,1,0), IVec3(0,1,1),
					   IVec3(1,0,0), IVec3(1,0,1), IVec3(1,1,0), IVec3(1,1,1) };

	// ********************************************************************* //
	void Model::SVON::Set(int _currentSize, const Math::IVec3& _position, int _size, VoxelType _type, Model* _model)
	{
		if( _currentSize == _size )
		{
			// This is the target voxel. Delete everything below.
			if( children ) RemoveSubTree(_position, _size, _model);
			// Physical update of this voxel
			_model->Update(_position, _size, type, _type);
			type = _type;
		} else {
			// Go into recursion
			int childIndex = ComputeChildIndex(_position,_size,_currentSize-1);
			if( !children )
			{ // Create new children
				children = (SVON*)_model->m_SVONAllocator.Alloc();
				// Set all children to the same type as this node.
				for(int i=0; i<8; ++i) { children[i].type = type; children[i].children = nullptr; }
			}
			children[childIndex].Set(_currentSize-1, _position, _size, _type, _model);
			// It could be that the children were deleted or are of the same
			// type now.
			if( IsUniform() )
			{
				// To do the model update the current position must be computed.
				int scale = _currentSize-_size;
				Math::IVec3 position(_position.x>>scale, _position.y>>scale, _position.z>>scale);
				// A uniform node does not need its children.
				if( children ) RemoveSubTree(position, _currentSize, _model);
				_model->Update(position, _currentSize, type, _type);
				type = _type;
			}
		}
	}

	// ********************************************************************* //
	void Model::SVON::RemoveSubTree(const Math::IVec3& _position, int _size, Model* _model)
	{
		assert( _size >= 0 );
		assert( children );
		for(int i=0; i<8; ++i)
		{
			// Recursive
			children[i].RemoveSubTree(_position+CHILD_OFFSETS[i], _size-1, _model);
			// Delete
			_model->Update(_position+CHILD_OFFSETS[i], _size-1, type, VoxelType::NONE);
			_model->m_SVONAllocator.Free(children);
		}
	}

	// ********************************************************************* //
	bool Model::SVON::IsUniform()
	{
		if( !children ) return true;	// Uniform none.
		VoxelType type = children[0].type;
		for( int i=1; i<8; ++i )
			if( children[0].type != type ) return false;
		return true;
	}

	// ********************************************************************* //
	int Model::SVON::ComputeChildIndex(const Math::IVec3& _targetPosition, int _targetSize, int _childSize)
	{
		// Find out the correct position
		int scale = _childSize-_targetSize;
		Math::IVec3 position(_targetPosition.x>>scale, _targetPosition.y>>scale, _targetPosition.z>>scale);
		// Now compute the index from the position
		return (_targetPosition.x & 1) + (_targetPosition.y & 1) * 2 + (_targetPosition.z & 1) * 4;
	}
};