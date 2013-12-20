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
		/// \brief Create a Vector from a Vector by casting the primitive type
		template<class Data2>
		explicit Vector(const Vector<n, Data2>& _v)	{ for (int i = 0; i < n; i++) m_data[i] = static_cast<Data>(_v[i]); }
		/// \brief Casting to a Data array
		operator Data* ()				{ return m_data; }
		/// \brief Casting to a const Data array
		operator const Data* () const	{ return m_data; }

		/// \brief Specialized constructor for a 2D vector from components.
		Vector(Data _d1, Data _d2)						{ static_assert(n==2, "2 Component constructor can only be used for vectors of dimension 2."); m_data[0] = _d1; m_data[1] = _d2; }
		/// \brief Specialized constructor for a 3D vector from components.
		Vector(Data _d1, Data _d2, Data _d3)			{ static_assert(n==3, "3 Component constructor can only be used for vectors of dimension 3."); m_data[0] = _d1; m_data[1] = _d2; m_data[2] = _d3; }
		/// \brief Specialized constructor for a 4D vector from components.
		Vector(Data _d1, Data _d2, Data _d3, Data _d4)	{ static_assert(n==4, "4 Component constructor can only be used for vectors of dimension 4."); m_data[0] = _d1; m_data[1] = _d2; m_data[2] = _d3; m_data[3] = _d4; }
		/// \brief Specialized constructor for a 4D vector from a 3D vector + data.
		Vector(const Vector<3, Data>& _v, Data _d)		{ static_assert(n==4, "Vec3 + Component constructor can only be used for vectors of dimension 4."); for (int i = 0; i < 3; i++) m_data[i] = _v[i]; m_data[3] = _d; }

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
		template<class Data2>
		Vector<n, decltype(Data(0)+Data2(0))> operator+ (const Vector<n, Data2>& _v) const		{ Vector<n, decltype(Data(0)+Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] + _v[i]; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)-Data2(0))> operator- (const Vector<n, Data2>& _v) const		{ Vector<n, decltype(Data(0)+Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] - _v[i]; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)*Data2(0))> operator* (const Vector<n, Data2>& _v) const		{ Vector<n, decltype(Data(0)+Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] * _v[i]; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)/Data2(0))> operator/ (const Vector<n, Data2>& _v) const		{ Vector<n, decltype(Data(0)+Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] / _v[i]; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)+Data2(0))> operator+ (const Data2 _d) const	{ Vector<n, decltype(Data(0)+Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] + _d; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)-Data2(0))> operator- (const Data2 _d) const	{ Vector<n, decltype(Data(0)-Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] - _d; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)*Data2(0))> operator* (const Data2 _d) const	{ Vector<n, decltype(Data(0)*Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] * _d; return result; }
		template<class Data2>
		Vector<n, decltype(Data(0)/Data2(0))> operator/ (const Data2 _d) const	{ Vector<n, decltype(Data(0)/Data2(0))> result; for (int i = 0; i < n; i++) result[i] = m_data[i] / _d; return result; }

		/// \brief Shifting operator for integral types.
		/// \details The Data is kept template to support anything which defines
		///		shifting. Using this operators with float will cause compiler errors.
		Vector<n, Data>& operator>>= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] >>= _d; return *this; }
		/// \copydoc Vector::operator>>=
		Vector<n, Data>& operator<<= (const Data _d)					{ for (int i = 0; i < n; i++) m_data[i] <<= _d; return *this; }
		/// \copydoc Vector::operator>>=
		Vector<n, Data> operator>> (const Data _d) const				{ return Vector<n, Data>(*this) >>= _d; }
		/// \copydoc Vector::operator>>=
		Vector<n, Data> operator<< (const Data _d) const				{ return Vector<n, Data>(*this) <<= _d; }

		/// \brief Component wise shifting operator
		/// \copydetails Vector::operator>>=
		Vector<n, Data>& operator>>= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] >>= _v[i]; return *this; }
		Vector<n, Data>& operator<<= (const Vector<n, Data>& _v)		{ for (int i = 0; i < n; i++) m_data[i] <<= _v[i]; return *this; }
		Vector<n, Data> operator>> (const Vector<n, Data>& _v) const	{ return Vector<n, Data>(*this) >>= _v; }
		Vector<n, Data> operator<< (const Vector<n, Data>& _v) const	{ return Vector<n, Data>(*this) <<= _v; }

		/// \brief Logical operator for integral types.
		/// \copydetails Vector::operator>>=
		Vector<n, Data>& operator|= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] |= _d; return *this; }
		Vector<n, Data>& operator&= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] &= _d; return *this; }
		Vector<n, Data>& operator^= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] ^= _d; return *this; }
		Vector<n, Data>& operator%= (const Data _d)						{ for (int i = 0; i < n; i++) m_data[i] %= _d; return *this; }
		Vector<n, Data> operator| (const Data _d) const					{ return Vector<n, Data>(*this) |= _d; }
		Vector<n, Data> operator& (const Data _d) const					{ return Vector<n, Data>(*this) &= _d; }
		Vector<n, Data> operator^ (const Data _d) const					{ return Vector<n, Data>(*this) ^= _d; }
		Vector<n, Data> operator% (const Data _d) const					{ return Vector<n, Data>(*this) %= _d; }

		Vector<n, Data>& operator|= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] |= _v[i]; return *this; }
		Vector<n, Data>& operator&= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] &= _v[i]; return *this; }
		Vector<n, Data>& operator^= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] ^= _v[i]; return *this; }
		Vector<n, Data>& operator%= (const Vector<n, Data>& _v)			{ for (int i = 0; i < n; i++) m_data[i] %= _v[i]; return *this; }
		Vector<n, Data> operator| (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(*this) |= _v; }
		Vector<n, Data> operator& (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(*this) &= _v; }
		Vector<n, Data> operator^ (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(*this) ^= _v; }
		Vector<n, Data> operator% (const Vector<n, Data>& _v) const		{ return Vector<n, Data>(*this) %= _v; }
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
	Vector<3, Data> cross(const Vector<3, Data>& _v1, const Vector<3, Data>& _v2)
	{
		Vector<3, Data> result;
		result.x = _v1.y * _v2.z - _v1.z * _v2.y;
		result.y = _v1.z * _v2.x - _v1.x * _v2.z;
		result.z = _v1.x * _v2.y - _v1.y * _v2.x;
		return result;
	}

	/// \brief Dot product (sum of component products).
	template<int n, class Data>
	Data dot(const Vector<n, Data>& _v1, const Vector<n, Data>& _v2)
	{
		Data result = Data(0);
		for( int i=0; i<n; ++i ) result += _v1[i] * _v2[i];
		return result;
	}

	/// \brief Quadratic euclidean length: x²+y²+...
	template<int n, class Data>
	Data lengthSq(const Vector<n, Data>& _v)				{ return dot(_v,_v); }

	/// \brief Euclidean length for floating point vectors
	template<int n>
	float length(const Vector<n, float>& _v)				{ return sqrt( lengthSq(_v) ); }
	template<int n>
	double length(const Vector<n, double>& _v)				{ return sqrt( lengthSq(_v) ); }

	/// \brief Component wise maximum.
	template<int n, class Data>
	Vector<n, Data> max(const Vector<n, Data>& _v1, const Vector<n, Data>& _v2)
	{
		Vector<n, Data> result;
		for( int i=0; i<n; ++i ) result = max(_v1[i],  _v2[i]);
		return result;
	}

	/// \brief Component wise minimum.
	template<int n, class Data>
	Vector<n, Data> min(const Vector<n, Data>& _v1, const Vector<n, Data>& _v2)
	{
		Vector<n, Data> result;
		for( int i=0; i<n; ++i ) result = min(_v1[i],  _v2[i]);
		return result;
	}

	/// \brief Component wise absolute value.
	template<int n, class Data>
	Vector<n, Data> abs(const Vector<n, Data>& _v)
	{
		Vector<n, Data> result;
		for( int i=0; i<n; ++i ) result = abs(_v);
		return result;
	}




	// ********************************************************************* //
	// Common vector typedefs
	//typedef Vector<3,int> IVec3;
	typedef Vector<4,int> IVec4;

} // namespace Math


namespace std {

	template <class _Kty> struct hash;

	/// \brief Custom hash function for vectors.
	template <int n, class Data>
	struct hash<Math::Vector<n,Data>>
	{
		std::size_t operator()(const Math::Vector<n,Data>& _key) const
		{
			hash<Data> hashfunc;
			size_t h = 0;
			for( int i = 0; i < n; ++i )
			{
				// A sum of hashes is universal:
				// http://en.wikipedia.org/wiki/Universal_hashing#Hashing_vectors
				h += hashfunc(_key[i]);
			}
			return h;
		}
	};

	/// \brief Stronger hash function for 2D vectors.
	/// \details This function generates different hashes for permutations too.
	template <class Data>
	struct hash<Math::Vector<2,Data>>
	{
		std::size_t operator()(const Math::Vector<2,Data>& _key) const
		{
			hash<Data> hashfunc;
			size_t h = 0xbd73a0fb;
			h += hashfunc(_key[0]) * 0xf445f0a9;
			h += hashfunc(_key[1]) * 0x5c23b2e1;
			return h;
		}
	};

	/// \brief Stronger hash function for 3D vectors.
	/// \details This function generates different hashes for permutations too.
	template <class Data>
	struct hash<Math::Vector<3,Data>>
	{
		std::size_t operator()(const Math::Vector<3,Data>& _key) const
		{
			hash<Data> hashfunc;
			size_t h = 0xbd73a0fb;
			h += hashfunc(_key[0]) * 0xf445f0a9;
			h += hashfunc(_key[1]) * 0x5c23b2e1;
			h += hashfunc(_key[2]) * 0x7d25f695;
			return h;
		}
	};

	/// \brief Stronger hash function for 4D vectors.
	/// \details This function generates different hashes for permutations too.
	template <class Data>
	struct hash<Math::Vector<4,Data>>
	{
		std::size_t operator()(const Math::Vector<4,Data>& _key) const
		{
			hash<Data> hashfunc;
			size_t h = 0xbd73a0fb;
			h += hashfunc(_key[0]) * 0xf445f0a9;
			h += hashfunc(_key[1]) * 0x5c23b2e1;
			h += hashfunc(_key[2]) * 0x7d25f695;
			h += hashfunc(_key[2]) * 0x11d6a9f3;
			return h;
		}
	};
} // namespace std