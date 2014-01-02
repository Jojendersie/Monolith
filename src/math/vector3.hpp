#pragma once

#include "mathbase.hpp"
#include "vector2.hpp"

namespace Math {

// ******************************************************************************** //
/// \brief The class for 3D vectors (element of R^3)
class Vec3
{
public:
	// Variables
	union
	{
		struct
		{
			float x;	// Coordinates
			float y;
			float z;
		};

		struct
		{
			float u;	// Nomenclature for texture coordinates
			float v;
			float w;
		};
	};

	// Constructors
	Vec3()																						{}
	Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z)												{}
	Vec3(const Vec2& v, float f) : x(v.x), y(v.y), z(f)											{}
	Vec3(float f, const Vec2& v) : x(f), y(v.x), z(v.y)											{}
	explicit Vec3(const float f) : x(f), y(f), z(f)												{}
	Vec3(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z)					{}
	explicit Vec3(const float* pfComponent) : x(pfComponent[0]), y(pfComponent[1]), z(pfComponent[2])	{}

	// Casting-operators
	operator float* ()												{return (float*)(this);}
	operator const float* () const									{return (const float*)(this);}

	// Casting to "color" with alpha channel==255
	operator uint32_t () const										{return (uint8_t(saturate(x)*255.0)<<24) | (uint8_t(saturate(y)*255.0)<<16) | (uint8_t(saturate(z)*255.0)<<8) | 255;}

	// Assignment operators
	Vec3& operator = (const Vec3& v)								{x = v.x; y = v.y; z = v.z; return *this;}
	Vec3& operator = (const float f)								{x = f; y = f; z = f; return *this;}
	Vec3& operator += (const Vec3& v)								{x += v.x; y += v.y; z += v.z; return *this;}
	Vec3& operator -= (const Vec3& v)								{x -= v.x; y -= v.y; z -= v.z; return *this;}
	Vec3& operator *= (const Vec3& v)								{x *= v.x; y *= v.y; z *= v.z; return *this;}
	Vec3& operator *= (const float f)								{x *= f; y *= f; z *= f; return *this;}
	Vec3& operator /= (const Vec3& v)								{x /= v.x; y /= v.y; z /= v.z; return *this;}
	Vec3& operator /= (float f)										{f = 1/f; x *= f; y *= f; z *= f; return *this;}

	inline float& operator[](int i)									{ return *((float*)this + i); }

	// ******************************************************************************** //
	// Arithmetic operators
	inline Vec3 operator + (const Vec3& b) const					{return Vec3(x + b.x, y + b.y, z + b.z);}
	inline Vec3 operator - (const Vec3& b) const					{return Vec3(x - b.x, y - b.y, z - b.z);}
	inline Vec3 operator - () const									{return Vec3(-x, -y, -z);}
	inline Vec3 operator * (const Vec3& b) const					{return Vec3(x*b.x, y*b.y, z*b.z);}
	inline Vec3 operator * (const float f) const					{return Vec3(x * f, y * f, z * f);}
	inline Vec3 operator / (const Vec3& b) const					{return Vec3(x / b.x, y / b.y, z / b.z);}
	inline Vec3 operator / (float f) const							{f = 1/f; return Vec3(x * f, y * f, z * f);}

	// ******************************************************************************** //
	// Comparison operators
	inline bool operator == (const Vec3& b) const					{if(abs(x - b.x) > EPSILON) return false; if(abs(y - b.y) > EPSILON) return false; return abs(z - b.z) < EPSILON;}
	inline bool operator != (const Vec3& b) const					{if(abs(x - b.x) > EPSILON) return true; if(abs(y - b.y) > EPSILON) return true; return abs(z - b.z) > EPSILON;}

	// ******************************************************************************** //
	// Vector3 functions
	// The non-static and non-const functions always change the calling object
	inline float		Length() const								{return sqrt(x * x + y * y + z * z);}
	inline float		LengthSq() const							{return x * x + y * y + z * z;}
	inline float		LengthInv() const							{return invsqrt(x * x + y * y + z * z);}
	inline const Vec3&	Normalize()									{*this *= invsqrt(x * x + y * y + z * z); return *this;}
	inline Vec3			Cross(const Vec3& v) const					{return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);}
	inline float		Dot(const Vec3& v) const					{return x * v.x + y * v.y + z * v.z;}
	
	// Create a uniformly distributed random direction vector ( length == 1 ).
	static Vec3			RandomDirection();

	// Create a random point uniformly distributed in the [-1,1]^3 cube.
	static Vec3			RandomPosition();

	// Rotate a vector around some axis. This transformation uses quaternion
	// math. Use this if you want to rotate only one vector. For the rotation
	// of multiple vectors a matrix is more efficient!
	// Speed: Debug 480, Release 176
	Vec3			Rotate(const Vec3& vAxis, float fAngle) const;
};

// ******************************************************************************** //
inline Vec3 operator * (const float f, const Vec3& v)				{return Vec3(v.x * f, v.y * f, v.z * f);}

inline Vec3	normalize(const Vec3& v)								{return v * invsqrt(v.x * v.x + v.y * v.y + v.z * v.z);}
inline float dot(const Vec3& v1, const Vec3& v2)					{return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
inline Vec3	cross(const Vec3& v1, const Vec3& v2)					{return Vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
inline float angle(const Vec3& v1, const Vec3& v2)					{return acos((v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) * invsqrt((v1.x * v1.x + v1.y * v1.y + v1.z * v1.z) * (v2.x * v2.x + v2.y * v2.y + v2.z * v2.z)));}

// ******************************************************************************** //
// Lerp, Abs, Min and Max have to be in global scope to override the default template
inline Vec3	min(const Vec3& v1, const Vec3& v2)						{return Vec3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z));}
inline Vec3	max(const Vec3& v1, const Vec3& v2)						{return Vec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));}
inline Vec3	abs(const Vec3& v1)										{return Vec3(abs(v1.x), abs(v1.y), abs(v1.z));}
inline Vec3	lerp(const Vec3& v1, const Vec3& v2, const float t)		{return Vec3(v1.x+t*(v2.x-v1.x), v1.y+t*(v2.y-v1.y), v1.z+t*(v2.z-v1.z));}
/// \brief The spherical interpolation applies only to normal vectors
Vec3 slerp(const Vec3& v1, const Vec3& v2, const float t);



}; // namespace Math