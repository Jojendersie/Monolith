#include "model.hpp"
#include "voxel.hpp"
#include <cstdlib>
#include "../input/camera.hpp"
#include "../algorithm/smallsort.hpp"

using namespace Math;

namespace Voxel {

	Model::Model() :
		m_chunks(nullptr),
		m_numChunks(0),
		m_numVoxels(0),
		m_position(0.0f),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f),
		m_SVONAllocator(sizeof(SVON)*8),
		m_voxelTree(nullptr),
		m_rootSize(-1),
		m_rootPosition(0)
	{
	}

	Model::~Model()
	{
		for( int i=0; i<m_numChunks; ++i )
			delete m_chunks[i];
		free(m_chunks);
	}

	Model::SVON* Model::NewSVON()
	{
		SVON* pNew = (SVON*)m_SVONAllocator.Alloc();
		for(int i=0; i<8; ++i)
			new (&pNew[i]) SVON;
		return pNew;
	}

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Mat4x4 mModelViewProjection = Mat4x4::Translation(-m_center) * Mat4x4::Rotation(m_rotation) * Mat4x4::Translation( m_position + m_center ) * _camera.GetViewProjection();

		// Draw all chunks
		for( int i=0; i<m_numChunks; ++i )
		{
			m_chunks[i]->Draw( _objectConstants, mModelViewProjection, _camera, m_position );
		}
	}

	void Model::Set( const Math::IVec3& _position, int _level, VoxelType _type )
	{
		// Compute if the position is inside the current tree
		if(m_rootSize == -1)
		{
			// First set at all
			m_rootSize = _level;
			m_voxelTree = NewSVON();
			m_rootPosition = _position >> 1;
		}
		int scale = m_rootSize-_level;
		Math::IVec3 position = _position>>scale;
		while( (scale < 0)
			|| (((position.x-m_rootPosition.x*2) & 0xfffffffe) != 0)
			|| (((position.y-m_rootPosition.y*2) & 0xfffffffe) != 0)
			|| (((position.z-m_rootPosition.z*2) & 0xfffffffe) != 0))
		{
			// scale < 0: Obviously the target voxel is larger than the
			//	current root -> create new larger root(set)
			// pos: The new voxel could be outside -> requires larger root
			//	too because the tree must cover a larger area.
			SVON* pNew = NewSVON();
			pNew[(m_rootPosition.x & 1) + (m_rootPosition.y & 1) * 2 + (m_rootPosition.z & 1) * 4].children = m_voxelTree;
			m_rootPosition >>= 1;
			m_voxelTree = pNew;
			++m_rootSize;
			// Still to small?
			++scale;
			position >>= 1;
		}

		// One of the eight children must contain the target position.
		// Most things from ComputeChildIndex are already computed
		// -> use last line inline.
		m_voxelTree[ (position.x & 1) + (position.y & 1) * 2 + (position.z & 1) * 4 ]
				.Set(m_rootSize, _position, _level, _type, this);

	}

	// ********************************************************************* //
	void Model::Update( const Math::IVec3& _position, int _size, VoxelType _oldType, VoxelType _newType )
	{
		// Compute real volume from logarithmic size
		_size = 1 << _size;
		_size = _size * _size * _size;

		// Remove the old voxel
		if( _oldType != VoxelType::NONE && _oldType != VoxelType::UNDEFINED)
		{
			float oldMass = VOXEL_INFO[int(_oldType)].mass * _size;
			m_center = (m_center * m_mass - _position * oldMass) / m_mass;
			m_mass -= oldMass;
			m_numVoxels -= _size;
		}

		// Add new voxel
		if( _newType != VoxelType::NONE && _oldType != VoxelType::UNDEFINED)
		{
			float newMass = VOXEL_INFO[int(_newType)].mass * _size;
			m_center = (m_center * m_mass + _position * newMass) / (m_mass + newMass);
			m_mass += newMass;
			m_numVoxels += _size;
		}

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
			if( children ) RemoveSubTree(_position, _size, _model, true);
			// Physical update of this voxel
			_model->Update(_position, _size, type, _type);
			type = _type;
		} else {
			// Go into recursion
			int childIndex = ComputeChildIndex(_position,_size,_currentSize-1);
			if( !children )
			{ // Create new children
				children = _model->NewSVON();
				// Set all children to the same type as this node.
				// It was uniform before!
				for(int i=0; i<8; ++i) children[i].type = type;
			}
			children[childIndex].Set(_currentSize-1, _position, _size, _type, _model);
			// It could be that the children were deleted or are of the same
			// type now.
			if( IsUniform() )
			{
				// To do the model update the current position must be computed.
				int scale = _currentSize-_size;
				Math::IVec3 position = _position>>scale;
				// A uniform node does not need its children.
				RemoveSubTree(position, _currentSize, _model, false);
				//_model->Update(position, _currentSize, type, _type);
				type = _type;
			} else {
				// Update the current type based one the (maybe) changed child types.
				type = MajorVoxelType();
			}
		}
	}

	// ********************************************************************* //
	void Model::SVON::RemoveSubTree(const Math::IVec3& _position, int _size, Model* _model, bool _removePhysically)
	{
		assert( _size >= 0 );
		assert( children );
		for(int i=0; i<8; ++i)
		{
			// Recursive
			if( children[i].children )
				children[i].RemoveSubTree(_position+CHILD_OFFSETS[i], _size-1, _model, _removePhysically);
			else if(_removePhysically)
				_model->Update(_position+CHILD_OFFSETS[i], _size-1, type, VoxelType::NONE);
		}
		// Delete
		_model->m_SVONAllocator.Free(children);
		children = nullptr;
	}

	// ********************************************************************* //
	bool Model::SVON::IsUniform()
	{
		if( !children ) return true;	// Uniform the same as the parent.
		VoxelType type = children[0].type;
		for( int i=1; i<8; ++i )
			if( children[i].type != type ) return false;
		return true;
	}

	// ********************************************************************* //
	VoxelType Model::SVON::MajorVoxelType()
	{
		assert( children );
		VoxelType types[8];
		// Enumerate and sort all types of the child voxels
		int num = 0;
		for(int i=0; i<8; ++i) if(children[i].type != VoxelType::NONE && children[i].type != VoxelType::UNDEFINED)
			types[num++] = children[i].type;
		assert( num > 0 );
		Algo::SmallSort(types, num);
		// Simple iterate over the type array and count which type has the
		// most elements.
		int count0 = 0;		// Previous major
		int count1 = 1;		// Current element counter
		VoxelType t = types[0];
		for(int i=1; i<num; ++i )
		{
			if( types[i] == types[i-1] ) ++count1;
			else {
				if( count1 > count0 ) {t=types[i-1]; count0 = count1;}
				count1 = 1;
			}
		}
		return t;
	}

	// ********************************************************************* //
	int Model::SVON::ComputeChildIndex(const Math::IVec3& _targetPosition, int _targetSize, int _childSize)
	{
		// Find out the correct position
		int scale = _childSize-_targetSize;
		Math::IVec3 position = _targetPosition>>scale;
		// Now compute the index from the position
		return (position.x & 1) + (position.y & 1) * 2 + (position.z & 1) * 4;
	}


	// ********************************************************************* //
	size_t Model::SVON::MemoryConsumtion() const
	{
		size_t size = 0;	// Count this-size at parent
		if( children ) {
			size = 8*sizeof(SVON);	// The child array
			for( int i=0; i<8; ++i )
				size += children[i].MemoryConsumtion();
		}
		return size;//*/
		// Theoretic model
		/*size_t size = sizeof(SVON)+1;
		if( children ) {
			for( int i=0; i<8; ++i )
				if( children[i].type != VoxelType::NONE && children[i].type != VoxelType::UNDEFINED )
					size += children[i].MemoryConsumtion();
		}
		return size;//*/
	}
};