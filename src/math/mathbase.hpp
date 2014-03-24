#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

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
	inline int floor( const float a )							{ int r=(int)a; return r - (int)((a<0)&&(a-r!=0.0f)); }
	/// \brief Round up to integer
	inline int ceil( const float a )							{ int r=int(a)+1; return r - (int)((a<0)&&(a-r!=0.0f)); }
	template<typename T, typename S> T lerp( const T& a, const T& b, const S& f )	{ return a+(b-a)*f; }
	inline float bilerp( float s00, float s10, float s01, float s11, float x, float y )
	{
		// Method 1: combination of linear interpolations
		/*float a = s00 + (s10 - s00) * x;
		float b = s01 + (s11 - s01) * x;
		return a + (b - a) * y;*/
		
		// Method2: precomputed factors (26% faster in Random::At(x,y,z)
		float f11 = x * y;
		float f01 = (1 - x) * y;
		float f10 = x * (1 - y);
		float f00 = (1 - x) * (1 - y);
		return s00*f00 + s01*f01 + s10*f10 + s11*f11;
	}
	inline float smooth( float f )					{ return f*f*f*(f*(f*6.0f-15.0f)+10.0f); }
	inline float smoothDerivative( float f )		{ return f*f*(f*(f-2.0f)+1.0f)*30.0f; }
	inline float saturate( float f )				{ return f < 0.0f ? 0.0f : ( f > 1.0f ? 1.0f : f ); }
	inline float invsqrt( float f )					{ float r = sqrt(f); return r==0.0f ? 1e30f : 1.0f/r; }
	/// \brief Return the next power of two which is greater than i
	inline int nextpow2( int i )
	{
		int o = i;
		i |= i >> 1;
		i |= i >> 2;
		i |= i >> 4;
		i |= i >> 8;
		i |= i >> 16;
		return i+1;
		// GEQ Was it a power of two before?
		//return (i+1 == o*2) ? o : (i+1);
	}

	/// \brief Logathim dualism rounded down
	inline int ld( int i )
	{
		if( i <= 0 ) return std::numeric_limits<int>::min();
		// Convert
		union { uint64_t i; double d; } t; // temp
		t.d = i;
		// Now extract the exponent
		return (t.i >> 52) - 0x3FF;
	}

	/// \details From http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
	/*inline int ld( int i )
	{
		unsigned int v;	         // 32-bit value to find the log2 of 
		register unsigned int r; // result of log2(v) will go here
		register unsigned int shift;

		r =     (v > 0xFFFF) << 4; v >>= r;
		shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
		shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
		shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
												r |= (v >> 1);
	}*/

	// ************************************************************************** //
	// Constants
	// ************************************************************************** //
	const float PI_2 = 1.570796327f;
	const float PI   = 3.141592654f;
	const float PIx2 = 6.283185307f;

	/// \brief Threshold for comparisons
	const float EPSILON = 0.000001f;
};