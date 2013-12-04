#include "math.hpp"

namespace Math {

// ******************************************************************************** //
// Create a uniformly distributed random direction vector ( length == 1 ).
// No inline because of Rand-dependency (-> more includes)
/*Vec4 Vec4::RandomDirection()
{
	float x,y,z,w;
	float fLenSq;
	do {
		x = Rand()*2.0f-1.0f;
		y = Rand()*2.0f-1.0f;
		z = Rand()*2.0f-1.0f;
		w = Rand()*2.0f-1.0f;
		fLenSq = x*x + y*y + z*z + w*w;
	} while( fLenSq > 1.0f );
	fLenSq = 1.0f/sqrt(fLenSq);
	return Vec4( x*fLenSq, y*fLenSq, z*fLenSq, w*fLenSq );
}

// ******************************************************************************** //
// Create a random point uniformly distributed in the [-1,1]^3 cube.
Vec4 Vec4::RandomPosition()
{
	return Vec4(Rand()*2.0f-1.0f, Rand()*2.0f-1.0f, Rand()*2.0f-1.0f, Rand()*2.0f-1.0f);
}*/

// ******************************************************************************** //
// Crossproduct of 3 4D - vectors.
// This should be the same as the determinant with a fourth unit vector.
const Vec4& Vec4::Cross(const Vec4& v2, const Vec4& v3)
{
    x = y * (v2.z * v3.w - v2.w * v3.z) -
		z * (v2.y * v3.w - v2.w * v3.y) +
		w * (v2.y * v3.z - v2.z * v3.y),

	y =-(x * (v2.z * v3.w - v3.z * v2.w) -
		 z * (v2.x * v3.w - v3.x * v2.w) +
		 w * (v2.x * v3.z - v3.x * v2.z)),

	z = x * (v2.y * v3.w - v3.y * v2.w) -
		y * (v2.x * v3.w - v3.x * v2.w) +
		w * (v2.x * v3.y - v3.x * v2.y),

	w =-(x * (v2.y * v3.z - v3.y * v2.z) -
		 y * (v2.x * v3.z - v3.x * v2.z) +
		 z * (v2.x * v3.y - v3.x * v2.y));

	return *this;
}

Vec4 cross(const Vec4& v1, const Vec4& v2, const Vec4& v3)
{
    return Vec4(v1.y * (v2.z * v3.w - v2.w * v3.z) -
				v1.z * (v2.y * v3.w - v2.w * v3.y) +
				v1.w * (v2.y * v3.z - v2.z * v3.y),

			  -(v1.x * (v2.z * v3.w - v3.z * v2.w) -
				v1.z * (v2.x * v3.w - v3.x * v2.w) +
				v1.w * (v2.x * v3.z - v3.x * v2.z)),

				v1.x * (v2.y * v3.w - v3.y * v2.w) -
				v1.y * (v2.x * v3.w - v3.x * v2.w) +
				v1.w * (v2.x * v3.y - v3.x * v2.y),

			  -(v1.x * (v2.y * v3.z - v3.y * v2.z) -
				v1.y * (v2.x * v3.z - v3.x * v2.z) +
				v1.z * (v2.x * v3.y - v3.x * v2.y)) );
}

// ******************************************************************************** //
// The spherical interpolation applies only to normal vectors
Vec4 slerp(const Vec4& v1, const Vec4& v2, const float t)
{
	float fOmega = acos( clamp(v1.Dot(v2), -1.0f, 1.0f) );
	//float fInvSin = 1.0f/Sin( fOmega );
	float f1 = sin( fOmega * (1.0f-t) );// * fInvSin;
	float f2 = sin( fOmega * t );// * fInvSin;
	return Vec4( v1.x*f1+v2.x*f2, v1.y*f1+v2.y*f2, v1.z*f1+v2.z*f2, v1.w*f1+v2.w*f2 ).Normalize();
}


// ******************************************************************************** //
// Construction from matrix
Quaternion::Quaternion(const Mat4x4& _m)
{
	// TODO: Mat3x3
	// Local copy for write access. The copy is without translation part.
	Mat4x4 M(_m(0,0), _m(0,1), _m(0,2), 0.0f,
			 _m(1,0), _m(1,1), _m(1,2), 0.0f,
			 _m(2,0), _m(2,1), _m(2,2), 0.0f,
			 0.0f,   0.0f,   0.0f,   1.0f);

	// Remove scaling part
	((Vec3*)(&M(0,0)))->Normalize();
	((Vec3*)(&M(0,1)))->Normalize();
	((Vec3*)(&M(0,2)))->Normalize();

	// ignore de-orthogonalization

	// Build Quaternion from rotation matrix
	// Src: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	float fTrace = _m(0,0) + _m(1,1) + _m(2,2);
	if( fTrace > 0 )
	{
		float s = 0.5f / sqrtf( fTrace+ 1.0f );
		i = ( _m(2,1) - _m(1,2) ) * s;
		j = ( _m(0,2) - _m(2,0) ) * s;
		k = ( _m(1,0) - _m(0,1) ) * s;
		r = 0.25f / s;
	} else {
		if( _m(0,0) > _m(1,1) && _m(0,0) > _m(2,2) )
		{
			float s = 2.0f * sqrtf( 1.0f + _m(0,0) - _m(1,1) - _m(2,2) );
			i = 0.25f * s;
			j = ( _m(0,1) + _m(1,0) ) / s;
			k = ( _m(0,2) + _m(2,0) ) / s;
			r = ( _m(2,1) - _m(1,2) ) / s;
		} else if( _m(1,1) > _m(2,2) )
		{
			float s = 2.0f * sqrtf( 1.0f + _m(1,1) - _m(0,0) - _m(2,2) );
			i = ( _m(0,1) + _m(1,0) ) / s;
			j = 0.25f * s;
			k = ( _m(1,2) + _m(2,1) ) / s;
			r = ( _m(0,2) - _m(2,0) ) / s;
		} else {
			float s = 2.0f * sqrtf( 1.0f + _m(2,2) - _m(0,0) - _m(1,1) );
			i = ( _m(0,2) + _m(2,0) ) / s;
			j = ( _m(1,2) + _m(2,1) ) / s;
			k = 0.25f * s;
			r = ( _m(1,0) - _m(0,1) ) / s;
		}
	}
}

// ******************************************************************************** //
// From Euler angles
Quaternion::Quaternion( float _fYaw, float _fPitch, float _fRoll )
{
	Quaternion q( Mat4x4::Rotation( _fYaw, _fPitch, _fRoll ) );
	*this = q;
}

// ******************************************************************************** //
Quaternion::operator Mat4x4 () const
{
	// Rotation composition from quaternion (remaining rest direct in matrix)
	// See http://de.wikipedia.org/wiki/Quaternion#Bezug_zu_orthogonalen_Matrizen for
	// details.
	float f2i  = 2.0f * i;
	float f2j  = 2.0f * j;
	float f2k  = 2.0f * k;
    float f2ri = f2i  * r;
    float f2rj = f2j  * r;
    float f2rk = f2k  * r;
    float f2ii = f2i  * i;
    float f2ij = f2j  * i;
    float f2ik = f2k  * i;
    float f2jj = f2j  * j;
    float f2jk = f2k  * j;
    float f2kk = f2k  * k;

	return Mat4x4(
		1.0f - ( f2jj + f2kk ), f2ij - f2rk,            f2ik + f2rj,            0.0f,
		f2ij + f2rk,            1.0f - ( f2ii + f2kk ), f2jk - f2ri,            0.0f,
		f2ik - f2rj,            f2jk + f2ri,            1.0f - ( f2ii + f2jj ), 0.0f,
		0.0f,                   0.0f,                   0.0f,                   1.0f
	);
}

// ******************************************************************************** //
// The spherical interpolation for union quaternions
Quaternion slerp(const Quaternion& a, const Quaternion& b, const float t)
{
	float fOmega = acos( clamp(a.Dot(b), -1.0f, 1.0f) );
	float f1 = sin( fOmega * (1.0f-t) );
	float f2 = sin( fOmega * t );
	return Quaternion( a.r*f1+b.r*f2, a.i*f1+b.i*f2, a.j*f1+b.j*f2, a.k*f1+b.k*f2 );
}

};