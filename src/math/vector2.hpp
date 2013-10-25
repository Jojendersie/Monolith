#pragma once

#include "mathbase.hpp"

namespace Math {

/// \brief The 2D vector class
class Vec2
{
public:
	// Variables
	union
	{
		struct
		{
			float x;	// coordinates
			float y;
		};

		struct
		{
			float u;	// nomenclature as texture coordinates
			float v;
		};
	};

	// Constructors
	Vec2()																	{}
	Vec2(const Vec2& v) : x(v.x), y(v.y)									{}
	Vec2(const float f) : x(f), y(f)										{}
	Vec2(const float _x, const float _y) : x(_x), y(_y)						{}
	Vec2(const float* pfComponent) : x(pfComponent[0]), y(pfComponent[1])	{}

	// Casting-operators
	operator float* ()											{return (float*)(this);}
	operator const float* () const								{return (const float*)(this);}

	// Assignment operators
	Vec2& operator = (const Vec2& v)							{x = v.x; y = v.y; return *this;}
	Vec2& operator = (const float f)							{x = f; y = f; return *this;}
	Vec2& operator += (const Vec2& v)							{x += v.x; y += v.y; return *this;}
	Vec2& operator -= (const Vec2& v)							{x -= v.x; y -= v.y; return *this;}
	Vec2& operator *= (const Vec2& v)							{x *= v.x; y *= v.y; return *this;}
	Vec2& operator *= (const float f)							{x *= f; y *= f; return *this;}
	Vec2& operator /= (const Vec2& v)							{x /= v.x; y /= v.y; return *this;}
	Vec2& operator /= (float f)									{f = 1/f; x *= f; y *= f; return *this;}

	inline float& operator[](int i)								{ return *((float*)this + i); }

	// ******************************************************************************** //
	// Arithmetical operators
	inline Vec2 operator + (const Vec2& b) const				{return Vec2(x + b.x, y + b.y);}
	inline Vec2 operator - (const Vec2& b) const				{return Vec2(x - b.x, y - b.y);}
	inline Vec2 operator - () const								{return Vec2(-x, -y);}
	inline Vec2 operator * (const Vec2& b) const				{return Vec2(x * b.x, y * b.y);}
	inline Vec2 operator * (const float f) const				{return Vec2(x * f, y * f);}
	inline Vec2 operator / (const Vec2& b) const				{return Vec2(x / b.x, y / b.y);}
	inline Vec2 operator / (float f) const						{f = 1/f; return Vec2(x * f, y * f);}	// Multiplication is faster

	// ******************************************************************************** //
	// Comparison operators
	inline bool operator == (const Vec2& b) const				{if(x != b.x) return false; return y == b.y;}
	inline bool operator != (const Vec2& b) const				{if(x != b.x) return true; return y != b.y;}

	// ******************************************************************************** //
	// Vector2 functions
	// The non-static and non-const functions always change the calling object
	inline float		Length() const							{return sqrt(x * x + y * y);}
	inline float		LengthSq() const						{return x * x + y * y;}
	inline float		LengthInv() const						{return invsqrt(x * x + y * y);}
	inline const Vec2&	Normalize()								{*this *= invsqrt(x * x + y * y); return *this;}
	inline float		Dot(const Vec2& v) const				{return x * v.x + y * v.y;}
	inline float		Cross(const Vec2& v) const				{return x * v.y - y * v.x;}
	inline Vec2			Perpendicular() const					{return Vec2(-y, x);}			// Orthogonal vector
	static Vec2			RandomDirection();
	static Vec2			RandomPosition();
	Vec2				Rotate(const float _fAlpha) const;		// Rotate a point in 2D space
};

// ******************************************************************************** //
// The Operator, which cannot be inside the class
inline Vec2 operator * (const float f, const Vec2& v)			{return Vec2(v.x * f, v.y * f);}

inline Vec2	normalize(const Vec2& v)							{return v * invsqrt(v.x * v.x + v.y * v.y);}
inline float dot(const Vec2& v1, const Vec2& v2)				{return v1.x * v2.x + v1.y * v2.y;}
inline float cross(const Vec2& v1, const Vec2& v2)				{return v1.x * v2.y - v1.y * v2.x;}
inline float angle(const Vec2& v1, const Vec2& v2)				{return cos((v1.x * v2.x + v1.y * v2.y) * invsqrt((v1.x * v1.x + v1.y * v1.y) * (v2.x * v2.x + v2.y * v2.y)));}

// ******************************************************************************** //
// Lerp, Min, Max and Abs have to be in global scope to override the default template
inline Vec2	min(const Vec2& v1, const Vec2& v2)					{return Vec2(min(v1.x, v2.x), min(v1.y, v2.y));}
inline Vec2	max(const Vec2& v1, const Vec2& v2)					{return Vec2(max(v1.x, v2.x), max(v1.y, v2.y));}
inline Vec2	lerp(const Vec2& v1, const Vec2& v2, const float f)	{return Vec2(v1.x+f*(v2.x-v1.x), v1.y+f*(v2.y-v1.y));}
inline Vec2	abs(const Vec2& v1)									{return Vec2(abs(v1.x), abs(v1.y));}
/// \brief The spherical interpolation applies only to normal vectors
Vec2 slerp(const Vec2& v1, const Vec2& v2, const float t);

}; // namespace Math