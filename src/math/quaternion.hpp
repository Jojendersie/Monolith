#pragma once

#include "vector.hpp"

namespace Math {

class Mat4x4;

// ******************************************************************************** //
// Union quaternions (4D - complex numbers) for rotation in 3D space
// Scalar operations are disabled because they only change the length of the quaternion
class Quaternion
{
private:
	Quaternion& operator *= (float f) { i*=f; j*=f; k*=f; r*=f; return *this; }
public:
	// Variables
	float i, j, k, r;

	// Constructors
	Quaternion() : i(0), j(0), k(0), r(1)                                                                                {}
	Quaternion( const Quaternion& _q ) : i(_q.i), j(_q.j), k(_q.k), r(_q.r)                {}

	/// \brief Creation from unnormalized vector
	explicit Quaternion( const Vec4& _v ) : i(_v[0]), j(_v[1]), k(_v[2]), r(_v[3])                                        { Normalize(); }

	// From arbitrary components (attention: normalization - use tricks if you
	// know what you do (Quaternion q(); q.i=...) )
	Quaternion( float r, float i, float j, float k ) : i(i), j(j), k(k), r(r)                { Normalize(); }

	// arbitrary NORMALIZED axis + Angle.
	Quaternion( const Vec3& _axis, float _a )
	{
		// Using -sin to get the same result as for the matrix.
		// TODO: Matrix wrong? - the conjugated = inverse rotation
		float fSin = float( - sin( _a * 0.5 ) );
		r = float( cos( _a * 0.5 ) );
		i = _axis[0] * fSin;
		j = _axis[1] * fSin;
		k = _axis[1] * fSin;
	}
	// arbitrary axis with |axis| = Angle.
	Quaternion(const Vec3& _axis)
	{
		// Using -sin to get the same result as for the matrix.
		// TODO: Matrix wrong? - the conjugated = inverse rotation
		float l = length(_axis);
		float factor = float(-sin(l * 0.5))/l;
		r = float(cos(l * 0.5));
		i = _axis[0] * factor;
		j = _axis[1] * factor;
		k = _axis[1] * factor;
	}

	// From Matrix
	Quaternion(const Mat4x4& _m);

	// From Euler angles
	Quaternion( float _fYaw, float _fPitch, float _fRoll );
	// C++11 variant: Quaternion( float _fYaw, float _fPitch, float _fRoll ) : Quaternion( MatrixRotation( _fYaw, _fPitch, _fRoll ) ) {}

	// Functions
	inline float                                Length() const                                                                        {return sqrt(i * i + j * j + k * k + r * r);}
	inline float                                LengthSq() const                                                                {return i * i + j * j + k * k + r * r;}
	inline float                                LengthInv() const                                                                {return invsqrt(i * i + j * j + k * k + r * r);}
	inline const Quaternion&        Normalize()                                                                                {*this *= invsqrt(i * i + j * j + k * k + r * r); return *this;}
	inline float                                Dot(const Quaternion& b) const                                        {return i * b.i + j * b.j + k * b.k + r * b.r;}

	/// \brief Compute the x axis of the local coordinate system.
	inline Vec3                                        XAxis() const { return Vec3( 1.0f-2.0f*j*j-2.0f*k*k, 2.0f*i*j+2.0f*k*r, 2.0f*i*k-2.0f*j*r ); }
	/// \brief Compute the y axis of the local coordinate system.
	inline Vec3                                        YAxis() const { return Vec3( 2.0f*i*j-2.0f*k*r, 1.0f-2.0f*i*i-2.0f*k*k, 2.0f*j*k+2.0f*i*r ); }
	/// \brief Compute the z axis of the local coordinate system.
	inline Vec3                                        ZAxis() const { return Vec3( 2.0f*i*k+2.0f*j*r, 2.0f*j*k-2.0f*i*r, 1.0f-2.0f*i*i-2.0f*j*j ); }

	// Casting-operators
	operator float* ()					{return (float*)(&i);}
	operator const float* () const		{return (const float*)(&i);}
	operator Vec4 () const				{return *this;}
	operator Mat4x4 () const;
	operator Vec3 () const;
	operator Mat3x3 () const;

	// Comparison operators
	inline bool operator == (const Quaternion& b) { if(abs(r-b.r) > EPSILON) return false; if(abs(i-b.i) > EPSILON) return false; if(abs(j-b.j) > EPSILON) return false; return abs(k-b.k) < EPSILON; }
	inline bool operator != (const Quaternion& b) { if(abs(r-b.r) >= EPSILON) return true; if(abs(i-b.i) >= EPSILON) return true; if(abs(j-b.j) >= EPSILON) return true; return abs(k-b.k) >= EPSILON; }

	// Assignment operators
	Quaternion& operator += (const Quaternion& b) { i+=b.i; j+=b.j; k+=b.k; r+=b.r; Normalize(); return *this; }
	Quaternion& operator -= (const Quaternion& b) { i-=b.i; j-=b.j; k-=b.k; r-=b.r; Normalize(); return *this; }
	Quaternion& operator *= (const Quaternion& b)
	{
		// Quaternion multiplication - non commutative (a*b != a*b)
		float nr = r*b.r - i*b.i - j*b.j - k*b.k;
		float ni = r*b.i + i*b.r + j*b.k - k*b.j;
		float nj = r*b.j + j*b.r + k*b.i - i*b.k;
		k = r*b.k + k*b.r + i*b.j - j*b.i;
		r = nr;
		i = ni;
		j = nj;
		Normalize();
		return *this;
	}
	Quaternion& operator /= (float f) { f = 1/f; i*=f; j*=f; k*=f; return *this; }

	// a/=b <=> a=a*(b^-1) <=> a=a*~b (~b = b conjugated)
	Quaternion& operator /= (const Quaternion& b)
	{
		float nr = r*b.r + i*b.i + j*b.j + k*b.k;
		float ni = - r*b.i + i*b.r - j*b.k + k*b.j;
		float nj = - r*b.j + j*b.r - k*b.i + i*b.k;
		k = - r*b.k + k*b.r - i*b.j + j*b.i;
		r = nr;
		i = ni;
		j = nj;
		return *this;
	}
	Quaternion& operator = (const Quaternion& _q) { i=_q.i; j=_q.j; k=_q.k; r=_q.r; return *this; }
};

// ******************************************************************************** //
// Arithmetic operators
inline Quaternion operator + (const Quaternion& a, const Quaternion& b)        {return Quaternion(a.r+b.r, a.i+b.i, a.j+b.j, a.k+b.k);}
inline Quaternion operator - (const Quaternion& a, const Quaternion& b)        {return Quaternion(a.r-b.r, a.i-b.i, a.j-b.j, a.k-b.k);}
inline Quaternion operator - (const Quaternion& a)                             {Quaternion b; b.r=-a.r; b.i=-a.i; b.j=-a.j; b.k=-a.k; return b;}        // Avoiding the normalize - no use of component constructor
// Conjugation / inversion of a
inline Quaternion operator ~ (const Quaternion& a)                             {Quaternion b; b.r=a.r; b.i=-a.i; b.j=-a.j; b.k=-a.k; return b;}        // Avoiding the normalize - no use of component constructor
// Quaternion multiplication - non commutative (a*b != a*b)
inline Quaternion operator * (const Quaternion& a, const Quaternion& b)
{
	// Return with implicit normalization
	return Quaternion(
		a.r*b.r - a.i*b.i - a.j*b.j - a.k*b.k,
		a.r*b.i + a.i*b.r + a.j*b.k - a.k*b.j,
		a.r*b.j + a.j*b.r + a.k*b.i - a.i*b.k,
		a.r*b.k + a.k*b.r + a.i*b.j - a.j*b.i
		);
}

// a/b <=> a/(b^-1) <=> a/~b (~b = b conjugated)
inline Quaternion operator / (const Quaternion& a, const Quaternion& b)
{
	// Return with implicit normalization
	return Quaternion(
		a.r*b.r + a.i*b.i + a.j*b.j + a.k*b.k,
		- a.r*b.i + a.i*b.r - a.j*b.k + a.k*b.j,
		- a.r*b.j + a.j*b.r - a.k*b.i + a.i*b.k,
		- a.r*b.k + a.k*b.r - a.i*b.j + a.j*b.i
		);
}

//Quaternion operator * (const Quaternion& a, const float f)                        {return OrQuaternion(v.m_fS * f; v.m_v3 * f);}
//Quaternion operator * (const float f, const Quaternion& a)                        {return OrQuaternion(v.m_fS * f; v.m_v3 * f);}
//Quaternion operator / (const Quaternion& v, const float f)                        {return OrQuaternion(v.m_v4 / f);}

// ******************************************************************************** //
// Functions
// The spherical interpolation for union quaternions
Quaternion Slerp(const Quaternion& a, const Quaternion& b, const float t);

inline Quaternion normalize(const Quaternion& a)                                        {return Quaternion( a.r, a.i, a.j, a.k );}
inline float dot(const Quaternion& a, const Quaternion& b)                        {return a.i * b.i + a.j * b.j + a.k * b.k + a.r * b.r;}
inline float angle(const Quaternion& a, const Quaternion& b)                {return acos( dot( a, b ) );}

}; // namespace Math

