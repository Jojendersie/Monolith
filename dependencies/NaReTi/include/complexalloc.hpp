#pragma once

#include "stackalloc.hpp"

namespace utils{
	//allocator that calls the destructors of objects created with construct
	template < typename _Alloc >
	class DetorAllocator : public _Alloc
	{
	public:
		class Destructible
		{
		public:
			virtual ~Destructible(){};
		};


		~DetorAllocator()
		{
			for (auto el : m_elements) el->~Destructible();
		}

		template< typename _T, typename... _Args, typename = std::enable_if< std::is_base_of<Destructible, _T>::value >::type >//objects need an virtual destructor
		_T* construct(_Args&&... _args)
		{
			_T* ptr = _Alloc::construct<_T>(std::forward< _Args >(_args)...);
			m_elements.push_back(ptr);
			return ptr;
		}

		//does not call a destructor
		//use this only for const size structures that own no dynamic buffers
		template< typename _T, typename... _Args>
		_T* constructUnsafe(_Args&&... _args)
		{
			return _Alloc::construct<_T>(std::forward< _Args >(_args)...);;
		}


	private:
		std::vector < Destructible* > m_elements;
	};

	typedef DetorAllocator< StackAlloc > DetorAlloc;
}