#pragma once

#include <cstdint>

namespace Math {

	/// \brief Integer type template whose size can be set by a number of bits.
	template<int NBits> struct Int { Int() {static_assert(false, "An integer type must have 8, 16, 32 or 64 bit.");} };
	template<> struct Int<8>	{ typedef int8_t T; };
	template<> struct Int<16>	{ typedef int16_t T; };
	template<> struct Int<32>	{ typedef int32_t T; };
	template<> struct Int<64>	{ typedef int64_t T; };


	/// \brief 64 bit fixed point number
	/// \tparam FracDigits Number of dual digits after the point. This number must
	///		be smaller than or equal 63 (one bit is for the sign)
	template<int FracDigits>
	class Fix
	{
	public:
		static_assert( FracDigits < 64, "You cannot have more digits after the point than there are digits!" );

		Fix( int64_t _fixed ) : m_data(_fixed) {}
		Fix( double _float );
		Fix( float _float );

		Fix& operator += (Fix _rhs);
		Fix& operator -= (Fix _rhs);
		// Use double or float arithmetic for multiplication (faster and preciser)
		//Fix& operator *= (Fix _rhs);
		Fix& operator <<= (int _rhs);
		Fix& operator >>= (int _rhs);

		Fix operator + (Fix _rhs) const;
		Fix operator - (Fix _rhs) const;
		// Use double or float arithmetic for multiplication (faster and preciser)
		//Fix operator * (Fix _rhs) const;
		Fix operator >> (int _rhs) const;
		Fix operator << (int _rhs) const;

		operator double();

	private:
		typename int64_t m_data;
	};

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits>::Fix( double _float )
	{
		m_data = int64_t(_float * double(1 << FracDigits));
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits>& Fix<FracDigits>::operator += (Fix<FracDigits> _rhs)
	{
		m_data += _rhs.m_data;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits>& Fix<FracDigits>::operator -= (Fix<FracDigits> _rhs)
	{
		m_data -= _rhs.m_data;
		return *this;
	}

	// ************************************************************************* //
	/*template<int FracDigits>
	Fix<64,FracDigits>& Fix<64,FracDigits>::operator *= (Fix<64,FracDigits> _rhs)
	{
		// Example
		// 0100.1011 * 0010.1010
		//           .10010110
		//         10.01011   
		//       1001.011     
		//       1100.01001110
		int64_t val = 0;
		int idx = 0;
		while( _rhs.m_data != 0 )
		{
			// Add the final shifted result of a single bit multiplication
			int sh = FracDigits - idx;
			if( sh > 0 )
				val += (_rhs.m_data & 1) * (m_data >> sh);
			else val += (_rhs.m_data & 1) * (m_data << -sh);

			// Go to the next digit
			_rhs >>= 1;
			++idx;
		}

		m_data = val;
		return *this;
	}*/

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits>& Fix<FracDigits>::operator <<= (int _rhs)
	{
		m_data <<= _rhs;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits>& Fix<FracDigits>::operator >>= (int _rhs)
	{
		m_data >>= _rhs;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits> Fix<FracDigits>::operator + (Fix<FracDigits> _rhs) const
	{
		// Use self assignment operator
		return Fix<FracDigits>(*this) += _rhs;
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits> Fix<FracDigits>::operator - (Fix<FracDigits> _rhs) const
	{
		// Use self assignment operator
		return Fix<FracDigits>(*this) -= _rhs;
	}

	// ************************************************************************* //
	/*template<int FracDigits>
	Fix<64,FracDigits> Fix<64,FracDigits>::operator * (Fix<64,FracDigits> _rhs) const
	{
		// Use self assignment operator
		return Fix<64,FracDigits>(*this) *= _rhs;
	}*/

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits> Fix<FracDigits>::operator >> (int _rhs) const
	{
		return Fix<FracDigits>(m_data >> _rhs);
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits> Fix<FracDigits>::operator << (int _rhs) const
	{
		return Fix<FracDigits>(m_data << _rhs);
	}

	// ************************************************************************* //
	template<int FracDigits>
	Fix<FracDigits>::operator double()
	{
		// Might be less precise than possible, but is fast and easy.
		return m_data / double(1 << FracDigits);
	}


	typedef Fix<30> Fixed;

} // namespace Math