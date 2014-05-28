#pragma once

#include <cassert>

namespace Jo {

	/**************************************************************************//**
	* \brief	A dynamic array which uses stack space as long as it isn't too large.
	* \details	This array does not need to reallocate space up to a size of n
	*			elements. That increases performance if working with small or
	*			local arrays. But copying is expensive and move construction must
	*			copy the stack space too.
	*
	*			Try to avoid using them as return value and inject a reference
	*			to a function which should return array stuff.
	*			
	*			Creating an HybridArray with ´new´ means that the array is located
	*			on heap fully and nothing lies on the stack. In this case 'stack'
	*			must be replaced by 'local object memory'. This is still faster
	*			because of cache usage and less reallocations for small arrays.
	*****************************************************************************/
	template<typename T, unsigned n = 8>
	class HybridArray
	{
	public:
		typedef T ElemType;

		/// \brief Standard: create an array with capacity n.
		HybridArray();

		/// \brief Create a dynamic array with preallocated space. Probably
		///		this disables the advantage of 'hybrid' but allows to replace
		///		std::vector in more cases.
		HybridArray(uint32_t _capacity);

		/// \brief Copy construction (deep).
		HybridArray(const HybridArray<T,n>& _other);

		/// \brief Move construction (deep only for arrays smaller n).
		HybridArray(HybridArray<T,n>&& _other);

		~HybridArray();

		/// \brief Deep copying assignment
		HybridArray<ElemType,n>& operator = (const HybridArray<T,n>& _other);

		/// \brief Write-array access.
		T& operator [] (uint32_t _index);
		/// \brief Read-array access.
		const T& operator [] (uint32_t _index) const;

		/// \brief Enlarge or prune the memory.
		/// \param [in] _capacity New capacity/size. If _capacity is below n
		///		the size will be influenced but the new capacity is n.
		///		
		///		A value of 0 causes a prune: capacity = Size();
		///		
		///		The size changes if _capacity is below the current size (prune)
		void Resize(uint32_t _capacity = 0);

		/// \brief Insert an element copy at the end of the array.
		/// \details This might cause a resize with costs O(n).
		const ElemType& PushBack(const ElemType& _element);

		/// \brief Insert an element at the end of the array.
		/// \details This might cause a resize with costs O(n).
		ElemType& PushBack(ElemType&& _element);

		/// \brief Delete the last element
		void PopBack();

		/// \brief Insert an element copy at the given index of the array.
		/// \details This might cause a resize with costs O(n).
		const ElemType& Insert(uint32_t _where, const ElemType& _element);

		/// \brief Insert an element at the given index of the array.
		/// \details This might cause a resize with costs O(n).
		ElemType& Insert(uint32_t _where, ElemType&& _element);

		/// \brief Delete an element in O(1). This operation changes the
		///		element order by replacing the first with the last element.
		void Delete(unsigned _index);

		/// \brief Delete an element in O(n). This operation keeps the
		///		element order.
		void ArrayDelete(unsigned _index);

		uint32_t Size() const		{ return m_size; }
		uint32_t Capacity() const	{ return m_capacity; }

		/// \brief Access first element
		ElemType& First()				{ assert(m_size>0); return *m_data; }
		const ElemType& First() const	{ assert(m_size>0); return *m_data; }

		/// \brief Access last element
		ElemType& Last()				{ assert(m_size>0); return m_data[m_size-1]; }
		const ElemType& Last() const	{ assert(m_size>0); return m_data[m_size-1]; }
	protected:
		uint32_t m_capacity;	///< Maximum number of elements
		uint32_t m_size;		///< Current number of elements
		ElemType* m_data;		///< Pointer to array memory block. Might be on stack or heap.

		T m_localStorage[n];	///< The local storage on stack or in object heap space.
	};






	// ********************************************************************* //
	//  HybridArray Implementation											 //
	// ********************************************************************* //
	template<typename T, unsigned n>
	HybridArray<T,n>::HybridArray() :
		m_capacity(n),
		m_size(0),
		m_data(m_localStorage)
	{
	}


	// ********************************************************************* //
	template<typename T, unsigned n>
	HybridArray<T,n>::HybridArray(uint32_t _capacity) :
		m_capacity(_capacity < n ? n : _capacity),
		m_size(0)
	{
		if( m_capacity > n )
			// Too large for local space
			m_data = (ElemType*)malloc(sizeof(ElemType) * n);
		else {
			m_data = m_localStorage;
		}
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	HybridArray<T,n>::HybridArray(const HybridArray<T,n>& _other) :
		m_capacity(_other.m_size),	// prune memory
		m_size(_other.m_size)
		// TODO: wanna have delegating constructor HybridArray(_capacity)
	{
		// Local or global copy?
		if( m_capacity <= n )
			m_data = m_localStorage;
		else
			m_data = (ElemType*)malloc(sizeof(T) * m_capacity);
		// Deep copy now
		for( unsigned i=0; i<m_size; ++i )
			new (m_data+i) T( _other.m_data[i] );
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	HybridArray<T,n>::HybridArray(HybridArray<T,n>&& _other) :
		m_capacity(_other.m_capacity),
		m_size(_other.m_size)
	{
		if( m_capacity <= n ) {
			// Local memory must be copied.
			m_data = m_localStorage;
			// Use flat copy if possible
			for( unsigned i=0; i<m_size; ++i )
				new (m_data+i) T( std::move(_other.m_data[i]) );
		} else {
			// Move memory
			m_data = _other.m_data;
			// This trick increases delete performance. The branch on
			// capacity is made before the one on nullptr.
			_other.m_capacity = 0;
		}
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	HybridArray<T,n>::~HybridArray()
	{
		for( unsigned i=0; i<m_size; ++i )
			m_data[i].~T();
		if( m_capacity > n ) free(m_data);
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	HybridArray<T,n>& HybridArray<T,n>::operator = (const HybridArray<T,n>& _other)
	{
		// Clear old data
		this->~HybridArray();

		// Deep copy
		new (this) HybridArray<T,n>(_other);

		return *this;
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	typename HybridArray<T,n>::ElemType& HybridArray<T,n>::operator [] (uint32_t _index)
	{
		// TODO: logging system
		assert(m_size > _index);

		return m_data[_index];
	}

	template<typename T, unsigned n>
	const typename HybridArray<T,n>::ElemType& HybridArray<T,n>::operator [] (uint32_t _index) const
	{
		// TODO: logging system
		assert(m_size > _index);

		return m_data[_index];
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	void HybridArray<T,n>::Resize(uint32_t _capacity)
	{
		// Resizing without change. Bad Performance!
		// TODO: logging system
		assert(m_capacity != _capacity);

		// Prune as much as possible
		if( _capacity == 0 ) _capacity = m_size;

		uint32_t oldCapacity = m_capacity;
		m_capacity = _capacity < n ? n : _capacity;
		// If both old and new capacity are <= n nothing happens otherwise
		// a realloc or copy is necessary
		if( m_capacity > n || oldCapacity > n )
		{
			ElemType* oldData = m_data;

			// Determine target memory
			if( m_capacity <= n ) m_data = m_localStorage;
			else m_data = (ElemType*)malloc( m_capacity * sizeof(ElemType) );

			// Now keep the old data
			// Use flat copy if possible
			for( unsigned i=0; i<m_size; ++i )
				new (m_data + i) ElemType( std::move(oldData[i]) );

			// Correctly delete pruned elements
			if( m_size > _capacity )
			{
				for( unsigned i=_capacity; i<m_size; ++i )
					m_data[i].~ElemType();
				m_size = _capacity;
			}
			if( oldCapacity > n ) free(oldData);
		}
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	const T& HybridArray<T,n>::PushBack(const ElemType& _element)
	{
		// Exponential growth if necessary
		if( m_size == m_capacity ) Resize(m_capacity * 2);
		// Clone
		return *(new (m_data + m_size++) ElemType( _element ));
	}

	template<typename T, unsigned n>
	T& HybridArray<T,n>::PushBack(ElemType&& _element)
	{
		// Exponential growth if necessary
		if( m_size == m_capacity ) Resize(m_capacity * 2);
		// Take over
		return *(new (m_data + m_size++) ElemType( _element ));
	}

	template<typename T, unsigned n>
	void HybridArray<T,n>::PopBack()
	{
		// TODO: logging system
		assert(m_size > 0);

		// Delete old
		m_data[--m_size].~ElemType();
	}

	// ********************************************************************* //
	template<typename T, unsigned n>
	const T& HybridArray<T,n>::Insert(uint32_t _where, const ElemType& _element)
	{
		// Exponential growth if necessary
		if( m_size == m_capacity ) Resize(m_capacity * 2);
		// Move everything after the index
		for( unsigned i=m_size; i>_where; --i )
			new (m_data+i) ElemType(std::move(m_data[i-1]));
		m_size++;
		// Clone
		return *(new (m_data + _where) ElemType( _element ));
	}

	template<typename T, unsigned n>
	T& HybridArray<T,n>::Insert(uint32_t _where, ElemType&& _element)
	{
		// Exponential growth if necessary
		if( m_size == m_capacity ) Resize(m_capacity * 2);
		// Move everything after the index
		for( unsigned i=m_size; i>_where; --i )
			new (m_data+i) ElemType(std::move(m_data[i-1]));
		m_size++;
		// Take over
		return *(new (m_data + _where) ElemType( _element ));
	}


	// ********************************************************************* //
	template<typename T, unsigned n>
	void HybridArray<T,n>::Delete(unsigned _index)
	{
		// TODO: logging system
		assert(m_size > _index);

		// Delete old
		m_data[_index].~ElemType();
		--m_size;
		// Replace with last
		if(m_size != _index)
			new (m_data+_index) ElemType(std::move(m_data[m_size]));
	}

	template<typename T, unsigned n>
	void HybridArray<T,n>::ArrayDelete(unsigned _index)
	{
		// TODO: logging system
		assert(m_size > _index);

		// Delete old
		m_data[_index].~ElemType();
		// Replace all successors
		--m_size;
		for( unsigned i=_index; i<m_size; ++i )
			new (m_data+i) ElemType(std::move(m_data[i+1]));
	}

} // namespace Jo