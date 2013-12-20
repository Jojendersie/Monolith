#pragma once

#include "math.hpp"

namespace Math {
	
	/// \brief class for vectors
	template<int n, class Data>
	class Vector
	{
	public:
		/// \brief Standard constructor creates an uninitialized Vector
		Vector() {}
		/// \brief Create a Vector filled with a constant
		explicit Vector(Data _d)			{for (int i = 0; i < n; i++) m_data[i] = _d; }
		/// \brief Create a Vector from an array
		explicit Vector(Data* _d)		{ for (int i = 0; i < n; i++) m_data[i] = _d[i]; }
		/// \brief Create a Vector from a Vector
		Vector(const Vector<n, Data>& _v)	{ for (int i = 0; i < n; i++) m_data[i] = _v[i]; }
		/// \brief Casting to a Data array
		operator Data* ()				{ return m_data; }
		/// \brief Casting to a const Data array
		operator const Data* () const	{ return m_data; }

		/// \brief Write access to an element
		Data& operator [](int _pos)			{ return m_data[_pos]; }
		/// \brief Read access to elements
		Data operator [](int _pos) const	{ return m_data[_pos]; }

		// assignment operators
		Vector<n, Data>& operator = (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] = _v.m_data[i]; return *this; }
		Vector<n, Data>& operator += (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] += _v.m_data[i]; return *this; }
		Vector<n, Data>& operator -= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] -= _v.m_data[i]; return *this; }
		Vector<n, Data>& operator *= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] *= _v.m_data[i]; return *this; }
		Vector<n, Data>& operator /= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] /= _v.m_data[i]; return *this; }
		Vector<n, Data>& operator = (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] = _d; return *this; }
		Vector<n, Data>& operator += (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] += _d; return *this; }
		Vector<n, Data>& operator -= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] -= _d; return *this; }
		Vector<n, Data>& operator *= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] /= _d; return *this; }
		Vector<n, Data>& operator /= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] *= _d; return *this; }

		//Arithmetic operators
		Vector<n, Data> operator+ (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) += _v; }
		Vector<n, Data> operator- (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) -= _v; }
		Vector<n, Data> operator* (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) *= _v; }
		Vector<n, Data> operator/ (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) /= _v; }
		Vector<n, Data> operator+ (const Data _d) const					{ return Vector<n, Data>(this) += _d; }
		Vector<n, Data> operator- (const Data _d) const					{ return Vector<n, Data>(this) -= _d; }
		Vector<n, Data> operator* (const Data _d) const					{ return Vector<n, Data>(this) *= _d; }
		Vector<n, Data> operator/ (const Data _d) const					{ return Vector<n, Data>(this) /= _d; }

		/// \brief Shifting operator for integral types.
		/// \details The Data is kept template to support anything which defines
		///		shifting. Using this operators with float will cause compiler errors.
		Vector<n, Data>& operator>>= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] >>= _d; return *this; }
		/// \copydoc Vector::operator>>=
		Vector<n, Data>& operator<<= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] <<= _d; return *this; }
		/// \copydoc Vector::operator>>=
		Vector<n, Data> operator>> (const Data _d) const				{ return Vector<n, Data>(this) >>= _d; }
		/// \copydoc Vector::operator>>=
		Vector<n, Data> operator<< (const Data _d) const				{ return Vector<n, Data>(this) <<= _d; }

		/// \brief Component wise shifting operator
		/// \copydetails Vector::operator>>=
		Vector<n, Data>& operator>>= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] >>= _v[i]; return *this; }
		Vector<n, Data>& operator<<= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] <<= _v[i]; return *this; }
		Vector<n, Data> operator>> (const Vector<n, Data>& _v) const	{ return Vector<n, Data>(this) >>= _v; }
		Vector<n, Data> operator<< (const Vector<n, Data>& _v) const	{ return Vector<n, Data>(this) <<= _v; }

		/// \brief Logical operator for integral types.
		/// \copydetails Vector::operator>>=
		Vector<n, Data>& operator|= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] |= _d; return *this; }
		Vector<n, Data>& operator&= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] &= _d; return *this; }
		Vector<n, Data>& operator^= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] ^= _d; return *this; }
		Vector<n, Data>& operator%= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] %= _d; return *this; }
		Vector<n, Data> operator| (const Data _d) const					{ return Vector<n, Data>(this) |= _d; }
		Vector<n, Data> operator& (const Data _d) const					{ return Vector<n, Data>(this) &= _d; }
		Vector<n, Data> operator^ (const Data _d) const					{ return Vector<n, Data>(this) ^= _d; }
		Vector<n, Data> operator% (const Data _d) const					{ return Vector<n, Data>(this) %= _d; }

		Vector<n, Data>& operator|= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] |= _v[i]; return *this; }
		Vector<n, Data>& operator&= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] &= _v[i]; return *this; }
		Vector<n, Data>& operator^= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] ^= _v[i]; return *this; }
		Vector<n, Data>& operator%= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] %= _v[i]; return *this; }
		Vector<n, Data> operator| (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) |= _d; }
		Vector<n, Data> operator& (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) &= _d; }
		Vector<n, Data> operator^ (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) ^= _d; }
		Vector<n, Data> operator% (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(this) %= _d; }
	protected:
		Data m_data[n];
	}; // class Vector

	//Arithmetic operators with Vector as RH
	template<int n, class Data>
	Vector<n, Data> operator+ (const Data _d, const Vector<n, Data>& _v) 				{ return Vector<n, Data>(_v) += _d; }
	template<int n, class Data>
	Vector<n, Data> operator- (const Data _d, const Vector<n, Data>& _v) 				{ return Vector<n, Data>(_v) -= _d; }
	template<int n, class Data>
	Vector<n, Data> operator* (const Data _d, const Vector<n, Data>& _v) 				{ return Vector<n, Data>(_v) *= _d; }
	// Division not commutative: What should this operator do?
	//template<int n, class Data>
	//Vector<n, Data> operator/ (const Data _d, const Vector<n, Data>& _v) 				{ return Vector<n, Data>(_v) /= _d; }

	// comparison for float
	template<int n>
	bool operator== (const Vector<n, float>& _v1, const Vector<n, float>& _v2)			{ for (int i = 0; i < n; i++) if (abs(_v1[i] - _v2[i])>EPSILON) return false; return true; }
	template<int n>
	bool operator!= (const Vector<n, float>& _v1, const Vector<n, float>& _v2)			{ for (int i = 0; i < n; i++) if (abs(_v1[i] - _v2[i])>EPSILON) return true; return false; }

	/// \brief Comparison for double. Uses the same EPSILON as float comparison
	template<int n>
	bool operator== (const Vector<n, double>& _v1, const Vector<n, double>& _v2)			{ for (int i = 0; i < n; i++) if (abs(_v1[i] - _v2[i])>EPSILON) return false; return true; }
	template<int n>
	bool operator!= (const Vector<n, double>& _v1, const Vector<n, double>& _v2)			{ for (int i = 0; i < n; i++) if (abs(_v1[i] - _v2[i])>EPSILON) return true; return false; }

	/// \brief Comparison for all other types with hard element equality.
	template<int n, class Data>
	bool operator== (const Vector<n, Data>& _v1, const Vector<n, Data>& _v2)			{ for (int i = 0; i < n; i++) if (_v1[i] != _v2[i]) return false; return true; }
	template<int n, class Data>
	bool operator!= (const Vector<n, Data>& _v1, const Vector<n, Data>& _v2)			{ for (int i = 0; i < n; i++) if (_v1[i] != _v2[i]) return true; return false; }

	/// \brief Cross product for vectors with 3 dimensions.
	template<class Data>
	Vector<3, Data> Cross(const Vector<3, Data>& _v1, const Vector<3, Data>& _v2)
	{
		Vector<3, Data> result;
		result.x = _v1.y * _v2.z - _v1.z * _v2.y;
		result.y = _v1.z * _v2.x - _v1.x * _v2.z;
		result.z = _v1.x * _v2.y - _v1.y * _v2.x;
		return result;
	}

	/// \brief Dot product (sum of component products).
	template<int n, class Data>
	Data Dot(const Vector<n, Data>& _v1, const Vector<n, Data>& _v2)
	{
		Data result = Data(0);
		for( int i=0; i<n; ++i ) result += _v1[i] * _v2[i];
		return result;
	}

	/// \brief Quadratic euclidean length: x²+y²+...
	template<int n, class Data>
	Data LengthSq(const Vector<n, Data>& _v)				{ return Dot(_v,_v); }

	/// \brief Euclidean length for floating point vectors
	template<int n>
	float Length(const Vector<n, float>& _v)				{ return sqrt( LengthSq(_v) ); }
	template<int n>
	double Length(const Vector<n, double>& _v)				{ return sqrt( LengthSq(_v) ); }




	// ********************************************************************* //
	// Common vector typedefs
	typedef Vector<4,int> IVec4;

} // namespace math