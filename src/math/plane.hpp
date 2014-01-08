#pragma once

#include "vector.hpp"

namespace Math {

// ******************************************************************************** //
// A class for planes
class Plane
{
public:
	// Variables
	union
	{
		struct
		{
			float a;		// Variables of plane equation
			float b;
			float c;
			float d;
		};

		struct
		{
			Vec3		n;	// Normal vector
			float		d;	// Distance to 0
		};
	};

	// Constructors
	Plane()																	{}
	Plane(const Plane& p) : a(p.a), b(p.b), c(p.c), d(p.d)					{}
	Plane(const float _a, const float _b, const float _c, const float _d)	{ float invLength = 1.0f / sqrt(_a*_a+_b*_b+_c*_c); a = _a*invLength; b = _b*invLength; c = _c*invLength; d = _d*invLength; }
	explicit Plane(const float* _value)										{ float invLength = 1.0f / length(Vec3(_value)); a = _value[0]*invLength; b = _value[1]*invLength; c = _value[2]*invLength; d = _value[3]*invLength; }
	Plane(const Vec3& _n, float _d)											{ float invLength = 1.0f / length(_n); a = _n[0]*invLength; b = _n[1]*invLength; c = _n[2]*invLength; d = _d*invLength; }
	Plane(const Vec3& _p, const Vec3& _n)									{ float invLength = 1.0f / length(_n); a = _n[0]*invLength; b = _n[1]*invLength; c = _n[2]*invLength; d = -n[0] * _p[0] - n[1] * _p[1] - n[2] * _p[2]; }
	Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)					{ Vec3 _n=cross(v3 - v2, v1 - v2); float invLength = length(_n); a = _n[0]*invLength; b = _n[1]*invLength; c = _n[2]*invLength; d = -n[0] * v1[0] - n[1] * v1[1] - n[2] * v1[2]; }
	Plane(const Vec4& _plane)												{ float invLength = 1.0f / sqrt(sqr(_plane[0])+sqr(_plane[1])+sqr(_plane[2])); a = _plane[0]*invLength; b = _plane[1]*invLength; c = _plane[2]*invLength; d = _plane[3]*invLength; }

	// Casting-operators
	operator float* ()				{ return (float*)(this); }
	operator const Vec4& () const	{ return *(Vec4*)(this); }
	operator Vec4 ()				{ return *(Vec4*)(this); }

	// Assignment operator
	Plane& operator = (const Plane& p)						{ a = p.a; b = p.b; c = p.c; d = p.d; return *this; }

	inline float& operator[](int i)							{ return *((float*)this + i); }

	// Methods for plain calculations
	inline Vec3 CalcReflect(const Vec3& _ray)				{ return (_ray - dot(_ray,n)*2.0f*n); }
	inline Plane Normalize()								{ float invLength = 1.0f / length(n); a *= invLength; b *= invLength; c *= invLength; d *= invLength; }
	inline float DotNormal(const Vec3& _v)					{ return a * _v[0] + b * _v[1] + c * _v[2]; }
	inline float DotCoords(const Vec3& _v)					{ return a * _v[0] + b * _v[1] + c * _v[2] + d; }
};

// Comparison operators
inline bool operator == (const Plane& a, const Plane& b) {if(a.a != b.a) return false; if(a.b != b.b) return false; if(a.c != b.c) return false; return a.d == b.d;}
inline bool operator != (const Plane& a, const Plane& b) {if(a.a != b.a) return true; if(a.b != b.b) return true; if(a.c != b.c) return true; return a.d != b.d;}


inline Plane normalize(const Plane& _p)			{const float invLength = 1.0f / length(_p.n); return Plane(_p.n * invLength, _p.d * invLength);}

}; // namespace Math
