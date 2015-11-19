#pragma once

#include "ei/vector.hpp"

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
			ei::Vec3	n;	// Normal vector
			float		d;	// Distance to 0
		};
	};

	// Constructors
	Plane()																	{}
	Plane(const Plane& p) : a(p.a), b(p.b), c(p.c), d(p.d)					{}
	Plane(const float _a, const float _b, const float _c, const float _d)	{ float invLength = 1.0f / sqrt(_a*_a+_b*_b+_c*_c); a = _a*invLength; b = _b*invLength; c = _c*invLength; d = _d*invLength; }
	explicit Plane(const float* _value)										{ float invLength = 1.0f / sqrt(_value[0]*_value[0]+_value[1]*_value[1]+_value[2]*_value[2]); a = _value[0]*invLength; b = _value[1]*invLength; c = _value[2]*invLength; d = _value[3]*invLength; }
	Plane(const ei::Vec3& _n, float _d)										{ float invLength = 1.0f / len(_n); a = _n[0]*invLength; b = _n[1]*invLength; c = _n[2]*invLength; d = _d*invLength; }
	Plane(const ei::Vec3& _p, const ei::Vec3& _n)							{ float invLength = 1.0f / len(_n); a = _n[0]*invLength; b = _n[1]*invLength; c = _n[2]*invLength; d = -n[0] * _p[0] - n[1] * _p[1] - n[2] * _p[2]; }
	Plane(const ei::Vec3& v1, const ei::Vec3& v2, const ei::Vec3& v3)		{ ei::Vec3 _n=cross(v3 - v2, v1 - v2); float invLength = len(_n); a = _n[0]*invLength; b = _n[1]*invLength; c = _n[2]*invLength; d = -n[0] * v1[0] - n[1] * v1[1] - n[2] * v1[2]; }
	Plane(const ei::Vec4& _plane)											{ float invLength = 1.0f / sqrt(ei::sq(_plane[0])+ei::sq(_plane[1])+ei::sq(_plane[2])); a = _plane[0]*invLength; b = _plane[1]*invLength; c = _plane[2]*invLength; d = _plane[3]*invLength; }

	// Casting-operators
	operator float* ()					{ return (float*)(this); }
	operator const ei::Vec4& () const	{ return *(ei::Vec4*)(this); }
	operator ei::Vec4 ()				{ return *(ei::Vec4*)(this); }

	// Assignment operator
	Plane& operator = (const Plane& p)						{ a = p.a; b = p.b; c = p.c; d = p.d; return *this; }

	inline float& operator[](int i)							{ return *((float*)this + i); }

	// Methods for plain calculations
	inline ei::Vec3 CalcReflect(const ei::Vec3& _ray) const		{ return (_ray - dot(_ray,n)*2.0f*n); }
	inline Plane Normalize()									{ float invLength = 1.0f / len(n); a *= invLength; b *= invLength; c *= invLength; d *= invLength; }
	inline float DotNormal(const ei::Vec3& _v) const			{ return a * _v[0] + b * _v[1] + c * _v[2]; }
	inline float DotCoords(const ei::Vec3& _v) const			{ return a * _v[0] + b * _v[1] + c * _v[2] + d; }
};

// Comparison operators
inline bool operator == (const Plane& a, const Plane& b) {if(a.a != b.a) return false; if(a.b != b.b) return false; if(a.c != b.c) return false; return a.d == b.d;}
inline bool operator != (const Plane& a, const Plane& b) {if(a.a != b.a) return true; if(a.b != b.b) return true; if(a.c != b.c) return true; return a.d != b.d;}


inline Plane normalize(const Plane& _p)			{const float invLength = 1.0f / len(_p.n); return Plane(_p.n * invLength, _p.d * invLength);}

}; // namespace Math
