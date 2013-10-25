#pragma once

#include "vector3.hpp"

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
	Plane(const float _a, const float _b, const float _c, const float _d)	{float fInvLenght = Vec3(_a, _b, _c).LengthInv(); a = _a*fInvLenght; b = _b*fInvLenght; c = _c*fInvLenght; d = _d*fInvLenght;}
	Plane(const float* pfValue)												{float fInvLenght = Vec3(pfValue).LengthInv(); a = pfValue[0]*fInvLenght; b = pfValue[1]*fInvLenght; c = pfValue[2]*fInvLenght; d = pfValue[3]*fInvLenght;}
	Plane(const Vec3& _n, float _d)											{float fInvLenght = _n.LengthInv(); a = _n.x*fInvLenght; b = _n.y*fInvLenght; c = _n.z*fInvLenght; d = _d*fInvLenght;}
	Plane(const Vec3& _p, const Vec3& _n)									{float fInvLenght = _n.LengthInv(); a = _n.x*fInvLenght; b = _n.y*fInvLenght; c = _n.z*fInvLenght; d = -n.x * _p.x - n.y * _p.y - n.z * _p.z;}
	Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)					{Vec3 _n=cross(v3 - v2, v1 - v2); float fInvLenght = _n.LengthInv(); a = _n.x*fInvLenght; b = _n.y*fInvLenght; c = _n.z*fInvLenght; d = -n.x * v1.x - n.y * v1.y - n.z * v1.z;}

	// Casting-operators
	operator float* () {return (float*)(this);}

	// Assignment operator
	Plane& operator = (const Plane& p) {a = p.a; b = p.b; c = p.c; d = p.d; return *this;}

	inline float& operator[](int i)								{ return *((float*)this + i); }

	// Methods for plain calculations
	inline Vec3 CalcReflect(Vec3 vRay) {return (vRay - vRay.Dot(n)*2.0f*n);}
	inline Plane Normalize()												{const float fInvLength = n.LengthInv(); a *= fInvLength; b *= fInvLength; c *= fInvLength; d *= fInvLength;}
	inline float DotNormal(const Plane& p, const Vec3& v)					{return p.a * v.x + p.b * v.y + p.c * v.z;}
	inline float DotCoords(const Plane& p, const Vec3& v)					{return p.a * v.x + p.b * v.y + p.c * v.z + p.d;}
};

// Comparison operators
inline bool operator == (const Plane& a, const Plane& b) {if(a.a != b.a) return false; if(a.b != b.b) return false; if(a.c != b.c) return false; return a.d == b.d;}
inline bool operator != (const Plane& a, const Plane& b) {if(a.a != b.a) return true; if(a.b != b.b) return true; if(a.c != b.c) return true; return a.d != b.d;}


inline Plane normalize(const Plane& p)			{const float fInvLength = p.n.LengthInv(); return Plane(p.n * fInvLength, p.d * fInvLength);}

}; // namespace Math
