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
		Vector(Data _d)			{for (int i = 0; i < n; i++) m_data[i] = _d; }
		/// \brief Create a Vector from an array
		Vector(Data* _d)		{ for (int i = 0; i < n; i++) m_data[i] = _d[i]; }
		/// \brief Create a Vector from a Vector
		Vector(const Vector<n, Data>& _v)	{ for (int i = 0; i < n; i++) m_data[i] = _v[i]; }
		/// \brief Casting to a Data array
		operator Data* ()				{ return m_data; }
		/// \brief Casting to a const Data array
		operator const Data* () const	{ return m_data; }

		/// \brief Writa access to an element
		Data& operator [](int _pos)	{ return m_data[_pos]; }
		/// \brief Read access to elements
		Data operator [](int _pos)	{ return m_data[_pos]; }

		// assignment operators
		Vector<n, Data>& operator = (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] = _v.m_data[i]; return this; }
		Vector<n, Data>& operator += (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] += _v.m_data[i]; return this; }
		Vector<n, Data>& operator -= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] -= _v.m_data[i]; return this; }
		Vector<n, Data>& operator *= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] *= _v.m_data[i]; return this; }
		Vector<n, Data>& operator /= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] /= _v.m_data[i]; return this; }
		Vector<n, Data>& operator = (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] = _d; return this; }
		Vector<n, Data>& operator += (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] += _d; return this; }
		Vector<n, Data>& operator -= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] -= _d; return this; }
		Vector<n, Data>& operator *= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] /= _d; return this; }
		Vector<n, Data>& operator /= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] *= _d; return this; }

		//Arithmetic operators
		Vector<n, Data> operator+ (const Vector<n, Data> _v) const								{ return Vector<n, Data>(this) += _v; }
		Vector<n, Data> operator- (const Vector<n, Data> _v) const								{ return Vector<n, Data>(this) -= _v; }
		Vector<n, Data> operator* (const Vector<n, Data> _v) const								{ return Vector<n, Data>(this) *= _v; }
		Vector<n, Data> operator/ (const Vector<n, Data> _v) const								{ return Vector<n, Data>(this) /= _v; }
		Vector<n, Data> operator+ (const Data _d) const										{ return Vector<n, Data>(this) += _d; }
		Vector<n, Data> operator- (const Data _d) const										{ return Vector<n, Data>(this) -= _d; }
		Vector<n, Data> operator* (const Data _d) const										{ return Vector<n, Data>(this) *= _d; }
		Vector<n, Data> operator/ (const Data _d) const										{ return Vector<n, Data>(this) /= _d; }

	protected:
		Data m_data[n];
	}; // class Vector

	//Arithmetic operators with Vector as RH
	template<int n, class Data>
	Vector<n, Data> operator+ (const Data _d, const Vector<n, Data> _v) 				{ return Vector<n, Data>(_v) += _d; }
	template<int n, class Data>
	Vector<n, Data> operator- (const Data _d, const Vector<n, Data> _v) 				{ return Vector<n, Data>(_v) -= _d; }
	template<int n, class Data>
	Vector<n, Data> operator* (const Data _d, const Vector<n, Data> _v) 				{ return Vector<n, Data>(_v) *= _d; }
	template<int n, class Data>
	Vector<n, Data> operator/ (const Data _d, const Vector<n, Data> _v) 				{ return Vector<n, Data>(_v) /= _d; }

	// comparison for float
	template<int n>
	bool operator== (const Vector<n, float> _v1, const Vector<n, float> _v2)				{ for (int i = 0; i < n; i++) if (abs(_v1[i] - _v2[i])>EPSILON) return false; return true; }
	template<int n>
	bool operator!= (const Vector<n, float> _v1, const Vector<n, float> _v2)				{ for (int i = 0; i < n; i++) if (abs(_v1[i] - _v2[i])>EPSILON) return true; return false; }

	// comparison for int
	template<int n>
	bool operator== (const Vector<n, int> _v1, const Vector<n, int> _v2)				{ for (int i = 0; i < n; i++) if (_v1[i] != _v2[i]) return false; return true; }
	template<int n>
	bool operator!= (const Vector<n, int> _v1, const Vector<n, int> _v2)				{ for (int i = 0; i < n; i++) if (_v1[i] != _v2[i]) return true; return false; }


} // namespace math