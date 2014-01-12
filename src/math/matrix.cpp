#include "mathbase.hpp"
#include "matrix.hpp"
#include "plane.hpp"
#include "quaternion.hpp"
#include <cstring>

namespace Math {

	// Setup a translation matrix
	Mat4x4 Mat4x4::Translation(float _x, float _y, float _z)
	{
		return Mat4x4(1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f, 0.0f,
					  _x,   _y,   _z,   1.0f);
	}

	// Setup a translation matrix
	Mat4x4 Mat4x4::Translation(const Vec3& _vector)
	{
		return Mat4x4(1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f, 0.0f,
					  _vector[0],  _vector[1],  _vector[2],  1.0f);
	}

	// Setup a translation matrix
	Mat4x4 Mat4x4::Scaling(const Vec3& _scale)
	{
		return Mat4x4(_scale[0],  0.0f, 0.0f, 0.0f,
					  0.0f, _scale[1],  0.0f, 0.0f,
					  0.0f, 0.0f, _scale[2],  0.0f,
					  0.0f, 0.0f, 0.0f,	1.0f);
	}

	// Setup a translation matrix
	Mat4x4 Mat4x4::Scaling(float _uniformScale)
	{
		return Mat4x4(_uniformScale,  0.0f, 0.0f, 0.0f,
					  0.0f, _uniformScale,  0.0f, 0.0f,
					  0.0f, 0.0f, _uniformScale,  0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Setup a translation matrix
	Mat4x4 Mat4x4::Scaling(float _sx, float _sy, float _sz)
	{
		return Mat4x4(_sx,  0.0f, 0.0f, 0.0f,
					  0.0f, _sy,  0.0f, 0.0f,
					  0.0f, 0.0f, _sz,  0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Calculate rotation around the x axis
	Mat4x4 Mat4x4::RotationX(const float _a)
	{
		Mat4x4 result(0.0f);
		result(0,0) = result(3,3) = 1.0f;
		result(1,1) = result(2,2) = cos(_a);
		result(1,2) = sin(_a);
		result(2,1) = -result(1,2);
		return result;
	}

	// Calculate rotation around the y axis
	Mat4x4 Mat4x4::RotationY(const float _a)
	{
		Mat4x4 result(0.0f);
		result(1,1) = result(3,3) = 1.0f;
		result(0,0) = result(2,2) = cos(_a);
		result(2,0) = sin(_a);
		result(0,2) = -result(2,0);
		return result;
	}

	// Calculate rotation around the z axis
	Mat4x4 Mat4x4::RotationZ(const float _a)
	{
		Mat4x4 result;
		result(2,2) = result(3,3) = 1.0f;
		result(0,0) = result(1,1) = cos(_a);
		result(0,1) = sin(_a);
		result(1,0) = -result(0,1);
		return result;
	}

	// Rotate around all three axis. This is the same as RotationZ(_c) * RotationX(_b) * RotationY(_a)
	Mat4x4 Mat4x4::Rotation(float _a, float _b, float _c)
	{
		float sinX = sin(_a), sinY = sin(_b), sinZ = sin(_c);
		float cosX = cos(_a), cosY = cos(_b), cosZ = cos(_c);
		float sinXY = sinX*sinY;
		float cosYZ = cosY*cosZ;
		float cosYsinZ = cosY*sinZ;
		return Mat4x4(cosYZ-sinXY*sinZ,		cosX*sinZ,	sinY*cosZ+sinX*cosYsinZ,	0.0f,
					 -cosYsinZ-sinXY*cosZ,	cosX*cosZ,	sinX*cosYZ-sinY*sinZ,		0.0f,
					 -cosX*sinY,			-sinX,		cosX*cosY,					0.0f,
					  0.0f,					0.0f,		0.0f,						1.0f);
	}

	// Rotate around an arbitrary axis
	Mat4x4 Mat4x4::Rotation(const Vec3& _axis, float _angle)
	{
		// Calculate as much as possible without redundant calculations
		const double fSin = sin(double(-_angle));
		const double fCos = cos(double(-_angle));
		const double fOneMinusCos = 1.0 - fCos;
		const double AxSin = _axis[0] * fSin;
		const double AySin = _axis[1] * fSin;
		const double AzSin = _axis[2] * fSin;

		// Create matrix and close up the calculation
		return Mat4x4(float((_axis[0] * _axis[0]) * fOneMinusCos + fCos),
			float((_axis[0] * _axis[1]) * fOneMinusCos - (AzSin)),
			float((_axis[0] * _axis[2]) * fOneMinusCos + (AySin)),
			0.0f,
			float((_axis[1] * _axis[0]) * fOneMinusCos + (AzSin)),
			float((_axis[1] * _axis[1]) * fOneMinusCos + fCos),
			float((_axis[1] * _axis[2]) * fOneMinusCos - (AxSin)),
			0.0f,
			float((_axis[2] * _axis[0]) * fOneMinusCos - (AySin)),
			float((_axis[2] * _axis[1]) * fOneMinusCos + (AxSin)),
			float((_axis[2] * _axis[2]) * fOneMinusCos + fCos),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f);
	}

	// Matrix from quaternion
	Mat4x4 Mat4x4::Rotation(const Quaternion& _r)
	{
		return Mat4x4(1.0f-2.0f*_r.j*_r.j-2.0f*_r.k*_r.k, 2.0f*_r.i*_r.j-2.0f*_r.k*_r.r,      2.0f*_r.i*_r.k+2.0f*_r.j*_r.r,      0.0f,
					  2.0f*_r.i*_r.j+2.0f*_r.k*_r.r,      1.0f-2.0f*_r.i*_r.i-2.0f*_r.k*_r.k, 2.0f*_r.j*_r.k-2.0f*_r.i*_r.r,      0.0f,
					  2.0f*_r.i*_r.k-2.0f*_r.j*_r.r,      2.0f*_r.j*_r.k+2.0f*_r.i*_r.r,      1.0f-2.0f*_r.i*_r.i-2.0f*_r.j*_r.j, 0.0f,
					  0.0f,                               0.0f,                               0.0f,                               1.0f);
	}

	// Setup an axis matrix - a vector base
	Mat4x4 Mat4x4::Axis(const Vec3& _xAxis, const Vec3& _yAxis, const Vec3& _zAxis)
	{
		return Mat4x4(_xAxis[0], _xAxis[1], _xAxis[2], 0.0f,
					  _yAxis[0], _yAxis[1], _yAxis[2], 0.0f,
					  _zAxis[0], _zAxis[1], _zAxis[2], 0.0f,
					  0.0f,     0.0f,     0.0f,     1.0f);
	}

	// OpenGL perspective projection matrix
	Mat4x4 Mat4x4::Projection(float _FOV, float _aspect, float _near, float _far)
	{
		//const float far = _far * 0.5f;
		const float s = 1.0f / tan(_FOV * 0.5f);
		const float frustumLengthInv = 1.0f / (_far - _near);

		return Mat4x4(s / _aspect,	0.0f,  0.0f,							   0.0f,
					  0.0f,			s,	   0.0f,							   0.0f,
					  0.0f,			0.0f, (_near + _far)*frustumLengthInv,	   1.0f,
					  0.0f,			0.0f, -(2.0f*_near*_far)*frustumLengthInv, 0.0f);
	}

	// OpenGL orthogonal projection matrix
	Mat4x4 Mat4x4::ParallelProjection(float _width, float _heigh, float _near, float _far)
	{
		const float s = 2.0f*_near;
		const float Q = _far / (_far - _near);

		return Mat4x4(s / _width,	0.0f,		0.0f,		0.0f,
					  0.0f,			s / _heigh,	0.0f,		0.0f,
					  0.0f,			0.0f,		Q,			1.0f,
					  0.0f,			0.0f,		-Q * _near,	0.0f);
	}

	// OpenGL orthogonal projection matrix
	Mat4x4 Mat4x4::ParallelProjection(float _left, float _right, float _bottom, float _top, float _near, float _far)
	{
		const float	r_l = 1.0f/(_right - _left);
		const float t_b = 1.0f/(_top - _bottom);
		const float f_n = 1.0f/(_near - _far);
		return Mat4x4(2.0f*r_l,	0.0f,		0.0f,		0.0f,
					  0.0f,		2.0f*t_b,	0.0f,		0.0f,
					  0.0f,		0.0f,		2.0f*f_n,	0.0f,
					  -(_left+_right)*r_l,	-(_bottom+_top)*t_b,	-(_far+_near)*f_n,	1.0f);
	}

	// Calculate camera matrix.
	Mat4x4 Mat4x4::Camera(const Vec3& _position, const Vec3& _lookAt, const Vec3& _up)
	{
		// Determine the z axis of camera coordinate system
		Vec3 zAxis(normalize(_lookAt - _position));

		// The c axis is the cross product from y and z axis
		Vec3 xAxis(normalize(cross(_up, zAxis)));

		// calculate an orthogonal y axis
		Vec3 yAxis(normalize(cross(zAxis, xAxis)));

		// Create rotation matrix simply from local coordinate system and move
		return Mat4x4::Translation(-_position) *
			Mat4x4(xAxis[0], yAxis[0], zAxis[0], 0.0f,
				   xAxis[1], yAxis[1], zAxis[1], 0.0f,
				   xAxis[2], yAxis[2], zAxis[2], 0.0f,
				   0.0f,    0.0f,    0.0f,    1.0f);
	}

	// Calculate camera matrix
	Mat4x4 Mat4x4::Camera(const Vec3& _position, const Vec3& _direction, const Vec3& _up, const Vec3& _bidir)
	{
		// Easy way - all axis already given
		return Mat4x4::Translation(-_position) *
			Mat4x4(_bidir[0], _up[0], _direction[0], 0.0f,
				   _bidir[1], _up[1], _direction[1], 0.0f,
				   _bidir[2], _up[2], _direction[2], 0.0f,
				   0.0f,     0.0f,  0.0f,         1.0f);
	}

	// Calculate a mirror matrix for given plane
	Mat4x4 Mat4x4::Mirror(const Plane& p)
	{
		return Mat4x4(-2.0f*p.a*p.a + 1.0f,	-2.0f*p.b*p.a,		-2.0f*p.c*p.a,		  0.0f,
					  -2.0f*p.a*p.b,		-2.0f*p.b + 1.0f,	-2.0f*p.c*p.b,		  0.0f,
					  -2.0f*p.a*p.c,		-2.0f*p.b*p.c,		-2.0f*p.c*p.c + 1.0f, 0.0f,
					  -2.0f*p.a*p.d,		-2.0f*p.b*p.d,		-2.0f*p.c*p.d,		  1.0f);
	}

	// Creates an orthogonal base for a direction vector
	Mat4x4 Mat4x4::Orthonormal(const Vec3& _normal)
	{
		// Calculate a second orthogonal vector with scalar product
		Vec3 v2 = (_normal[0]==1.0f)?Vec3(0.0f, 1.0f, 0.0f): normalize((_normal[1] != 0.0f)? Vec3(1.0f, -_normal[0]/_normal[1], 0.0f) : Vec3(1.0f, 0.0f, -_normal[0]/_normal[2]));
		// 3. Vector is simple the cross product
		Vec3 v3 = cross(_normal, v2);
		return Mat4x4(v2[0],		 v2[1],		v2[2],		0.0f,
					  v3[0],		 v3[1],		v3[2],		0.0f,
					  _normal[0], _normal[1],	_normal[2],	0.0f,
					  0.0f,		 0.0f,		0.0f,		1.0f);
	}

	// Transform a position vector.
	Vec3 Mat4x4::Transform(const Vec3& _v) const
	{
		return Vec3(_v[0]*(*this)(0,0)+_v[1]*(*this)(1,0)+_v[2]*(*this)(2,0)+(*this)(3,0),
					_v[0]*(*this)(0,1)+_v[1]*(*this)(1,1)+_v[2]*(*this)(2,1)+(*this)(3,1),
					_v[0]*(*this)(0,2)+_v[1]*(*this)(1,2)+_v[2]*(*this)(2,2)+(*this)(3,2));
	}

	// Transform a direction vector.
	Vec3 Mat4x4::TransformDirection(const Vec3& _v) const
	{
		return Vec3(_v[0]*(*this)(0,0)+_v[1]*(*this)(1,0)+_v[2]*(*this)(2,0),
					_v[0]*(*this)(0,1)+_v[1]*(*this)(1,1)+_v[2]*(*this)(2,1),
					_v[0]*(*this)(0,2)+_v[1]*(*this)(1,2)+_v[2]*(*this)(2,2));
	}

	// Transform a position vector and do division through the w part.
	Vec3 Mat4x4::TransformPerspective(const Vec3& _v) const
	{
		float wInf = 1.0f / (_v[0]*(*this)(0,3)+_v[1]*(*this)(1,3)+_v[2]*(*this)(2,3)+(*this)(3,3));
		return Vec3(_v[0]*(*this)(0,0)+_v[1]*(*this)(1,0)+_v[2]*(*this)(2,0)+(*this)(3,0),
					_v[0]*(*this)(0,1)+_v[1]*(*this)(1,1)+_v[2]*(*this)(2,1)+(*this)(3,1),
					_v[0]*(*this)(0,2)+_v[1]*(*this)(1,2)+_v[2]*(*this)(2,2)+(*this)(3,2)) * wInf;

	}


	Vec4 cross(const Vec4& v1, const Vec4& v2, const Vec4& v3)
	{
		return Vec4(v1[1] * (v2[2] * v3[3] - v2[3] * v3[2]) -
			v1[2] * (v2[1] * v3[3] - v2[3] * v3[1]) +
			v1[3] * (v2[1] * v3[2] - v2[2] * v3[1]),

			-(v1[0] * (v2[2] * v3[3] - v3[2] * v2[3]) -
			v1[2] * (v2[0] * v3[3] - v3[0] * v2[3]) +
			v1[3] * (v2[0] * v3[2] - v3[0] * v2[2])),

			v1[0] * (v2[1] * v3[3] - v3[1] * v2[3]) -
			v1[1] * (v2[0] * v3[3] - v3[0] * v2[3]) +
			v1[3] * (v2[0] * v3[1] - v3[0] * v2[1]),

			-(v1[0] * (v2[1] * v3[2] - v3[1] * v2[2]) -
			v1[1] * (v2[0] * v3[2] - v3[0] * v2[2]) +
			v1[2] * (v2[0] * v3[1] - v3[0] * v2[1])) );
	}

	// Compute the inverse with determinant method
	Mat4x4 Mat4x4::Inverse() const
	{
		Mat4x4 result;
		Vec4 v, vec[3];
		float det;

		det = Det();

		if(!det)
			return Identity();

		// Swap sign in each pass
		float signDet = 1.0f/det;

		for(int i=0; i<4; i++)
		{
			// Calculate determinant as cross product
			v = cross(*(Vec4*)&m_data[0<i?0:4], *(Vec4*)&m_data[1<i?4:8], *(Vec4*)&m_data[2<i?8:12]);

			// Sign * Determinant_i / Determinante
			result(i,0) = signDet * v[0];
			result(i,1) = signDet * v[1];
			result(i,2) = signDet * v[2];
			result(i,3) = signDet * v[3];
			signDet *= -1.0f;
		}

		return result;
	}

	// Calculate the determinant of the whole 4x4 matrix with Laplace's formula
	float Mat4x4::Det() const {
		// Determinant of upper left 3x3-sub matrix
		// (cross out 4. row and 4. column)
		float detA44 = + (*this)(0,0) * ((*this)(1,1) * (*this)(2,2) - (*this)(1,2) * (*this)(2,1))
			- (*this)(0,1) * ((*this)(1,0) * (*this)(2,2) - (*this)(1,2) * (*this)(2,0))
			+ (*this)(0,2) * ((*this)(1,0) * (*this)(2,1) - (*this)(1,1) * (*this)(2,0));
		// (cross out 4. row and 3. column)
		float detA43 = + (*this)(0,0) * ((*this)(1,1) * (*this)(2,3) - (*this)(1,3) * (*this)(2,1))
			- (*this)(0,1) * ((*this)(1,0) * (*this)(2,3) - (*this)(1,3) * (*this)(2,0))
			+ (*this)(0,3) * ((*this)(1,0) * (*this)(2,1) - (*this)(1,1) * (*this)(2,0));
		// (cross out 4. row and 2. column)
		float detA42 = + (*this)(0,0) * ((*this)(1,2) * (*this)(2,3) - (*this)(1,3) * (*this)(2,2))
			- (*this)(0,2) * ((*this)(1,0) * (*this)(2,3) - (*this)(1,3) * (*this)(2,0))
			+ (*this)(0,3) * ((*this)(1,0) * (*this)(2,2) - (*this)(1,2) * (*this)(2,0));
		// (cross out 4. row and 1. column)
		float detA41 = + (*this)(0,1) * ((*this)(1,2) * (*this)(2,3) - (*this)(1,3) * (*this)(2,2))
			- (*this)(0,2) * ((*this)(1,1) * (*this)(2,3) - (*this)(1,3) * (*this)(2,1))
			+ (*this)(0,3) * ((*this)(1,1) * (*this)(2,2) - (*this)(1,2) * (*this)(2,1));

		// Sum determinants from sub matrices
		return - (*this)(3,0) * detA41
			+ (*this)(3,1) * detA42
			- (*this)(3,2) * detA43
			+ (*this)(3,3) * detA44;
	}

	
	// ********************************************************************* //
	// Computes (_v,1) * _A which is the same as Transform().
	Vec3 operator * (const Vec3& _v, const Mat4x4& _A)
	{
		return Vec3(_v[0]*_A(0,0)+_v[1]*_A(1,0)+_v[2]*_A(2,0)+_A(3,0),
					_v[0]*_A(0,1)+_v[1]*_A(1,1)+_v[2]*_A(2,1)+_A(3,1),
					_v[0]*_A(0,2)+_v[1]*_A(1,2)+_v[2]*_A(2,2)+_A(3,2));
	}

	// ********************************************************************* //
	// Computes _A * (_v,1) which is the same as Transpose().Transform().
	Vec3 operator * (const Mat4x4& _A, const Vec3& _v)
	{
		return Vec3(_v[0]*_A(0,0)+_v[1]*_A(0,1)+_v[2]*_A(0,2)+_A(0,3),
					_v[0]*_A(1,0)+_v[1]*_A(1,1)+_v[2]*_A(1,2)+_A(1,3),
					_v[0]*_A(2,0)+_v[1]*_A(2,1)+_v[2]*_A(2,2)+_A(2,3));
	}

	// ********************************************************************* //
	// Computes _v * _A.
	Vec4 operator * (const Vec4& _v, const Mat4x4& _A)
	{
		return Vec4(_v[0]*_A(0,0)+_v[1]*_A(1,0)+_v[2]*_A(2,0)+_v[3]*_A(3,0),
					_v[0]*_A(0,1)+_v[1]*_A(1,1)+_v[2]*_A(2,1)+_v[3]*_A(3,1),
					_v[0]*_A(0,2)+_v[1]*_A(1,2)+_v[2]*_A(2,2)+_v[3]*_A(3,2),
					_v[0]*_A(0,3)+_v[1]*_A(1,3)+_v[2]*_A(2,3)+_v[3]*_A(3,3));
	}

	// ********************************************************************* //
	// Computes _A * _v.
	Vec4 operator * (const Mat4x4& _A, const Vec4& _v)
	{
		return Vec4(_v[0]*_A(0,0)+_v[1]*_A(0,1)+_v[2]*_A(0,2)+_v[3]*_A(0,3),
					_v[0]*_A(1,0)+_v[1]*_A(1,1)+_v[2]*_A(1,2)+_v[3]*_A(1,3),
					_v[0]*_A(2,0)+_v[1]*_A(2,1)+_v[2]*_A(2,2)+_v[3]*_A(2,3),
					_v[0]*_A(3,0)+_v[1]*_A(3,1)+_v[2]*_A(3,2)+_v[3]*_A(3,3));
	}



// ******************************************************************************** //
// Solve the equation system Ax=v with Gauß-Jordan method.
// The return value is true iff an unique solution exists. The solution is saved
// in _pV_X
/*bool MatrixSolveEquation(Matrix _A, Vec4& _vX)
{
	for(unsigned i=0;i<4;++i)
	{
		// make sure that the pivot element != 0
		if(_A.m[i][i]==0.0f)
		{
			// Problem: find a valid row and swap with the current one.
			// The row is valid if the currently first element is != 0. Additionally
			// the element in the current row has to be !=0 too.
			unsigned j=i+1;
			for(;j<4;++j)
			{
				if(_A.m[i][j]!=0.0f && _A.m[j][i]!=0)
				{
					// found row -> swap
					float fSwap0 = _A.m[i][0]; _A.m[i][0] = _A.m[j][0]; _A.m[j][0] = fSwap0;
					float fSwap1 = _A.m[i][1]; _A.m[i][1] = _A.m[j][1]; _A.m[j][1] = fSwap1;
					float fSwap2 = _A.m[i][2]; _A.m[i][2] = _A.m[j][2]; _A.m[j][2] = fSwap2;
					float fSwap3 = _A.m[i][3]; _A.m[i][3] = _A.m[j][3]; _A.m[j][3] = fSwap3;
					float fSwap = _vX[i]; _vX[i] = _vX[j]; _vX[j] = fSwap;
					// unnecessary to check more lines
					break;
				}
			}
			// If there is no such row the system cannot be solved unique.
			// Break and do not calculate more.
			if(j>=4) return false;
		}

		// The pivot element is != 0 => correct division of the row
		float fInvPivot = 1.0f/_A.m[i][i];
		// In the first column there is the pivot or a 0 => leave out.
		_A.m[i][1] *= fInvPivot;
		_A.m[i][2] *= fInvPivot;
		_A.m[i][3] *= fInvPivot;
		_vX[i] *= fInvPivot;
		_A.m[i][i] = 1.0f;

		// Subtract new row from all other rows
		for(unsigned j=0;j<4;++j)
			if(i!=j)
			{
				float fFactor = _A.m[j][i];
				// Bench: for(uint32 k=i+1;k<4;++k) _A.m[j,k] -= fFactor*_A.m[i,k];
				_A.m[j][1] -= fFactor*_A.m[i][1];
				_A.m[j][2] -= fFactor*_A.m[i][2];
				_A.m[j][3] -= fFactor*_A.m[i][3];
				_vX[j] -= fFactor*(_vX[i]);
				_A.m[j][i] = 0.0f;
			}
	}
	return true;
}*/




	// ********************************************************************* //
	// Setup a translation matrix
	Mat3x3 Mat3x3::Scaling( const Vec3& _scale )
	{
		return Mat3x3(_scale[0],  0.0f, 0.0f,
					  0.0f, _scale[1],  0.0f,
					  0.0f, 0.0f, _scale[2]);
	}

	// Setup a translation matrix
	Mat3x3 Mat3x3::Scaling(float _uniformScale)
	{
		return Mat3x3(_uniformScale,  0.0f, 0.0f,
					  0.0f, _uniformScale,  0.0f,
					  0.0f, 0.0f, _uniformScale);
	}

	// Setup a translation matrix
	Mat3x3 Mat3x3::Scaling(float _sx, float _sy, float _sz)
	{
		return Mat3x3(_sx,  0.0f, 0.0f,
					  0.0f, _sy,  0.0f,
					  0.0f, 0.0f, _sz);
	}

	// Calculate rotation around the x axis
	Mat3x3 Mat3x3::RotationX(const float _a)
	{
		Mat3x3 result(0.0f);
		result(0,0) = result(3,3) = 1.0f;
		result(1,1) = result(2,2) = cos(_a);
		result(1,2) = sin(_a);
		result(2,1) = -result(1,2);
		return result;
	}

	// Calculate rotation around the y axis
	Mat3x3 Mat3x3::RotationY(const float _a)
	{
		Mat3x3 result(0.0f);
		result(1,1) = result(3,3) = 1.0f;
		result(0,0) = result(2,2) = cos(_a);
		result(2,0) = sin(_a);
		result(0,2) = -result(2,0);
		return result;
	}

	// Calculate rotation around the z axis
	Mat3x3 Mat3x3::RotationZ(const float _a)
	{
		Mat3x3 result;
		result(2,2) = result(3,3) = 1.0f;
		result(0,0) = result(1,1) = cos(_a);
		result(0,1) = sin(_a);
		result(1,0) = -result(0,1);
		return result;
	}

	// Rotate around all three axis. This is the same as RotationZ(_c) * RotationX(_b) * RotationY(_a)
	Mat3x3 Mat3x3::Rotation(float _a, float _b, float _c)
	{
		float sinX = sin(_a), sinY = sin(_b), sinZ = sin(_c);
		float cosX = cos(_a), cosY = cos(_b), cosZ = cos(_c);
		float sinXY = sinX*sinY;
		float cosYZ = cosY*cosZ;
		float cosYsinZ = cosY*sinZ;
		return Mat3x3(cosYZ-sinXY*sinZ,		cosX*sinZ,	sinY*cosZ+sinX*cosYsinZ,
					  -cosYsinZ-sinXY*cosZ,	cosX*cosZ,	sinX*cosYZ-sinY*sinZ,
					  -cosX*sinY,			-sinX,		cosX*cosY);
	}

	// Rotate around an arbitrary axis
	Mat3x3 Mat3x3::Rotation(const Vec3& _axis, float _angle)
	{
		// Calculate as much as possible without redundant calculations
		const double fSin = sin(double(-_angle));
		const double fCos = cos(double(-_angle));
		const double fOneMinusCos = 1.0 - fCos;
		const double AxSin = _axis[0] * fSin;
		const double AySin = _axis[1] * fSin;
		const double AzSin = _axis[2] * fSin;

		// Create matrix and close up the calculation
		return Mat3x3(float((_axis[0] * _axis[0]) * fOneMinusCos + fCos),
			float((_axis[0] * _axis[1]) * fOneMinusCos - (AzSin)),
			float((_axis[0] * _axis[2]) * fOneMinusCos + (AySin)),
			float((_axis[1] * _axis[0]) * fOneMinusCos + (AzSin)),
			float((_axis[1] * _axis[1]) * fOneMinusCos + fCos),
			float((_axis[1] * _axis[2]) * fOneMinusCos - (AxSin)),
			float((_axis[2] * _axis[0]) * fOneMinusCos - (AySin)),
			float((_axis[2] * _axis[1]) * fOneMinusCos + (AxSin)),
			float((_axis[2] * _axis[2]) * fOneMinusCos + fCos)
			);
	}

	// Matrix from quaternion
	Mat3x3 Mat3x3::Rotation(const Quaternion& _r)
	{
		return Mat3x3(1.0f-2.0f*_r.j*_r.j-2.0f*_r.k*_r.k, 2.0f*_r.i*_r.j-2.0f*_r.k*_r.r,      2.0f*_r.i*_r.k+2.0f*_r.j*_r.r,
					  2.0f*_r.i*_r.j+2.0f*_r.k*_r.r,      1.0f-2.0f*_r.i*_r.i-2.0f*_r.k*_r.k, 2.0f*_r.j*_r.k-2.0f*_r.i*_r.r,
					  2.0f*_r.i*_r.k-2.0f*_r.j*_r.r,      2.0f*_r.j*_r.k+2.0f*_r.i*_r.r,      1.0f-2.0f*_r.i*_r.i-2.0f*_r.j*_r.j);
	}

	// Setup an axis matrix - a vector base
	Mat3x3 Mat3x3::Axis(const Vec3& _xAxis, const Vec3& _yAxis, const Vec3& _zAxis)
	{
		return Mat3x3(_xAxis[0], _xAxis[1], _xAxis[2],
					  _yAxis[0], _yAxis[1], _yAxis[2],
					  _zAxis[0], _zAxis[1], _zAxis[2]);
	}

	// Creates an orthogonal base for a direction vector
	Mat3x3 Mat3x3::Orthonormal(const Vec3& _normal)
	{
		// Calculate a second orthogonal vector with scalar product
		Vec3 v2 = (_normal[0]==1.0f)?Vec3(0.0f, 1.0f, 0.0f): normalize((_normal[1] != 0.0f)? Vec3(1.0f, -_normal[0]/_normal[1], 0.0f) : Vec3(1.0f, 0.0f, -_normal[0]/_normal[2]));
		// 3. Vector is simple the cross product
		Vec3 v3 = cross(_normal, v2);
		return Mat3x3(v2[0],		 v2[1],		v2[2],
					  v3[0],		 v3[1],		v3[2],
					  _normal[0], _normal[1],	_normal[2]);
	}

	// Compute the inverse with determinant method
	Mat3x3 Mat3x3::Inverse() const
	{
		float detInv = 1.0f / Det();
		
		return detInv * Mat3x3((*this)(1,1)*(*this)(2,2) - (*this)(1,2)*(*this)(2,1),
							   (*this)(2,1)*(*this)(0,2) - (*this)(0,1)*(*this)(2,2),
							   (*this)(0,1)*(*this)(1,2) - (*this)(0,2)*(*this)(1,1),
							   (*this)(1,2)*(*this)(2,0) - (*this)(1,0)*(*this)(2,2),
							   (*this)(0,0)*(*this)(2,2) - (*this)(0,2)*(*this)(2,0),
							   (*this)(0,2)*(*this)(1,0) - (*this)(0,0)*(*this)(1,2),
							   (*this)(1,0)*(*this)(2,1) - (*this)(1,1)*(*this)(2,0),
							   (*this)(0,1)*(*this)(2,0) - (*this)(0,0)*(*this)(2,1),
							   (*this)(0,0)*(*this)(1,1) - (*this)(1,0)*(*this)(0,1));
	}

	// Calculate the determinant of the whole 3x3 matrix with Laplace's formula
	float Mat3x3::Det() const {
		return (*this)(0,0) * ((*this)(1,1) * (*this)(2,2) - (*this)(1,2) * (*this)(2,1)) -
			   (*this)(0,1) * ((*this)(1,0) * (*this)(2,2) - (*this)(1,2) * (*this)(2,0)) +
			   (*this)(0,2) * ((*this)(1,0) * (*this)(2,1) - (*this)(1,1) * (*this)(2,0));
	}


	// ********************************************************************* //
	// Computes _v * _A.
	Vec3 operator * (const Vec3& _v, const Mat3x3& _A)
	{
		return Vec3(_v[0]*_A(0,0)+_v[1]*_A(1,0)+_v[2]*_A(2,0),
					_v[0]*_A(0,1)+_v[1]*_A(1,1)+_v[2]*_A(2,1),
					_v[0]*_A(0,2)+_v[1]*_A(1,2)+_v[2]*_A(2,2));
	}

	// ********************************************************************* //
	// Computes _A * _v.
	Vec3 operator * (const Mat3x3& _A, const Vec3& _v)
	{
		return Vec3(_v[0]*_A(0,0)+_v[1]*_A(0,1)+_v[2]*_A(0,2),
					_v[0]*_A(1,0)+_v[1]*_A(1,1)+_v[2]*_A(1,2),
					_v[0]*_A(2,0)+_v[1]*_A(2,1)+_v[2]*_A(2,2));
	}

} // namespace Math