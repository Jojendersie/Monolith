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
	///		  - it must define a constant T::UNDEFINED
	///		  - it must have a Touch() method to mark potentially outdated
	///			nodes. A node is outdated if one of its children changed
	///			(recursively).
	///		* Listener Must have an Update() method which is called if voxels are changed
	template<typename T, typename Listener>
	class SparseVoxelOctree
	{
	public:
		struct SVON;

		/// \brief Create an empty octree.
		/// \param [in] _defaultData Initial value used when something must be
		///		set back.
		SparseVoxelOctree( Listener* _listener );

		/// \brief Set a voxel in the octree and call update of the listener.
		/// \details This method overwrites all covered voxels.
		/// \param [in] _position Position inside the given level.
		/// \param [in] _level Depth in the grid hierarchy. 0 is the maximum
		///		resolution of the voxel grid. The size is the logarithmic edge
		///		length of voxels (2^0, 2^1, ...).
		/// \param [in] _type Voxel to be set. A voxel equal to _defaultData can
		///		delete a voxel.
		void Set( const Math::IVec3& _position, int _level, T _type );

		/// \brief Getter which returns a node reference.
		///	\param [in] _position Target position inside the _level.
		///	\param [in] _level Depth in the grid hierarchy. 0 is the maximum
		///		resolution of the voxel grid. The level is the logarithmic edge
		///		length of voxels (2^0, 2^1, ...).
		/// \return nullptr if the node is not in the tree otherwise the node.
		const SVON* Get( const Math::IVec3& _position, int _level ) const;
		SVON* Get( const Math::IVec3& _position, int _level );

		/// \brief Traverse through the whole tree.
		/// \param [in] _processor An arbitrary implementation of the
		///		SVOProcessor concept. The processor object can hold any amount
		///		of local data as additional parameters. For each node the two
		///		functions are called.
		///
		///		Using an empty inline implementation will remove the calls
		///		through optimization.
		template<class Processor>
		void Traverse( Processor& _processor );

		/// \brief Traverse through the whole tree and compute additional
		///		neighborhood access.
		/// \param [in] _processor An arbitrary implementation of the
		///		SVONeighborProcessor concept. The processor object can hold any
		///		amount of local data as additional parameters. For each node
		///		the two functions are called.
		///
		///		Using an empty inline implementation will remove the calls
		///		through optimization.
		template<class Processor>
		void TraverseEx( Processor& _processor );

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
		bool RayCast( const Math::Ray& _ray, int _targetLevel, HitResult& _hit ) const;



		/// \brief A Sparse-Voxel-Octree-Node
#		pragma pack(push,1)
		struct SVON {

			/// \brief Recursive traverse.
			/// \see SparseVoxelOctree::Traverse
			/// \param [in] _position Voxel position in 3D grid + level in the
			///		tree where 0 is the highest resolution.
			/// \param [in] _processor An arbitrary implementation of the
			///		SVOProcessor concept.
			template<class Processor>
			void Traverse( const Math::IVec4& _position, Processor& _processor );

			/// \brief Recursive traverse with neighborhood information.
			/// \see SparseVoxelOctree::TraverseEx
			/// \param [in] _position Voxel position in 3D grid + level in the
			///		tree where 0 is the highest resolution.
			/// \param [in] _processor An arbitrary implementation of the
			///		SVONeighborProcessor concept.
			template<class Processor>
			void TraverseEx( const Math::IVec4& _position, Processor& _processor,
				const SVON* _left, const SVON* _right, const SVON* _bottom,
				const SVON* _top, const SVON* _front, const SVON* _back );

			/// \copydoc SparseVoxelOctree::RayCast
			bool RayCast( const Math::Ray& _ray, Math::IVec3& _position, int _level,
				int _targetLevel, HitResult& _hit ) const;

			T& Data()						{ return m_data; }
			const T& Data() const			{ return m_data; }
			const SVON* Children() const	{ return m_children; }
			/// \brief Robust getter for child index access.
			/// \details This method can be called on nullptrs and checks the
			///		index in debug mode. Further it returns nullptr if the node
			///		has no children or the specific child is undefined.
			SVON* GetChild( int _index )	{ assert(_index >= 0 && _index < 8); if(!this) return nullptr; if( m_children && (m_children[_index].m_children || m_children[_index].m_data != T::UNDEFINED)) return m_children + _index; return nullptr; }
			const SVON* GetChild( int _index ) const	{ assert(_index >= 0 && _index < 8); if(!this) return nullptr; if( m_children && (m_children[_index].m_children || m_children[_index].m_data != T::UNDEFINED)) return m_children + _index; return nullptr; }
		private:
			T m_data;
			SVON* m_children;

			SVON() : m_children(nullptr), m_data(T::UNDEFINED)	{}

			/// \brief Set a voxel on a chosen resolution.
			/// \param [in] _currentSize The voxel size in the current recursion
			///		level. This can be used to compute the full actual position.
			///	\param [in] _position Position where to set the new voxel.
			///		_position[3] is the logarithmic size of the voxel to be set. 0 is
			///		the layer with the highest resolution.
			///	\param [in] _type The new type.
			/// \param [in] _parent The model for all update steps if voxels are
			///		overwritten.
			static void Set(SVON* _this, int _currentSize, const Math::IVec4& _position,
				T _type, SparseVoxelOctree* _parent);

			/// \brief Set all children to _defaultData to do an correct update and than
			///		delete the memory block.
			/// \param [in] _parent The model for all update steps if voxels are
			///		overwritten.
			///	\param [in] _removePhysicaly Call the model update for each deleted voxel or not? 
			void RemoveSubTree(const Math::IVec4& _position, SparseVoxelOctree* _parent, bool _removePhysically);

			/// \brief Computes the child array index [0,7] assuming that the
			///		target position is within the current voxels children
			static int ComputeChildIndex(const Math::IVec4& _targetPosition, int _childSize);

			friend class SparseVoxelOctree;
		};
#		pragma pack(pop)



		/// \brief Concept of a kernel for tree traversals.
		/// \details The methods are non-virtual. Since the tree works with
		///		templates it is not required and would only slow down things.
		struct SVOProcessor
		{
			/// \brief Called before tree recursion.
			/// \param [in] _position Octree position and level.
			/// \param [inout] _node Read and write access to the current
			///		node's data. Inclusive read access to its children.
			/// \return false if traversal should stop here and do not go deeper.
			bool PreTraversal(const Math::IVec4& _position, SVON* _node)	{}

			/// \brief Called after returning from recursion.
			/// \details If there are no children or recursion is canceled due
			///		to PreTraversal's return value PostTraversal is also called.
			void PostTraversal(const Math::IVec4& _position, SVON* _node)	{}
		protected:
			/// \brief This is a concept - do not use instances of this type
			SVOProcessor()						{}
			SVOProcessor(const SVOProcessor&)	{}
			SVOProcessor(SVOProcessor&&)		{}
		};

		/// \brief Concept of a kernel for tree traversals with neighborhood.
		/// \details \see SVOProcessor
		struct SVONeighborProcessor
		{
			/// \copydoc SVOProcessor::PreTraversal
			/// \param [in] _left Read access to the neighbor in negative x
			///		direction or nullptr (no neighbor).
			bool PreTraversal(const Math::IVec4& _position, SVON* _node,
				const SVON* _left, const SVON* _right, const SVON* _bottom,
				const SVON* _top, const SVON* _front, const SVON* _back)	{}

			/// \copydoc SVOProcessor::PostTraversal
			void PostTraversal(const Math::IVec4& _position, SVON* _node,
				const SVON* _left, const SVON* _right, const SVON* _bottom,
				const SVON* _top, const SVON* _front, const SVON* _back)	{}
		protected:
			/// \brief This is a concept - do not use instances of this type
			SVONeighborProcessor()								{}
			SVONeighborProcessor(const SVONeighborProcessor&)	{}
			SVONeighborProcessor(SVONeighborProcessor&&)		{}
		};


	protected:
		Listener* m_listener;

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

		/// \brief Touch all nodes on a path
		///	\param [in] _position Target position inside the _level.
		///	\param [in] _level Depth in the grid hierarchy. 0 is the maximum
		///		resolution of the voxel grid. The level is the logarithmic edge
		///		length of voxels (2^0, 2^1, ...).
		void SetDirty( const Math::IVec3& _position, int _level );

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
	SparseVoxelOctree<T,Listener>::SparseVoxelOctree( Listener* _listener ) :
		m_listener(_listener),
		m_SVONAllocator(sizeof(SVON)*8),
		m_root(),
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
			new (&pNew[i]) SVON();
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
			pNew[rootIndex].m_children = m_root.m_children;
			pNew[rootIndex].m_data = m_root.m_data;
			// Set root to higher level
			m_rootPosition >>= 1;
			m_root.m_children = pNew;
			m_root.m_data = T::UNDEFINED;
			++m_rootSize;
			// Still to small?
			++scale;
			position >>= 1;
		}

		// One of the eight children must contain the target position.
		SVON::Set(&m_root, m_rootSize, IVec4(_position, _level), _type, this);

		// Make all 6 neighbors dirty - their neighborhood changed
		SetDirty( IVec3(_position[0]+1, _position[1], _position[2]), _level );
		SetDirty( IVec3(_position[0]-1, _position[1], _position[2]), _level );
		SetDirty( IVec3(_position[0], _position[1]+1, _position[2]), _level );
		SetDirty( IVec3(_position[0], _position[1]-1, _position[2]), _level );
		SetDirty( IVec3(_position[0], _position[1], _position[2]+1), _level );
		SetDirty( IVec3(_position[0], _position[1], _position[2]-1), _level );
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	void SparseVoxelOctree<T,Listener>::SetDirty( const Math::IVec3& _position, int _level )
	{
		// Get on an empty model? Would be better if this never happens ->
		// better performance because no 'if' on m_rootSize required.
		assert(m_rootSize != -1);

		// Special cases: not inside octree
		int scale = m_rootSize-_level;
		if( scale < 0 ) return;
		Math::IVec3 position = _position >> scale;
		if((position-m_rootPosition) != Math::IVec3(0))
			return;

		// Search in the octree (while not on target level or tree ends)
		SVON* current = &m_root;
		while( (scale >= 1) && current->m_children ) {
			current->Data().Touch();
			--scale;
			int x = (_position[0] >> scale) & 1;
			int y = (_position[1] >> scale) & 1;
			int z = (_position[2] >> scale) & 1;
			current = &current->m_children[ x + y * 2 + z * 4 ];
		}
		current->Data().Touch();
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	typename const SparseVoxelOctree<T,Listener>::SVON* SparseVoxelOctree<T,Listener>::Get( const Math::IVec3& _position, int _level ) const
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
		while( (scale > 0) && current->m_children ) {
			--scale;
			int x = (_position[0] >> scale) & 1;
			int y = (_position[1] >> scale) & 1;
			int z = (_position[2] >> scale) & 1;
			current = &current->m_children[ x + y * 2 + z * 4 ];
		}

		if( current->Data() == T::UNDEFINED && !current->Children() ) return nullptr;
		return current;
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	typename SparseVoxelOctree<T,Listener>::SVON* SparseVoxelOctree<T,Listener>::Get( const Math::IVec3& _position, int _level )
	{
		// Use the constant getter and allow write access to the element afterwards
		return const_cast<SparseVoxelOctree<T,Listener>::SVON*>(const_cast<const SparseVoxelOctree<T,Listener>*>(this)->Get(_position, _level));
	}

	// ********************************************************************* //
	template<typename T, typename Listener> template<typename Processor>
	void SparseVoxelOctree<T,Listener>::Traverse( Processor& _processor )
	{
		assert(m_rootSize != -1);

		m_root.Traverse(IVec4(m_rootPosition, m_rootSize), _processor);
	}

	// ********************************************************************* //
	template<typename T, typename Listener> template<typename Processor>
	void SparseVoxelOctree<T,Listener>::TraverseEx( Processor& _processor )
	{
		assert(m_rootSize != -1);

		m_root.TraverseEx(IVec4(m_rootPosition, m_rootSize), _processor,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
	}

	// ********************************************************************* //
	template<typename T, typename Listener>
	bool SparseVoxelOctree<T,Listener>::RayCast( const Math::Ray& _ray, int _targetLevel, HitResult& _hit ) const
	{
		assert(m_rootSize != -1);

		// if root to small scale it up and test against a single box
		if( m_rootSize < _targetLevel )
		{
			IVec3 position = m_rootPosition << (_targetLevel - m_rootSize);
			if( Math::Intersect::RayAACube( _ray, position, (1<<_targetLevel), _hit.side ) )
			{
				_hit.position = position;
				_hit.voxel = T::UNDEFINED;
				return true;
			} else
				return false;
		} else 
			// Use recursive algorithm.
			return m_root.RayCast( _ray, Math::IVec3(m_rootPosition), m_rootSize, _targetLevel, _hit );
	}

	// ********************************************************************* //










	// ********************************************************************* //
	template<typename T, typename Listener>
	void SparseVoxelOctree<T,Listener>::SVON::Set(SVON* _this, int _currentSize,
		const Math::IVec4& _position,
		T _data, SparseVoxelOctree* _parent)
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
			currentElement->m_data.Touch();
			// Do not set back to undefined with touch - undefined is used to
			// mark non existent tree areas.
			//assert(currentElement->m_data != T::UNDEFINED);// TODO: Use this property for optimizations
			int childIndex = ComputeChildIndex(_position, _currentSize-1);
			if( !currentElement->m_children )
			{ // Create new children
				currentElement->m_children = _parent->NewSVON();
				// Set all children to the same type as this node.
				// It was uniform before!
				for(int i=0; i<8; ++i) currentElement->m_children[i].m_data = currentElement->m_data;
			}
			--_currentSize;
			callStack.PushBack(currentElement);
			currentElement = &currentElement->m_children[childIndex];
		}
		assert( _currentSize == _position[3] );

		// This is the target voxel. Delete everything below.
		if( currentElement->m_children ) currentElement->RemoveSubTree(_position, _parent, true);
		// Physical update of this voxel
		_parent->m_listener->Update(_position, currentElement->m_data, _data);
		currentElement->m_data = _data;

		// Unroll stack as long as the voxel type in hierarchy changes.
		bool deletedVoxel = _data == T::UNDEFINED;
		while( deletedVoxel && callStack.Size()>0 )
		{
			currentElement = callStack.Last();
			callStack.PopBack();
			// If all 8 children are undefined and have no children delete that nodes.
			for( int i = 0; i < 8; ++i )
			{
				if( currentElement->m_children[i].m_data != T::UNDEFINED 
					|| currentElement->m_children[i].m_children )
				{
					deletedVoxel = false;
					break;	// Stop testing the other children
				}
			}

			if( deletedVoxel )
			{
				_parent->m_SVONAllocator.Free(currentElement->m_children);
				currentElement->m_children = nullptr;
				currentElement->m_data = _data;
			}
		}
	}


	// ********************************************************************* //
	template<typename T, typename Listener>
	void SparseVoxelOctree<T,Listener>::SVON::RemoveSubTree(const Math::IVec4& _position, SparseVoxelOctree* _parent, bool _removePhysically)
	{
		assert( _position[3] >= 0 );
		assert( m_children );
		for(int i=0; i<8; ++i)
		{
			// Recursive
			if( m_children[i].m_children )
				m_children[i].RemoveSubTree(_position+CHILD_OFFSETS[i], _parent, _removePhysically);
			else if(_removePhysically)
				_parent->m_listener->Update(_position+CHILD_OFFSETS[i], m_data, T::UNDEFINED);
		}
		// Delete
		_parent->m_SVONAllocator.Free(m_children);
		m_children = nullptr;
	}

	// ********************************************************************* //
	template<typename T, typename Listener> template<class Processor>
	void SparseVoxelOctree<T,Listener>::SVON::Traverse( const Math::IVec4& _position, Processor& _processor )
	{
		if( _processor.PreTraversal(_position, this) && m_children )
		{
			Math::IVec4 position(_position[0]<<1, _position[1]<<1, _position[2]<<1, _position[3]);
			for( int i=0; i<8; ++i )
			{
				// Is the voxel outside the tree/really empty?
				if( m_children[i].m_data != T::UNDEFINED || m_children[i].m_children )
					m_children[i].Traverse(position + CHILD_OFFSETS[i], _processor);
			}
		}

		_processor.PostTraversal(_position, this);
	}

	// ********************************************************************* //
	template<typename T, typename Listener> template<class Processor>
	void SparseVoxelOctree<T,Listener>::SVON::TraverseEx( const Math::IVec4& _position, Processor& _processor,
		const SVON* _left, const SVON* _right, const SVON* _bottom,
		const SVON* _top, const SVON* _front, const SVON* _back )
	{
		// Preorder traversal - delivers stop criterion
		if( _processor.PreTraversal(_position, this, _left, _right, _bottom, _top, _front, _back) && m_children )
		{
			// Prepare traversal and internal neighborhood
			SVON* children[8];
			for( int i=0; i<8; ++i )
			{
				// Is the voxel outside the tree/really empty?
				if( m_children[i].m_data != T::UNDEFINED || m_children[i].m_children )
					children[i] = m_children + i;
				else children[i] = nullptr;
			}
			Math::IVec4 p(_position[0]<<1, _position[1]<<1, _position[2]<<1, _position[3]-1);
			// Manually call traversal for each child due to complex neighborhood construction
			if( children[0] ) children[0]->TraverseEx( p                                  , _processor, _left->GetChild( 1 ),         children[ 1 ], _bottom->GetChild( 2 ),       children[ 2 ], _front->GetChild( 4 ),        children[ 4 ] );
			if( children[1] ) children[1]->TraverseEx( IVec4(p[0]+1, p[1]  , p[2]  , p[3]), _processor,        children[ 0 ], _right->GetChild( 0 ), _bottom->GetChild( 3 ),       children[ 3 ], _front->GetChild( 5 ),        children[ 5 ] );
			if( children[2] ) children[2]->TraverseEx( IVec4(p[0]  , p[1]+1, p[2]  , p[3]), _processor, _left->GetChild( 3 ),         children[ 3 ],          children[ 0 ], _top->GetChild( 0 ), _front->GetChild( 6 ),        children[ 6 ] );
			if( children[3] ) children[3]->TraverseEx( IVec4(p[0]+1, p[1]+1, p[2]  , p[3]), _processor,        children[ 2 ], _right->GetChild( 2 ),          children[ 1 ], _top->GetChild( 1 ), _front->GetChild( 7 ),        children[ 7 ] );
			if( children[4] ) children[4]->TraverseEx( IVec4(p[0]  , p[1]  , p[2]+1, p[3]), _processor, _left->GetChild( 5 ),         children[ 5 ], _bottom->GetChild( 6 ),       children[ 6 ],         children[ 0 ], _back->GetChild( 0 ) );
			if( children[5] ) children[5]->TraverseEx( IVec4(p[0]+1, p[1]  , p[2]+1, p[3]), _processor,        children[ 4 ], _right->GetChild( 4 ), _bottom->GetChild( 7 ),       children[ 7 ],         children[ 1 ], _back->GetChild( 1 ) );
			if( children[6] ) children[6]->TraverseEx( IVec4(p[0]  , p[1]+1, p[2]+1, p[3]), _processor, _left->GetChild( 7 ),         children[ 7 ],          children[ 4 ], _top->GetChild( 4 ),         children[ 2 ], _back->GetChild( 2 ) );
			if( children[7] ) children[7]->TraverseEx( IVec4(p[0]+1, p[1]+1, p[2]+1, p[3]), _processor,        children[ 6 ], _right->GetChild( 6 ),          children[ 5 ], _top->GetChild( 5 ),         children[ 3 ], _back->GetChild( 3 ) );
		}

		_processor.PostTraversal(_position, this, _left, _right, _bottom, _top, _front, _back);
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
		int _targetLevel, HitResult& _hit ) const
	{
		// Test the current cube
		if( m_children && !(_targetLevel==_level))
		{
			// Recursion required. We can user fast test without side detection.
			float t;
			int edgeLength = 1<<_level;
			if( Math::Intersect::RayAACube( _ray, _position * edgeLength, edgeLength, t ) )
			{
				// Ray passes the current node. Make a sphere test for each
				// child.
				TmpCollisionEntry list[8];
				int num = 0;		// How many children have a collision?
				_position <<= 1;	// First child index
				edgeLength /= 2;
				float r = edgeLength * 0.866025404f;	// * 0.5 * sqrt(3)
				Math::Vec3 centerOffset(edgeLength * 0.5f, edgeLength * 0.5f, edgeLength * 0.5f);
				for( int i=0; i<8; ++i )
				{
					// Is the child non-empty?
					if( m_children[i].m_children || m_children[i].m_data != T::UNDEFINED )
					{
						// Create spheres centered at the box centers
						if( Math::Intersect::RaySphere( _ray,
							Math::Sphere( (_position + Math::IVec3(CHILD_OFFSETS[i]))*edgeLength + centerOffset, r ),
							list[num].value ) )
							list[num++].index = i;
					}
				}
				 
				// Sort the possible colliding children after range.
				Algo::SmallSort( list, num );

				// Test them sequentially and stop immediately if something was hit.
				for( int i=0; i<num; ++i )
					if( m_children[list[i].index].RayCast( _ray, _position + Math::IVec3(CHILD_OFFSETS[list[i].index]), _level-1, _targetLevel, _hit ) )
						return true;

				// No child collides
				return false;
			}
		} else {
			// This is a leaf - if we hit we wanna know the side.
			int edgeLength = 1<<_level;
			if( Math::Intersect::RayAACube( _ray, _position * edgeLength, edgeLength, _hit.side ) )
			{
				_hit.position = _position;
				_hit.voxel = m_data;
				return true;
			} else
				return false;
		}

		return false;
	}

} // namespace Voxel