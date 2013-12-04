#pragma once

#include "mathbase.hpp"
#include "vector2.hpp"

namespace Math {

	class IVec3;

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
	Vec3(const IVec3& v);
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
inline Vec3 orth(const Vec3& v)										{return (v.y == 0 && v.z == 0) ? Vec3(0, v.x, 0) : Vec3(0, v.z, -v.y); }

// ******************************************************************************** //
// Lerp, Abs, Min and Max have to be in global scope to override the default template
inline Vec3	min(const Vec3& v1, const Vec3& v2)						{return Vec3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z));}
inline Vec3	max(const Vec3& v1, const Vec3& v2)						{return Vec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));}
inline Vec3	abs(const Vec3& v1)										{return Vec3(abs(v1.x), abs(v1.y), abs(v1.z));}
inline Vec3	lerp(const Vec3& v1, const Vec3& v2, const float t)		{return Vec3(v1.x+t*(v2.x-v1.x), v1.y+t*(v2.y-v1.y), v1.z+t*(v2.z-v1.z));}
/// \brief The spherical interpolation applies only to normal vectors
Vec3 slerp(const Vec3& v1, const Vec3& v2, const float t);




// ******************************************************************************** //
// \brief Equivalent to Vec3, but with integer numbers (element of Z^3).
class IVec3
{
public:
	// Variables
	union
	{
		struct
		{
			int x;	// Coordinates
			int y;
			int z;
		};

		struct
		{
			int u;	// Nomenclature for texture coordinates
			int v;
			int w;
		};
	};

	// Constructors
	IVec3()																						{}
	IVec3(const IVec3& v) : x(v.x), y(v.y), z(v.z)												{}
	IVec3(const Vec3& v) : x(int(v.x)), y(int(v.y)), z(int(v.z))								{}
	IVec3(const int i) : x(i), y(i), z(i)														{}
	IVec3(const int _x, const int _y, const int _z) : x(_x), y(_y), z(_z)						{}
	IVec3(const int* piComponent) : x(piComponent[0]), y(piComponent[1]), z(piComponent[2])		{}

	// Casting-Operators
	operator int* ()												{return (int*)(this);}
	operator const int* () const									{return (const int*)(this);}

	// Casting to "color" with alpha channel==255
	operator uint32_t () const											{return (clamp(x, 0, 255)<<24) | (clamp(y, 0, 255)<<16) | (clamp(z, 0, 255)<<8) | 255;}

	// Assignment operators
	IVec3& operator = (const IVec3& v)								{x = v.x; y = v.y; z = v.z; return *this;}
	IVec3& operator = (const int i)									{x = i; y = i; z = i; return *this;}
	IVec3& operator += (const IVec3& v)								{x += v.x; y += v.y; z += v.z; return *this;}
	IVec3& operator -= (const IVec3& v)								{x -= v.x; y -= v.y; z -= v.z; return *this;}
	IVec3& operator *= (const IVec3& v)								{x *= v.x; y *= v.y; z *= v.z; return *this;}
	IVec3& operator *= (const int i)								{x *= i; y *= i; z *= i; return *this;}
	IVec3& operator /= (const IVec3& v)								{x /= v.x; y /= v.y; z /= v.z; return *this;}
	IVec3& operator /= (int i)										{x /= i; y /= i; z /= i; return *this;}

	inline int& operator[](int i)									{ return *((int*)this + i); }

	// ******************************************************************************** //
	// Arithmetic operators
	inline IVec3 operator + (const IVec3& b) const					{return IVec3(x + b.x, y + b.y, z + b.z);}
	inline IVec3 operator - (const IVec3& b) const					{return IVec3(x - b.x, y - b.y, z - b.z);}
	inline IVec3 operator - () const								{return IVec3(-x, -y, -z);}
	inline IVec3 operator * (const IVec3& b) const					{return IVec3(x*b.x, y*b.y, z*b.z);}
	inline IVec3 operator * (const int i) const						{return IVec3(x * i, y * i, z * i);}
	inline Vec3 operator * (const float f) const					{return Vec3(x * f, y * f, z * f);}
	inline IVec3 operator / (const IVec3& b) const					{return IVec3(x / b.x, y / b.y, z / b.z);}
	inline IVec3 operator / (int i) const							{return IVec3(x / i, y / i, z / i);}
	inline Vec3 operator / (float f) const							{return Vec3(x / f, y / f, z / f);}

	// ******************************************************************************** //
	// Comparison operators
	inline bool operator == (const IVec3& b) const					{if(x != b.x) return false; if(y != b.y) return false; return z == b.z;}
	inline bool operator != (const IVec3& b) const					{if(x != b.x) return true; if(y != b.y) return true; return z != b.z;}

	// ******************************************************************************** //
	// Vector3 functions
	// The non-static and non-const functions always change the calling object
	inline float		Length() const								{return sqrt(float(x * x + y * y + z * z));}
	inline int			LengthSq() const							{return x * x + y * y + z * z;}
	inline const IVec3&	Normalize(int iL)							{float fL = iL*invsqrt(float(x * x + y * y + z * z)); x = int(x*fL); y = int(y*fL); z = int(z*fL); return *this;}
	inline IVec3		Cross(const IVec3& v) const					{return IVec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);}
	inline int			Dot(const IVec3& v) const					{return x * v.x + y * v.y + z * v.z;}
	// Creates an vector with length 1000 and random direction
	static IVec3		Random();
};

// ******************************************************************************** //
inline IVec3 operator * (const int i, const IVec3& v)				{return IVec3(v.x * i, v.y * i, v.z * i);}

inline IVec3 normalize(const IVec3& v, int iL)						{float fL = iL*invsqrt(float(v.x * v.x + v.y * v.y + v.z * v.z)); return IVec3(int(v.x*fL), int(v.y*fL), int(v.z*fL)); }
inline int   dot(const IVec3& v1, const IVec3& v2) 					{return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
inline IVec3 cross(const IVec3& v1, const IVec3& v2)				{return IVec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
inline float angle(const IVec3& v1, const IVec3& v2)				{return acos((v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) * invsqrt(float(v1.LengthSq()) * float(v2.LengthSq())));}

// ******************************************************************************** //
// Lerp, Abs, Min and Max have to be in global scope to override the default template
inline IVec3 min(const IVec3& v1, const IVec3& v2)					{return IVec3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z));}
inline IVec3 max(const IVec3& v1, const IVec3& v2)					{return IVec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));}
inline IVec3 lerp(const IVec3& v1, const IVec3& v2, const int i)	{return IVec3(v1.x+i*(v2.x-v1.x), v1.y+i*(v2.y-v1.y), v1.z+i*(v2.z-v1.z));}
inline IVec3 abs(const IVec3& v1)									{return IVec3(abs(v1.x), abs(v1.y), abs(v1.z));}



// ******************************************************************************** //
// Combined operators between integer and float vectors
inline Vec3 operator + (const Vec3& v1, const IVec3& v2)			{return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);}
inline Vec3 operator + (const IVec3& v1, const Vec3& v2)			{return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);}
inline Vec3 operator - (const Vec3& v1, const IVec3& v2)			{return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);}
inline Vec3 operator - (const IVec3& v1, const Vec3& v2)			{return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);}
inline Vec3 operator * (const Vec3& v1, const IVec3& v2)			{return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);}
inline Vec3 operator * (const IVec3& v1, const Vec3& v2)			{return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);}
inline Vec3 operator / (const Vec3& v1, const IVec3& v2)			{return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);}
inline Vec3 operator / (const IVec3& v1, const Vec3& v2)			{return Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);}


}; // namespace Math