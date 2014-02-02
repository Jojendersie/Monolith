#pragma once

#include "../math/vector.hpp"
#include "../math/ray.hpp"
#include "../math/intersection.hpp"
#include "../algorithm/smallsort.hpp"
#include <hybridarray.hpp>
#include <poolallocator.hpp>

namespace Voxel {

	/// \brief A generic sparse octree implementation.
	/// \details Each node contains data and there are special operations
	///		assuming that the data are voxels.
	///		
	///		* T is the type of voxel data.
	///		* Listener Must have an Update() method which is called if voxels are changed
	template<typename T, typename Listener>
	class SparseVoxelOctree
	{
	public:
		/// \brief Create an empty octree.
		/// \param [in] _defaultData Initial value used when something must be
		///		set back.
		SparseVoxelOctree( T _defaultData, Listener* _listener );

		/// \brief Set a voxel in the octree and call update of the listener.
		/// \details This method overwrites all covered voxels.
		/// \param [in] _position Position inside the given level.
		/// \param [in] _level Depth in the grid hierarchy. 0 is the maximum
		///		resolution of the voxel grid. The size is the logarithmic edge
		///		length of voxels (2^0).
		/// \param [in] _type Voxel to be set. A voxel equal to _defaultData can
		///		delete a voxel.
		void Set( const Math::IVec3& _position, int _level, T _type );

		/// \brief Returns the type of a voxel on a certain grid level and
		///		position.
		///	\details If the position is outside the return value is _defaultData. For
		///		levels other than 0 the returned value will be some
		///		approximating LOD (majority) of the children.
		T Get( const Math::IVec3& _position, int _level ) const;

		/// \brief Test if a property is fulfilled for all children and get the
		///		voxel data.
		/// \details Applies a predicate too all children of a specified voxel.
		///		Only if all children fulfill the predicate the return value is
		///		true.
		///		If the node does not have children the return value is
		///		_predicate(_voxelOut). This means all children are assumed to
		///		be the same as the parent (uniform node).
		///		
		///		Additionally the type of the voxel is returned.
		/// \param [in] _position Position inside the given level. If the
		///		position is outside the return value is false and the voxel
		///		type is the default value.
		/// \param [in] _level Depth in the grid hierarchy.
		/// \param [in] _predicate The unary predicate function.
		/// \param [out] _voxelOut Return of the voxels type itself -
		///		which is a gratis get.
		bool IsEachChild( const Math::IVec3& _position, int _level, bool(*_predicate)(T), T& _voxelOut ) const;

		/// \brief Test if a property is fulfilled for any child and get the
		///		voxel data.
		///	\details similar to SparseVoxelOctree::IsEachChild.
		bool IsAnyChild( const Math::IVec3& _position, int _level, bool(*_predicate)(T), T& _voxelOut ) const;

		/// \brief Traverse through the whole tree in preorder and call the
		///		callback.
		/// \param [in] _callback This function is called for each voxel. It
		///		has to return true if the children should be traversed too.
		///		If a voxel does not have children the return value is ignored.
		///
		///		The function takes the octree position, the voxel data,
		///		if the voxel has children, and the parameters passed.
		///	\param [in] _param Pointer to an arbitrary structure to pass
		///		additional information to the callback.
		template<typename Param>
		void Traverse( bool(*_callback)(const Math::IVec4&,T,bool,Param*), Param* _param ) const;

		/// \brief Full collision information in local coordinates
		struct HitResult
		{
			Math::IVec3 position;
			T voxel;
			
			/// \brief The side on which the returned voxel was hit.
			///	\details Left, Bottom and Front mean that the ray came from a smaller
			///		coordinate in x, y or z respective.
			Math::Intersect::Side side;
		};

		/// \brief Determine which voxel is hit first on which side from
		///		given ray.
		///	\param [in] _ray A ray in tree space.
		///	\param [in] _targetLevel Target level (>=0) where the voxel should
		///		be found. Common is to use 0 - the deepest level but if
		///		constructing with larger voxels it could also be useful to
		///		find the first nonempty voxel on a higher level.
		///	\param [out] _hit The first voxel on the target level which is non
		///		empty. This is only defined if there is a collision.
		///	\return true if there is a collision.
		bool RayCast( const Math::Ray& _ray, int _targetLevel, HitResult& _hit );

#ifdef _DEBUG
		size_t MemoryConsumption() const;
#endif


	protected:
		T m_defaultData;
		Listener* m_listener;

		/// \brief A Sparse-Voxel-Octree-Node
#		pragma pack(push,1)
		struct SVON {
			SVON* children;
			T voxel;

			SVON(T _defaultData) : children(nullptr), voxel(_defaultData)	{}

			/// \brief Set a voxel on a chosen resolution.
			/// \param [in] _currentSize The voxel size in the current recursion
			///		level. This can be used to compute the full actual position.
			///	\param [in] _position Position where to set the new voxel.
			///		_position[3] is the logarithmic size of the voxel to be set. 0 is
			///		the layer with the highest resolution.
			///	\param [in] _type The new type.
			/// \param [in] _model The model for all update steps if voxels are
			///		overwritten.
			static void Set(SVON* _this, int _currentSize, const Math::IVec4& _position,
				T _type, SparseVoxelOctree* _model);

			/// \brief Set all children to _defaultData to do an correct update and than
			///		delete the memory block.
			/// \param [in] _model The model for all update steps if voxels are
			///		overwritten.
			///	\param [in] _removePhysicaly Call the model update for each deleted voxel or not? 
			void RemoveSubTree(const Math::IVec4& _position, SparseVoxelOctree* _parent, bool _removePhysically);

			/// \brief Recursive pre-order traverse.
			/// \see SparseVoxelOctree::Traverse
			/// \param [in] _position Voxel position in 3D grid.
			/// \param [in] _size Grid size, 0 is the highest resolution.
			template<class Param>
			void Traverse( const Math::IVec4& _position, bool(*_callback)(const Math::IVec4&,T,bool,Param*), Param* _param, T _defaultData ) const;

			/// \brief Check if all children have the same type.
			bool IsUniform() const;

			/// \brief Computes which type has the majority for the 8 children.
			/// \details Nonsolids (tested with IsSolid()) are not counted.
			T MajorVoxelType();

			/// \brief Computes the child array index [0,7] assuming that the
			///		target position is within the current voxels children
			static int ComputeChildIndex(const Math::IVec4& _targetPosition, int _childSize);

			/// \copydoc SparseVoxelOctree::RayCast
			bool RayCast( const Math::Ray& _ray, Math::IVec3& _position, int _level,
				int _targetLevel, HitResult& _hit );

			/// \brief Temporarily method to benchmark the tree
#ifdef _DEBUG
			size_t MemoryConsumption() const;
#endif
		};
#		pragma pack(pop)

		/// \brief An own allocator for all nodes of this model's octree.
		/// \details Instead of single voxels this is always an array of 8.
		Jo::Memory::PoolAllocator m_SVONAllocator;

		/// \brief Use the pool allocator and call the constructor 8 times
		SVON* NewSVON();

		/// \brief A sparse voxel octree root.
		///	\details Each pointer points to a set of 8 children. So on root level there
		///		are always 8 nodes.		
		Math::IVec3 m_rootPosition;	///< Position of the root node in grid space of the node which covers all 8 roots.
		int m_rootSize;				///< Level of the node which contains the 8 roots where 0 is the highest possible resolution.
		SVON m_root;				///< The single top level root node

		/// \brief Internal getter which returns a node reference.
		const SVON* _Get( const Math::IVec3& _position, int _level ) const;

		/// \brief Used to sort children after a floating point number
		struct TmpCollisionEntry
		{
			float value;
			int index;

			/// \brief Less then with respect to the value
			bool operator < ( const TmpCollisionEntry& _other ) const
			{
				return value < _other.value;
			}
		};
	};






	// ********************************************************************* //
	static const Math::IVec4 CHILD_OFFSETS[8] = { Math::IVec4(0,0,0,-1), Math::IVec4(1,0,0,-1), Math::IVec4(0,1,0,-1), Math::IVec4(1,1,0,-1),
		Math::IVec4(0,0,1,-1), Math::IVec4(1,0,1,-1), Math::IVec4(0,1,1,-1), Math::IVec4(1,1,1,-1) };

	// ********************************************************************* //
	template<typename T, typename Listener>
	SparseVoxelOctree<T,Listener>::SparseVoxelOctree( T _defaultData, Listener* _listener ) :
		m_defaultData(_defaultData),
		m_listener(_listener),
		m_SVONAllocator(sizeof(SVON)*8),
		m_root(_defaultData),
		m_rootSize(-1),
		m_rootPosition(0)
	{
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	typename SparseVoxelOctree<T,Listener>::SVON* SparseVoxelOctree<T,Listener>::NewSVON()
	{
		SVON* pNew = (SVON*)m_SVONAllocator.Alloc();
		for(int i=0; i<8; ++i)
			new (&pNew[i]) SVON(m_defaultData);
		return pNew;
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	void SparseVoxelOctree<T,Listener>::Set( const Math::IVec3& _position, int _level, T _type )
	{
		// Compute if the position is inside the current tree
		if(m_rootSize == -1)
		{
			// First set at all
			m_rootSize = _level;
			m_rootPosition = _position;
		}
		int scale = m_rootSize-_level;
		Math::IVec3 position = _position>>scale;
		while( (scale < 0)
			|| ((position-m_rootPosition) != Math::IVec3(0)) )
		{
			// scale < 0: Obviously the target voxel is larger than the
			//	current root -> create new larger root(set)
			// pos: The new voxel could be outside -> requires larger root
			//	too because the tree must cover a larger area.
			SVON* pNew = NewSVON();
			// Move the old root in one of the children
			int rootIndex = (m_rootPosition[0] & 1) + (m_rootPosition[1] & 1) * 2 + (m_rootPosition[2] & 1) * 4;
			pNew[rootIndex].children = m_root.children;
			pNew[rootIndex].voxel = m_root.voxel;
			// Set root to higher level
			m_rootPosition >>= 1;
			m_root.children = pNew;
			m_root.voxel = m_root.MajorVoxelType();
			++m_rootSize;
			// Still to small?
			++scale;
			position >>= 1;
		}

		// One of the eight children must contain the target position.
		SVON::Set(&m_root, m_rootSize, IVec4(_position, _level), _type, this);
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	typename const SparseVoxelOctree<T,Listener>::SVON* SparseVoxelOctree<T,Listener>::_Get( const Math::IVec3& _position, int _level ) const
	{
		// Get on an empty model? Would be better if this never happens ->
		// better performance because no 'if' on m_rootSize required.
		assert(m_rootSize != -1);

		// Special cases: not inside octree
		int scale = m_rootSize-_level;
		if( scale < 0 ) return nullptr;
		Math::IVec3 position = _position >> scale;
		if((position-m_rootPosition) != Math::IVec3(0))
			return nullptr;

		// Search in the octree (while not on target level or tree ends)
		const SVON* current = &m_root;
		while( (scale > 0) && current->children ) {
			--scale;
			int x = (_position[0] >> scale) & 1;
			int y = (_position[1] >> scale) & 1;
			int z = (_position[2] >> scale) & 1;
			current = &current->children[ x + y * 2 + z * 4 ];
		}

		return current;
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	T SparseVoxelOctree<T,Listener>::Get( const Math::IVec3& _position, int _level ) const
	{
		const SVON* node = _Get(_position, _level);
		return node ? node->voxel : m_defaultData;
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	bool SparseVoxelOctree<T,Listener>::IsEachChild( const Math::IVec3& _position, int _level, bool(*_predicate)(T), T& _voxelOut ) const
	{
		const SVON* node = _Get(_position, _level);

		if( !node ) { _voxelOut = m_defaultData; return false; }
		_voxelOut = node->voxel;
		if( !node->children ) return _predicate(node->voxel);

		// Test all children - stop if any does not have the searched property
		for( int i=0; i<8; ++i )
			if( !_predicate(node->children[i].voxel) ) return false;

		return true;
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	bool SparseVoxelOctree<T,Listener>::IsAnyChild( const Math::IVec3& _position, int _level, bool(*_predicate)(T), T& _voxelOut ) const
	{
		const SVON* node = _Get(_position, _level);

		if( !node ) { _voxelOut = m_defaultData; return false; }
		_voxelOut = node->voxel;
		if( !node->children ) return _predicate(node->voxel);

		// Test all children - stop if any does have the searched property
		for( int i=0; i<8; ++i )
			if( _predicate(node->children[i].voxel) ) return true;

		return false;
	}

	// ********************************************************************* //
	template<typename T, typename Listener> template<typename Param>
	void SparseVoxelOctree<T,Listener>::Traverse( bool(*_callback)(const Math::IVec4&,T,bool,Param*), Param* _param ) const
	{
		assert(m_rootSize != -1);

		m_root.Traverse(IVec4(m_rootPosition, m_rootSize), _callback, _param, m_defaultData);
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	bool SparseVoxelOctree<T,Listener>::RayCast( const Math::Ray& _ray, int _targetLevel, HitResult& _hit )
	{
		assert(m_rootSize != -1);

		// Use recursive algorithm.
		// TODO: what if m_rootSize < _targetLevel?
		return m_root.RayCast( _ray, m_rootPosition, m_rootSize, _targetLevel, _hit );
	}

	// ********************************************************************* //
#ifdef _DEBUG
	template<typename T, typename Listener>
	size_t SparseVoxelOctree<T,Listener>::MemoryConsumption() const
	{
		size_t sum = 0;
		for( int i=0; i<8; ++i )
			sum += m_roots[i].MemoryConsumption();
		return sum;
	}
#endif









	// ********************************************************************* //
	template<typename T, typename Listener>
	void SparseVoxelOctree<T,Listener>::SVON::Set(SVON* _this, int _currentSize,
		const Math::IVec4& _position,
		T _type, SparseVoxelOctree* _model)
	{
		// A stack with 'this' pointers for recursion. The recursion is unrolled
		// automatically because this allows 'longjump' optimizations. Not
		// everything must be updated after recursion and unrolling the stack
		// stops as early as possible.
		Jo::HybridArray<SVON*, 16> callStack;
		SVON* currentElement = _this;

		// Go downwards
		while( _currentSize > _position[3] )
		{
			int childIndex = ComputeChildIndex(_position, _currentSize-1);
			if( !currentElement->children )
			{ // Create new children
				currentElement->children = _model->NewSVON();
				// Set all children to the same type as this node.
				// It was uniform before!
				for(int i=0; i<8; ++i) currentElement->children[i].voxel = currentElement->voxel;
			}
			--_currentSize;
			callStack.PushBack(currentElement);
			currentElement = &currentElement->children[childIndex];
		}
		assert( _currentSize == _position[3] );

		// This is the target voxel. Delete everything below.
		if( currentElement->children ) currentElement->RemoveSubTree(_position, _model, true);
		// Physical update of this voxel
		_model->m_listener->Update(_position, currentElement->voxel, _type);
		currentElement->voxel = _type;

		// Unroll stack as long as the voxel type in hierarchy changes.
		bool typeChanged = true;
		while( typeChanged && callStack.Size()>0 )
		{
			currentElement = callStack.PopBack();
			// Was the same before and number of children with that type only
			// increased.
			typeChanged = currentElement->voxel != _type;

			// It could be that the children were deleted or are of the same
			// type now.
			if( currentElement->IsUniform() )
			{
				// To do the model update the current position must be computed.
				int scale = _currentSize-_position[3];
				int x = _position[0] >> scale;
				int y = _position[1] >> scale;
				int z = _position[2] >> scale;
				// A uniform node does not need its children.
				currentElement->RemoveSubTree(IVec4(x, y, z, _currentSize), _model, false);
				//_model->m_listener->Update(position, _currentSize, type, _type);
				assert( currentElement->voxel == _type );
			} else if(typeChanged) {
				// Update the current type based one the changed child types.
				// If the majority was already of the new type nothing can happen
				// (So typeChanged is necessary that something might happen).
				T oldType = currentElement->voxel;
				currentElement->voxel = currentElement->MajorVoxelType();
				// Still a change?
				typeChanged = currentElement->voxel != oldType;
			}
		}

	}


	// ********************************************************************* //
	template<typename T, typename Listener>
	void SparseVoxelOctree<T,Listener>::SVON::RemoveSubTree(const Math::IVec4& _position, SparseVoxelOctree* _parent, bool _removePhysically)
	{
		assert( _position[3] >= 0 );
		assert( children );
		for(int i=0; i<8; ++i)
		{
			// Recursive
			if( children[i].children )
				children[i].RemoveSubTree(_position+CHILD_OFFSETS[i], _parent, _removePhysically);
			else if(_removePhysically)
				_parent->m_listener->Update(_position+CHILD_OFFSETS[i], voxel, _parent->m_defaultData);
		}
		// Delete
		_parent->m_SVONAllocator.Free(children);
		children = nullptr;
	}

	// ********************************************************************* //
	template<typename T, typename Listener> template<class Param>
	void SparseVoxelOctree<T,Listener>::SVON::Traverse( const Math::IVec4& _position, bool(*_callback)(const Math::IVec4&,T,bool,Param*), Param* _param, T _defaultData ) const
	{
		if( _callback(_position,voxel,children!=nullptr,_param) && children )
		{
			Math::IVec4 position(_position[0]<<1, _position[1]<<1, _position[2]<<1, _position[3]);
			for( int i=0; i<8; ++i )
			{
				//if( children[i].voxel != _defaultData )
					children[i].Traverse(position + CHILD_OFFSETS[i], _callback, _param, _defaultData);
				//else
				//	assert( !children[i].children );
			}
		}
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	bool SparseVoxelOctree<T,Listener>::SVON::IsUniform() const
	{
		if( !children ) return true;	// Uniform the same as the parent.
		if( children[0].children ) return false;	// If a child has children the uniformness is undefined
		for( int i=1; i<8; ++i )
			if( children[i].voxel != children[0].voxel || children[i].children ) return false;
		return true;
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	T SparseVoxelOctree<T,Listener>::SVON::MajorVoxelType()
	{
		assert( children );
		T types[8];
		// Enumerate and sort all types of the child voxels
		int num = 0;
		for(int i=0; i<8; ++i)// if( IsSolid(children[i].voxel) )
			types[num++] = children[i].voxel;
		assert( num > 0 );
		Algo::SmallSort(types, num);
		// Simple iterate over the type array and count which type has the
		// most elements.
		int count0 = 0;		// Previous major
		int count1 = 1;		// Current element counter
		T t = types[0];
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
	template<typename T, typename Listener>
	int SparseVoxelOctree<T,Listener>::SVON::ComputeChildIndex(const Math::IVec4& _targetPosition, int _childSize)
	{
		// Find out the correct position
		int scale = _childSize-_targetPosition[3];
		int x = (_targetPosition[0] >> scale) & 1;
		int y = (_targetPosition[1] >> scale) & 1;
		int z = (_targetPosition[2] >> scale) & 1;
		// Now compute the index from the position
		return x + y * 2 + z * 4;
	}


	// ********************************************************************* //
	template<typename T, typename Listener>
	bool SparseVoxelOctree<T,Listener>::SVON::RayCast( const Math::Ray& _ray, Math::IVec3& _position, int _level,
		int _targetLevel, HitResult& _hit )
	{
		// Test the current cube
		if( children && !(_targetLevel==_level))
		{
			// Recursion required. We can user fast test without side detection.
			float t;
			if( Math::Intersect::RayAACube( _ray, _position, (1<<_level), t ) )
			{
				// Ray passes the current node. Make a sphere test for each
				// child.
				TmpCollisionEntry list[8];
				int num = 0;		// How many children have a collision?
				_position <<= 1;	// First child index
				float r = (1 << (_level - 1)) * 0.866025404f;	// * 0.5 * sqrt(3)
				for( int i=0; i<8; ++i )
				{
					if( Math::Intersect::RaySphere( _ray,
						Math::Sphere( _position + CHILD_OFFSETS[i], r ),
						list[num] ) )
					list[num++].index = i;
				}
				 
				// Sort the possible colliding children after range.
				Algo::SmallSort( list, num );

				// Test them sequentially and stop immediately if something was hit.
				for( int i=0; i<num; ++i )
					if( children[list[i].index]->RayCast( _ray, _position + CHILD_OFFSETS[list[i].index], _level-1, _hit ) )
						return true;

				// No child collides
				return false;
			}
		} else {
			// This is a leaf - if we hit we wanna know the side.
			if( Math::Intersect::RayAACube( _ray, _position, (1<<_level), _hit.side ) )
			{
				_hit.position = _position;
				_hit.voxel = voxel;
				return true;
			} else
				return false;
		}
	}


	// ********************************************************************* //
#ifdef _DEBUG
	template<typename T, typename Listener>
	size_t SparseVoxelOctree<T,Listener>::SVON::MemoryConsumption() const
	{
		size_t size = 0;	// Count this-size at parent
		if( children ) {
			size = 8*sizeof(SVON);	// The child array
			for( int i=0; i<8; ++i )
				size += children[i].MemoryConsumption();
		}
		return size;//*/
		// Theoretic model
		/*size_t size = sizeof(SVON)+1;
		if( children ) {
			for( int i=0; i<8; ++i )
				if( IsSolid(children[i].voxel) )
					size += children[i].MemoryConsumtion();
		}
		return size;//*/
	}
#endif
} // namespace Voxel