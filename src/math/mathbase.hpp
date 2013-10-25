#pragma once

#include <cmath>
#include <cstdint>

namespace Math {

	// ************************************************************************** //
	// Helper functions
	// ************************************************************************** //
	inline float sgn( float f )									{ return f<0.0f ? -1.0f : 1.0f; }
	template<typename T> const T abs(const T& a)				{ return a < 0 ? -a : a; }
	template<typename T> const T min(const T& a, const T& b)	{ return a<b ? a : b; }
	template<typename T> const T max(const T& a, const T& b)	{ return a>b ? a : b; }
	template<typename T> const T clamp(const T& x, const T& _min, const T& _max)	{ return x < _min ? _min : ( x > _max ? _max : x ); }
	template<typename T> const T sqr(const T& a)				{ return a*a; }
	/// \brief Round down to integer
	inline int floor( const float a )				{ int r=(int)a; return r - (int)((a<0)&&(a-r!=0.0f)); }
	/// \brief Round up to integer
	inline int ceil( const float a )				{ int r=int(a)+1; return r - (int)((a<0)&&(a-r!=0.0f)); }
	inline float lerp( float a, float b, float f )	{ return a+(b-a)*f; }
	inline float smooth( float f )					{ return f*f*f*(f*(f*6.0f-15.0f)+10.0f); }
	inline float smoothDerivative( float f )		{ return f*f*(f*(f-2.0f)+1.0f)*30.0f; }
	inline float saturate( float f )				{ return f < 0.0f ? 0.0f : ( f > 1.0f ? 1.0f : f ); }
	inline float invsqrt( float f )					{ float r = sqrt(f); return r==0.0f ? 1e30f : 1.0f/r; }

	// ************************************************************************** //
	// Constants
	// ************************************************************************** //
	const float PI_2 = 1.570796327f;
	const float PI   = 3.141592654f;
	const float PIx2 = 6.283185307f;

	/// \brief Threshold for comparisons
	const float EPSILON = 0.000001f;
};