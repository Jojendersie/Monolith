#pragma once

#include <cassert>

namespace Algo {

	/// \brief conditional swap without branches.
	/// TODO: benchmark for different sizes.
	template<typename T>
	inline void CmpSwap(T* _data, int _x, int _y)
	{
		// This is doing a real copy - which is good for primitive types but
		// there could be a better method (advantage of this solution:
		// no branches! - which is not possible with std::swap)
	/*	const T t = _data[_x] < _data[_y] ? _data[_x] : _data[_y];
		_data[_y] = _data[_x] < _data[_y] ? _data[_y] : _data[_x];
		_data[_x] = std::move(t);*/

		// Tested: this one is three times faster.
		if(_data[_y] < _data[_x])
			std::swap(_data[_x], _data[_y]);
	}

	/// \brief Function to sort two elements as fast as possible
	template<typename T>
	inline void Sort2(T* _data)
	{
		CmpSwap(_data, 0, 1);
	}

	/// \brief Function to sort three elements as fast as possible
	template<typename T>
	inline void Sort3(T* _data)
	{
		CmpSwap(_data, 1, 2);
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 1, 2);
	}

	/// \brief Function to sort four elements as fast as possible
	template<typename T>
	inline void Sort4(T* _data)
	{
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 1, 3);
		CmpSwap(_data, 0, 2);
		CmpSwap(_data, 1, 2);
	}

	/// \brief Function to sort five elements as fast as possible
	template<typename T>
	inline void Sort5(T* _data)
	{
		CmpSwap(_data, 1, 2);
		CmpSwap(_data, 3, 4);
		CmpSwap(_data, 1, 3);
		CmpSwap(_data, 0, 2);
		CmpSwap(_data, 2, 4);
		CmpSwap(_data, 0, 3);
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 1, 2);
	}

	/// \brief Function to sort six elements as fast as possible
	template<typename T>
	inline void Sort6(T* _data)
	{
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 4, 5);
		CmpSwap(_data, 0, 2);
		CmpSwap(_data, 3, 5);
		CmpSwap(_data, 1, 4);
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 4, 5);
		CmpSwap(_data, 1, 2);
		CmpSwap(_data, 3, 4);
		CmpSwap(_data, 2, 3);
		/* Form http://stackoverflow.com/questions/2786899/fastest-sort-of-fixed-length-6-int-array
		 * TODO: test if really faster
		SWAP(1, 2);
		SWAP(4, 5);
		SWAP(0, 2);
		SWAP(3, 5);
		SWAP(0, 1);
		SWAP(3, 4);
		SWAP(1, 4);
		SWAP(0, 3);
		SWAP(2, 5);
		SWAP(1, 3);
		SWAP(2, 4);
		SWAP(2, 3);*/
	}

	/// \brief Function to sort seven elements as fast as possible
	template<typename T>
	inline void Sort7(T* _data)
	{
		CmpSwap(_data, 1, 2);
		CmpSwap(_data, 3, 4);
		CmpSwap(_data, 5, 6);
		CmpSwap(_data, 0, 2);
		CmpSwap(_data, 4, 6);
		CmpSwap(_data, 3, 4);
		CmpSwap(_data, 2, 6);
		CmpSwap(_data, 1, 5);
		CmpSwap(_data, 0, 4);
		CmpSwap(_data, 2, 5);
		CmpSwap(_data, 0, 3);
		CmpSwap(_data, 2, 4);
		CmpSwap(_data, 1, 3);
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 4, 5);
	}


	/// \brief Function to sort eight elements as fast as possible
	template<typename T>
	inline void Sort8(T* _data)
	{
		CmpSwap(_data, 0, 7);
		CmpSwap(_data, 3, 4);
		CmpSwap(_data, 1, 6);
		CmpSwap(_data, 2, 5);
		CmpSwap(_data, 0, 3);
		CmpSwap(_data, 4, 7);
		CmpSwap(_data, 1, 2);
		CmpSwap(_data, 5, 6);
		CmpSwap(_data, 0, 1);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 4, 5);
		CmpSwap(_data, 6, 7);
		CmpSwap(_data, 3, 5);
		CmpSwap(_data, 2, 4);
		CmpSwap(_data, 1, 2);
		CmpSwap(_data, 3, 4);
		CmpSwap(_data, 5, 6);
		CmpSwap(_data, 2, 3);
		CmpSwap(_data, 4, 5);
	}

	/// \brief An optimal sorting algorithm for up to 8 elements.
	/// \details This function uses sorting networks for 2 to 8 elements which
	///		have the lowest known numbers of compare-exchange operations.
	///		
	///		Source: http://www.angelfire.com/blog/ronz/Articles/999SortingNetworksReferen.html
	template<typename T>
	inline void SmallSort(T* _data, int _numElements)
	{
		switch( _numElements )
		{
		case 0: case 1: break;
		case 2: Sort2(_data); break;
		case 3: Sort3(_data); break;
		case 4: Sort4(_data); break;
		case 5: Sort5(_data); break;
		case 6: Sort6(_data); break;
		case 7: Sort7(_data); break;
		case 8: Sort8(_data); break;
		default:
			// Can only sort up to 8 elements with SmallSort!
			assert(false);
			break;
		}
	}


} // namespace Algo