#include "quaternion.hpp"
#include "matrix.hpp"

namespace Math {

// ******************************************************************************** //
// Construction from matrix
Quaternion::Quaternion(const Mat4x4& _m)
{
	// Local copy for write access. The copy is without translation part.
	Mat3x3 M(_m(0,0), _m(0,1), _m(0,2),
			 _m(1,0), _m(1,1), _m(1,2),
			 _m(2,0), _m(2,1), _m(2,2));

	// Remove scaling part
	M.Column(0) = normalize(M.Column(0));
	M.Column(1) = normalize(M.Column(1));
	M.Column(2) = normalize(M.Column(2));

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
	float f2i = 2.0f * i;
	float f2j = 2.0f * j;
	float f2k = 2.0f * k;
	float f2ri = f2i * r;
	float f2rj = f2j * r;
	float f2rk = f2k * r;
	float f2ii = f2i * i;
	float f2ij = f2j * i;
	float f2ik = f2k * i;
	float f2jj = f2j * j;
	float f2jk = f2k * j;
	float f2kk = f2k * k;

	return Mat4x4(
		1.0f - ( f2jj + f2kk ), f2ij - f2rk, f2ik + f2rj, 0.0f,
		f2ij + f2rk, 1.0f - ( f2ii + f2kk ), f2jk - f2ri, 0.0f,
		f2ik - f2rj, f2jk + f2ri, 1.0f - ( f2ii + f2jj ), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}
// ******************************************************************************** //
Quaternion::operator Mat3x3 () const
{
	// Rotation composition from quaternion (remaining rest direct in matrix)
	// See http://de.wikipedia.org/wiki/Quaternion#Bezug_zu_orthogonalen_Matrizen for
	// details.
	// basically Mat 4x4 without padding
	float f2i = 2.0f * i;
	float f2j = 2.0f * j;
	float f2k = 2.0f * k;
	float f2ri = f2i * r;
	float f2rj = f2j * r;
	float f2rk = f2k * r;
	float f2ii = f2i * i;
	float f2ij = f2j * i;
	float f2ik = f2k * i;
	float f2jj = f2j * j;
	float f2jk = f2k * j;
	float f2kk = f2k * k;

	return Mat3x3(
		1.0f - (f2jj + f2kk), f2ij - f2rk, f2ik + f2rj,
		f2ij + f2rk, 1.0f - (f2ii + f2kk), f2jk - f2ri,
		f2ik - f2rj, f2jk + f2ri, 1.0f - (f2ii + f2jj)
		);
}
// ******************************************************************************** //
Quaternion::operator Vec3 () const
{
	//axis representation with |Axis|=angle
	//<=> tangential velocity is crossproduct with representation
	double sina = sqrt(i*i + j*j + k*k);
	float factor=2*float(atan2(sina,r)/sina) ;

	return Vec3(
		i*factor,
		j*factor,
		k*factor
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