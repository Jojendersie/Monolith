#include "vector3.hpp"
#include "matrix.hpp"

namespace Math {

	// ******************************************************************************** //
	Vec3::Vec3(const IVec3& v) : x(float(v.x)), y(float(v.y)), z(float(v.z))												{}

// ******************************************************************************** //
// Create a uniformly distributed random direction vector ( length == 1 ).
// No inline because of Rand-dependency (-> more includes)
/*Vec3 Vec3::RandomDirection()
{
	float fAngle1 = Rand()*f2Pi;
	float fAngle2 = Rand()*f2Pi;
	float fSinA1 = sin(fAngle1);
	return Vec3( fSinA1 * cos(fAngle2),
				 cos(fAngle1),
				 fSinA1 * sin(fAngle2)
		);
}

// ******************************************************************************** //
// Create a random point uniformly distributed in the [-1,1]^3 cube.
Vec3 Vec3::RandomPosition()
{
	return Vec3(Rand()*2.0f-1.0f, Rand()*2.0f-1.0f, Rand()*2.0f-1.0f);
}

// ******************************************************************************** //
IVec3 OrE::Math::IVec3::Random()
{
	return IVec3( int(RandU()), int(RandU()), int(RandU()) ).Normalize( 1000 );
}*/


// ******************************************************************************** //
// Transform a 3D directional vector - faster since only 3x3 matrix is applied
Vec3 Vec3::TransformDirection(const Matrix& m) const
{
	// multiply 3x3 matrix with vector - no translation
	return Vec3(x * m.m11 + y * m.m21 + z * m.m31,
				x * m.m12 + y * m.m22 + z * m.m32,
				x * m.m13 + y * m.m23 + z * m.m33);

	// Calculation of the fourth component not necessary for directional vectors
	// (vec3,0).

	return *this;
}

// ******************************************************************************** //
// Rotate a vector around some axis. This transformation uses quaternion
// math. Use this if you want to rotate only one vector. For the rotation
// of multiple vectors a matrix is more efficient!
Vec3 Vec3::Rotate(const Vec3& vAxis, float fAngle) const
{
	// Calculate quaternion Errors in Code??
	/*double x = cos(fAngle*0.5);
	double i = sin(fAngle*0.5);
	double j = vAxis.y * i;
	double k = vAxis.z * i;
	i *= vAxis.x;

	const double a00 = x * x;
	const double a01 = x * i;
	const double a02 = x * j;
	const double a03 = x * k;
	const double a11 = i * i;
	const double a12 = i * j;
	const double a13 = i * k;
	const double a22 = j * j;
	const double a23 = j * k;
	const double a33 = k * k;

	return Vec3(float(x * (a00 + a11 - a22 - a33) + 2.0 * ((a12 - a03) * y + (a13 + a02) * z)),
				float(y * (a00 - a11 + a22 - a33) + 2.0 * ((a12 + a03) * x + (a23 - a01) * z)),
				float(z * (a00 - a11 - a22 + a33) + 2.0 * ((a13 - a02) * x + (a23 + a01) * y)));*/

	// Direct Matrix variant (faster and not wrong like the one above)
	const double dSin = sin(double(-fAngle));
	const double dCos = cos(double(-fAngle));
	const double dOneMinusCos = 1.0 - dCos;
	const double dAxSin = vAxis.x * dSin;
	const double dAySin = vAxis.y * dSin;
	const double dAzSin = vAxis.z * dSin;

	// Create the matrix:
	const double c12 = (vAxis.x * vAxis.y) * dOneMinusCos;
	const double c13 = (vAxis.x * vAxis.z) * dOneMinusCos;
	const double c23 = (vAxis.y * vAxis.z) * dOneMinusCos;
	double m11 = (vAxis.x * vAxis.x) * dOneMinusCos + dCos;	double m12 = c12 - dAzSin;								double m13 = c13 + dAySin;
	double m21 = c12 + dAzSin;								double m22 = (vAxis.y * vAxis.y) * dOneMinusCos + dCos;	double m23 = c23 - dAxSin;
	double m31 = c13 - dAySin;								double m32 = c23 + dAxSin;								double m33 = (vAxis.z * vAxis.z) * dOneMinusCos + dCos;

	return Vec3(float(x * m11 + y * m21 + z * m31),
				float(x * m12 + y * m22 + z * m32),
				float(x * m13 + y * m23 + z * m33));
/*	return Vec3(float(x * m11 + y * m12 + z * m13),
				float(x * m21 + y * m22 + z * m23),
				float(x * m31 + y * m32 + z * m33));*/
}

/*Vec3 OrE::Math::Vec3::Rotate(const Vec3& v, const Vec3& vAxis, float fAngle)
{
	// Calculate quaternion
	double x = cos(fAngle*0.5);
	double i = sin(fAngle*0.5);
	double j = vAxis.y * i;
	double k = vAxis.z * i;
	i *= vAxis.x;

	const double a00 = x * x;
	const double a01 = x * i;
	const double a02 = x * j;
	const double a03 = x * k;
	const double a11 = i * i;
	const double a12 = i * j;
	const double a13 = i * k;
	const double a22 = j * j;
	const double a23 = j * k;
	const double a33 = k * k;

	return Vec3(float(v.x * (a00 + a11 - a22 - a33) + 2.0 * ((a12 - a03) * v.y + (a13 + a02) * v.z)),
				float(v.y * (a00 - a11 + a22 - a33) + 2.0 * ((a12 + a03) * v.x + (a23 - a01) * v.z)),
				float(v.z * (a00 - a11 - a22 + a33) + 2.0 * ((a13 - a02) * v.x + (a23 + a01) * v.y)));
}*/

// ******************************************************************************** //
// The spherical interpolation applies only to normal vectors
Vec3 slerp(const Vec3& v1, const Vec3& v2, const float t)
{
	float fOmega = acos( clamp(v1.Dot(v2), -1.0f, 1.0f) );
	float f1 = sin( fOmega * (1.0f-t) );
	float f2 = sin( fOmega * t );
	return Vec3( v1.x*f1+v2.x*f2, v1.y*f1+v2.y*f2, v1.z*f1+v2.z*f2 ).Normalize();
}

};