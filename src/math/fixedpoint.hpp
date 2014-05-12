#pragma once

#include <cstdint>

namespace Math {

	/// \brief 64 bit fixed point number
	/// \tparam FracDigits Number of dual digits after the point. This number must
	///		be smaller than or equal 63 (one bit is for the sign)
	template<int FracDigits>
	class TFix
	{
	public:
		static_assert( FracDigits < 64, "You cannot have more digits after the point than there are digits!" );

		/// \brief Create uninitialized
		TFix() {}
		/// \brief Copy construction
		explicit TFix( int64_t _fixed ) : m_data(_fixed) {}
		/// \brief Implicit generation too dangerous.
		explicit TFix( double _float );
		/// \brief Implicit generation too dangerous.
		explicit TFix( float _float );

		TFix& operator += (TFix _rhs);
		TFix& operator -= (TFix _rhs);
		// Use double or float arithmetic for multiplication (faster and preciser)
		TFix& operator *= (TFix _rhs);
		TFix& operator <<= (int _rhs);
		TFix& operator >>= (int _rhs);

		TFix operator + (TFix _rhs) const;
		TFix operator - (TFix _rhs) const;
		// Use double or float arithmetic for multiplication (faster and preciser)
		TFix operator * (TFix _rhs) const;
		TFix operator >> (int _rhs) const;
		TFix operator << (int _rhs) const;

		operator double() const;
		//operator float() const;

	private:
		typename int64_t m_data;
	};

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>::TFix( double _float )
	{
		m_data = int64_t(_float * double(1 << FracDigits));
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>::TFix( float _float )
	{
		m_data = int64_t(_float * float(1 << FracDigits));
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>& TFix<FracDigits>::operator += (TFix<FracDigits> _rhs)
	{
		m_data += _rhs.m_data;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>& TFix<FracDigits>::operator -= (TFix<FracDigits> _rhs)
	{
		m_data -= _rhs.m_data;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>& TFix<FracDigits>::operator *= (TFix<FracDigits> _rhs)
	{
		*this = double(*this) * double(_rhs)
		return *this;
	}
	/*	// Example
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
	TFix<FracDigits>& TFix<FracDigits>::operator <<= (int _rhs)
	{
		m_data <<= _rhs;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>& TFix<FracDigits>::operator >>= (int _rhs)
	{
		m_data >>= _rhs;
		return *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits> TFix<FracDigits>::operator + (TFix<FracDigits> _rhs) const
	{
		// Use self assignment operator on copy
		return _rhs += *this;
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits> TFix<FracDigits>::operator - (TFix<FracDigits> _rhs) const
	{
		// Use self assignment operator on copy
		return TFix<FracDigits>(*this) -= _rhs;
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits> TFix<FracDigits>::operator * (TFix<FracDigits> _rhs) const
	{
		return TFix(double(*this) * double(_rhs));
	}
	/*	// Use self assignment operator
		return Fix<64,FracDigits>(*this) *= _rhs;
	}*/

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits> TFix<FracDigits>::operator >> (int _rhs) const
	{
		return FixedPoint<FracDigits>(m_data >> _rhs);
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits> TFix<FracDigits>::operator << (int _rhs) const
	{
		return TFix<FracDigits>(m_data << _rhs);
	}

	// ************************************************************************* //
	template<int FracDigits>
	TFix<FracDigits>::operator double() const
	{
		// Might be less precise than possible, but is fast and easy.
		return m_data / double(1 << FracDigits);
	}

	// ************************************************************************* //
	/*template<int FracDigits>
	FixedPoint<FracDigits>::operator float() const
	{
		// Might be less precise than possible, but is fast and easy.
		return float(m_data / double(1 << FracDigits));
	}*/


	typedef TFix<30> Fix;

} // namespace Math