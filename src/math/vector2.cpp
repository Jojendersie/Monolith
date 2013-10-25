#include "vector2.hpp"

namespace Math {
// The most operators and functions are implemented inline in the header

// ******************************************************************************** //
// Create a uniformly distributed random direction vector ( length == 1 ).
// No inline because of Rand-dependency (-> more includes)
/*Vec2 Vec2::RandomDirection()
{
	float fAngle = Rand()*f2Pi;
	return Vec2( cos(fAngle), sin(fAngle) );
}

// ******************************************************************************** //
// Create a random point uniformly distributed in the [-1,1]^3 cube.
Vec2 Vec2::RandomPosition()
{
	return Vec2(Rand()*2.0f-1.0f, Rand()*2.0f-1.0f);
}*/

// ******************************************************************************** //
// Rotate a point in 2D space (Cartesian coordinates)
Vec2 Vec2::Rotate(const float _fAlpha) const
{
	float fCos = cos(_fAlpha);
	float fSin = sin(_fAlpha);
	return Vec2(x*fCos - y*fSin,
				x*fSin + y*fCos);
}

// ******************************************************************************** //
// The spherical interpolation applies only to normal vectors
Vec2 slerp(const Vec2& v1, const Vec2& v2, const float t)
{
	float fOmega = acos( clamp(v1.Dot(v2), -1.0f, 1.0f) );
	//float fInvSin = 1.0f/Sin( fOmega );
	float f1 = sin( fOmega * (1.0f-t) );// * fInvSin;
	float f2 = sin( fOmega * t );// * fInvSin;
	return Vec2( v1.x*f1+v2.x*f2, v1.y*f1+v2.y*f2 ).Normalize();
}

};