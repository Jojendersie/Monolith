#include "mathbase.hpp"
#include "matrix.hpp"
#include "plane.hpp"
#include <cstring>

namespace Math {

// ******************************************************************************** //
Matrix::Matrix(const float* pfValue)	{memcpy(this, pfValue, sizeof(float)*16);/*dwMatrixID = Or_MatrixIDCounter++;*/}

// ******************************************************************************** //
// Operators
Matrix& Matrix::operator = (const Matrix& m) {memcpy(this, &m, sizeof(float)*16); return *this;}

// ******************************************************************************** //
// Multiply vector from left (interpret v as row vector with a fourth component of one)
Vec3 operator * (const Vec3& v,
				 const Matrix& m)
{
	// Uniform coordinates in 4D -> division through w
	const double w = 1.0/(v.x * m.m14 + v.y * m.m24 + v.z * m.m34 + m.m44);

	return Vec3(float( w * (v.x * m.m11 + v.y * m.m21 + v.z * m.m31 + m.m41) ),
				float( w * (v.x * m.m12 + v.y * m.m22 + v.z * m.m32 + m.m42) ),
				float( w * (v.x * m.m13 + v.y * m.m23 + v.z * m.m33 + m.m43) ));
}

// ******************************************************************************** //
// Multiply vector from right (interpret v as col vector with a fourth component of one)
Vec3 operator * (const Matrix& m,
				 const Vec3& v)
{
	// Uniform coordinates in 4D -> division through w
	const double w = 1.0/(v.x * m.m41 + v.y * m.m42 + v.z * m.m43 + m.m44);

	return Vec3(float( w * (v.x * m.m11 + v.y * m.m12 + v.z * m.m13 + m.m14) ),
				float( w * (v.x * m.m21 + v.y * m.m22 + v.z * m.m23 + m.m24) ),
				float( w * (v.x * m.m31 + v.y * m.m32 + v.z * m.m33 + m.m34) ));
}

// ******************************************************************************** //
// Multiply 4D-vector from left (interpret v as row vector)
Vec4 operator * (const Vec4& v,
				 const Matrix& m)
{
	return Vec4( v.x * m.m11 + v.y * m.m21 + v.z * m.m31 + m.m41 ,
				 v.x * m.m12 + v.y * m.m22 + v.z * m.m32 + m.m42 ,
				 v.x * m.m13 + v.y * m.m23 + v.z * m.m33 + m.m43 ,
				 v.x * m.m14 + v.y * m.m24 + v.z * m.m34 + m.m44 );
}

// ******************************************************************************** //
// Multiply 4D-vector from right
Vec4 operator * (const Matrix& m,
				 const Vec4& v)
{
	return Vec4( v.x * m.m11 + v.y * m.m12 + v.z * m.m13 + m.m14 ,
				 v.x * m.m21 + v.y * m.m22 + v.z * m.m23 + m.m24 ,
				 v.x * m.m31 + v.y * m.m32 + v.z * m.m33 + m.m34 ,
				 v.x * m.m41 + v.y * m.m42 + v.z * m.m43 + m.m44 );
}

// ******************************************************************************** //
// Setup translation matrix for (vector * matrix) transformations
Matrix MatrixTranslation( const Vec3& v )
{
	return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
		          0.0f, 1.0f, 0.0f, 0.0f,
				  0.0f, 0.0f, 1.0f, 0.0f,
				  v.x,  v.y,  v.z,  1.0f);
}

// ******************************************************************************** //
// Setup translation matrix for (vector * matrix) transformations
Matrix MatrixTranslation(const float x, const float y, const float z)
{
	return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
		          0.0f, 1.0f, 0.0f, 0.0f,
				  0.0f, 0.0f, 1.0f, 0.0f,
				  x,    y,    z,    1.0f);
}

// ******************************************************************************** //
// Calculate rotation around the x axis
Matrix MatrixRotationX(const float f)
{
	Matrix mResult;

	mResult.m11 = 1.0f; mResult.m12 = 0.0f; mResult.m13 = 0.0f; mResult.m14 = 0.0f;
	mResult.m21 = 0.0f;                                         mResult.m24 = 0.0f;
	mResult.m31 = 0.0f;                                         mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;

	// Use Sin and Cos as few as possible
	mResult.m22 = mResult.m33 = cos(f);
	mResult.m23 = sin(f);
	mResult.m32 = -mResult.m23;

	return mResult;
}

// ******************************************************************************** //
// Calculate rotation around the y axis
Matrix MatrixRotationY(const float f)
{
	Matrix mResult;

	                    mResult.m12 = 0.0f;                     mResult.m14 = 0.0f;
	mResult.m21 = 0.0f; mResult.m22 = 1.0f; mResult.m23 = 0.0f; mResult.m24 = 0.0f;
	                    mResult.m32 = 0.0f;                     mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;

	mResult.m11 = mResult.m33 = cos(f);
	mResult.m31 = sin(f);
	mResult.m13 = -mResult.m31;

	return mResult;
}

// ******************************************************************************** //
// Calculate rotation around the z axis
Matrix MatrixRotationZ(const float f)
{
	Matrix mResult;

	                                        mResult.m13 = 0.0f; mResult.m14 = 0.0f;
	                                        mResult.m23 = 0.0f; mResult.m24 = 0.0f;
	mResult.m31 = 0.0f; mResult.m32 = 0.0f; mResult.m33 = 1.0f; mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;

	mResult.m11 = mResult.m22 = cos(f);
	mResult.m12 = sin(f);
	mResult.m21 = -mResult.m12;

	return mResult;
}

// ******************************************************************************** //
// Rotate around all three axis. This is the same as
// MatrixRotationZ(z) * MatrixRotationX(x) * MatrixRotationY(y);
Matrix MatrixRotation(const float x,
						  const float y,
						  const float z)
{
	float fSinX = sin(x), fSinY = sin(y), fSinZ = sin(z);
	float fCosX = cos(x), fCosY = cos(y), fCosZ = cos(z);
	float fSinXY = fSinX*fSinY;
	float fCosYZ = fCosY*fCosZ;
	float fCosYSinZ = fCosY*fSinZ;
	return Matrix(fCosYZ-fSinXY*fSinZ,		fCosX*fSinZ, fSinY*fCosZ+fSinX*fCosYSinZ,	0.0f,
					-fCosYSinZ-fSinXY*fCosZ,	fCosX*fCosZ, fSinX*fCosYZ-fSinY*fSinZ,		0.0f,
					-fCosX*fSinY,				-fSinX,		 fCosX*fCosY,					0.0f,
					0.0f,						0.0f,		 0.0f,							1.0f);
}

// ******************************************************************************** //
// Rotate around an arbitrary axis
Matrix MatrixRotationAxis(const Vec3& vAxis,
									 const float f)
{
	// Calculate as much as possible without redundant calculations
	const double fSin = sin(double(-f));
	const double fCos = cos(double(-f));
	const double fOneMinusCos = 1.0 - fCos;
	const double AxSin = vAxis.x * fSin;
	const double AySin = vAxis.y * fSin;
	const double AzSin = vAxis.z * fSin;

	// Create matrix and close up the calculation
	return Matrix(float((vAxis.x * vAxis.x) * fOneMinusCos + fCos),
		          float((vAxis.x * vAxis.y) * fOneMinusCos - (AzSin)),
				  float((vAxis.x * vAxis.z) * fOneMinusCos + (AySin)),
				  0.0f,
				  float((vAxis.y * vAxis.x) * fOneMinusCos + (AzSin)),
				  float((vAxis.y * vAxis.y) * fOneMinusCos + fCos),
				  float((vAxis.y * vAxis.z) * fOneMinusCos - (AxSin)),
				  0.0f,
				  float((vAxis.z * vAxis.x) * fOneMinusCos - (AySin)),
				  float((vAxis.z * vAxis.y) * fOneMinusCos + (AxSin)),
				  float((vAxis.z * vAxis.z) * fOneMinusCos + fCos),
				  0.0f,
				  0.0f,
				  0.0f,
				  0.0f,
				  1.0f);
}

// ******************************************************************************** //
// Direct computation of MatrixRotation(x,y,z)*MatrixTranslation
Matrix MatrixRotation_Translatation(const Vec3& vR,
											   const Vec3& vP)
{
	float fSinX = sin(vR.x), fSinY = sin(vR.y), fSinZ = sin(vR.z);
	float fCosX = cos(vR.x), fCosY = cos(vR.y), fCosZ = cos(vR.z);
	float fSinXY = fSinX*fSinY;
	float fCosYZ = fCosY*fCosZ;
	float fCosYSinZ = fCosY*fSinZ;
	return Matrix(fCosYZ-fSinXY*fSinZ,		fCosX*fSinZ, fSinY*fCosZ+fSinX*fCosYSinZ,	0.0f,
				  -fCosYSinZ-fSinXY*fCosZ,	fCosX*fCosZ, fSinX*fCosYZ-fSinY*fSinZ,		0.0f,
				  -fCosX*fSinY,				-fSinX,		 fCosX*fCosY,					0.0f,
				  vP.x,						vP.y,		 vP.z,							1.0f);
}

// ******************************************************************************** //
// Setup scaling matrix
Matrix MatrixScaling(const Vec3& v)
{
	return Matrix(v.x,  0.0f, 0.0f, 0.0f,
		          0.0f, v.y,  0.0f, 0.0f,
				  0.0f, 0.0f, v.z,  0.0f,
				  0.0f, 0.0f, 0.0f, 1.0f);
}

// ******************************************************************************** //
// Setup proportional scaling matrix
Matrix MatrixScaling(const float f)
{
	return Matrix(f,  0.0f, 0.0f, 0.0f,
				  0.0f, f,  0.0f, 0.0f,
				  0.0f, 0.0f, f,  0.0f,
				  0.0f, 0.0f, 0.0f, 1.0f);
}

// ******************************************************************************** //
// Setup an axis matrix - a vector base
Matrix MatrixAxis(const Vec3& vXAxis,
					  const Vec3& vYAxis,
					  const Vec3& vZAxis)
{
	return Matrix(vXAxis.x, vXAxis.y, vXAxis.z, 0.0f,
		          vYAxis.x, vYAxis.y, vYAxis.z, 0.0f,
				  vZAxis.x, vZAxis.y, vZAxis.z, 0.0f,
				  0.0f,     0.0f,     0.0f,     1.0f);
}

// ******************************************************************************** //
// Calculate determinant of the upper left 3x3 sub matrix
float MatrixDet3(const Matrix& m)
{
	return m.m11 * (m.m22 * m.m33 - m.m23 * m.m32) -
           m.m12 * (m.m21 * m.m33 - m.m23 * m.m31) +
           m.m13 * (m.m21 * m.m32 - m.m22 * m.m31);
}

// ******************************************************************************** //
// Calculate the determinant of the whole 4x4 matrix with Laplace's formula
float MatrixDet(const Matrix& m)
{
	// Determinant of upper left 3x3-sub matrix
	// (cross out 4. row and 4. column)
	float detA44 = + m.m11 * (m.m22 * m.m33 - m.m23 * m.m32)
				   - m.m12 * (m.m21 * m.m33 - m.m23 * m.m31)
				   + m.m13 * (m.m21 * m.m32 - m.m22 * m.m31);
	// (cross out 4. row and 3. column)
	float detA43 = + m.m11 * (m.m22 * m.m34 - m.m24 * m.m32)
				   - m.m12 * (m.m21 * m.m34 - m.m24 * m.m31)
				   + m.m14 * (m.m21 * m.m32 - m.m22 * m.m31);
	// (cross out 4. row and 2. column)
	float detA42 = + m.m11 * (m.m23 * m.m34 - m.m24 * m.m33)
				   - m.m13 * (m.m21 * m.m34 - m.m24 * m.m31)
				   + m.m14 * (m.m21 * m.m33 - m.m23 * m.m31);
	// (cross out 4. row and 1. column)
	float detA41 = + m.m12 * (m.m23 * m.m34 - m.m24 * m.m33)
				   - m.m13 * (m.m22 * m.m34 - m.m24 * m.m32)
				   + m.m14 * (m.m22 * m.m33 - m.m23 * m.m32);

	// Sum determinants from sub matrices
	return - m.m41 * detA41
		   + m.m42 * detA42
		   - m.m43 * detA43
		   + m.m44 * detA44;
}


// ******************************************************************************** //
// Invert a matrix
// Returns the identity matrix, if matrix is not invertible .
Matrix invert(const Matrix& m)
{
	Matrix mSolution;
	Vec4 v, vec[3];
	float fDet;

	fDet = MatrixDet(m);

	if(!fDet)
		return MatrixIdentity();

	// Swap sign in each pass
	float fSignDet = 1.0f/fDet;

	for(int i=0; i<4; i++)
	{
		// Calculate determinant as cross product
		v = cross(m.m[0<i?0:1], m.m[1<i?1:2], m.m[2<i?2:3]);

		// Sign * Determinant_i / Determinante
		mSolution.m[0][i] = fSignDet * v.x;
		mSolution.m[1][i] = fSignDet * v.y;
		mSolution.m[2][i] = fSignDet * v.z;
		mSolution.m[3][i] = fSignDet * v.w;
		fSignDet *= -1.0f;
	}

	return mSolution;
}

// ******************************************************************************** //
// Transpose matrix
Matrix MatrixTranspose(const Matrix& m)
{
	return Matrix(m.m11, m.m21, m.m31, m.m41,
		          m.m12, m.m22, m.m32, m.m42,
				  m.m13, m.m23, m.m33, m.m43,
				  m.m14, m.m24, m.m34, m.m44);
}

// ******************************************************************************** //
// OpenGL perspective projection matrix
Matrix MatrixProjection(const float fFOV,
							const float fAspect,
							const float fNearPlane,
							const float fFarPlane)
{
	const float fFar = fFarPlane * 0.5f;
	const float s = 1.0f / tan(fFOV * 0.5f);
	const float fFrustumLengthInv = 1.0f / (fNearPlane - fFar);

	return Matrix(s / fAspect,	0.0f,  0.0f,									0.0f,
				  0.0f,			s,	   0.0f,									0.0f,
				  0.0f,			0.0f, (fNearPlane + fFar)*fFrustumLengthInv,	(2.0f*fNearPlane*fFar)*fFrustumLengthInv,
				  0.0f,			0.0f, -1.0f,									0.0f);
	// DirectX compatible version:
/*	const float s = 1.0f / Tan(fFOV * 0.5f);
	const float Q = fFarPlane / (fFarPlane - fNearPlane);

	return Matrix(s / fAspect, 0.0f, 0.0f,			0.0f,
		            0.0f,		 s,	   0.0f,			0.0f,
					0.0f,		 0.0f, Q,				1.0f,
					0.0f,		 0.0f, -Q * fNearPlane, 0.0f);*/
}

// ******************************************************************************** //
// OpenGL orthogonal projection matrix
Matrix MatrixParallelProjection(const float fWidth,
									const float fHeigh,
									const float fNearPlane,
									const float fFarPlane)
{
	const float s = 2.0f*fNearPlane;
	const float Q = fFarPlane / (fFarPlane - fNearPlane);

	return Matrix(s / fWidth,	0.0f,		0.0f,				0.0f,
		          0.0f,			s / fHeigh,	0.0f,				0.0f,
				  0.0f,			0.0f,		Q,					1.0f,
				  0.0f,			0.0f,		-Q * fNearPlane,	0.0f);
}

// ******************************************************************************** //
Matrix MatrixParallelProjection(const float l, const float r, const float b, const float t, const float n, const float f)
{
	const float	r_l = 1.0f/(r - l);
	const float t_b = 1.0f/(t - b);
	const float f_n = 1.0f/(n - f);
	return Matrix( 2.0f*r_l,	0.0f,		0.0f,		0.0f,
				   0.0f,		2.0f*t_b,	0.0f,		0.0f,
				   0.0f,		0.0f,		2.0f*f_n,	0.0f,
				   -(l+r)*r_l,	-(b+t)*t_b,	-(f+n)*f_n,	1.0f);
}

// ******************************************************************************** //
// Calculate camera matrix
Matrix MatrixCamera(const Vec3& vPos,
						const Vec3& vLookAt,
						const Vec3& vUp) // = Vec3(0.0f, 1.0f, 0.0f)
{
	// Determine the z axis of camera coordinate system
	Vec3 vZAxis(normalize(vLookAt - vPos));

	// The c axis is the cross product from y and z axis
	Vec3 vXAxis(normalize(cross(vUp, vZAxis)));

	// calculate an orthogonal y axis
	Vec3 vYAxis(normalize(cross(vZAxis, vXAxis)));

	// Create rotation matrix simply from local coordinate system and move
	return MatrixTranslation(-vPos) *
	       Matrix(vXAxis.x, vYAxis.x, vZAxis.x, 0.0f,
		          vXAxis.y, vYAxis.y, vZAxis.y, 0.0f,
				  vXAxis.z, vYAxis.z, vZAxis.z, 0.0f,
				  0.0f,     0.0f,     0.0f,     1.0f);// *
		   //MatrixTranslation(-vPos);
}

// ******************************************************************************** //
// Calculate camera matrix
Matrix MatrixCamera(const Vec3& vPos, const Vec3& vDir, const Vec3& vUp, const Vec3& vBidir)
{
	// Easy way - all axis already given
	return MatrixTranslation(-vPos) *
	       Matrix(vBidir.x, vUp.x, vDir.x, 0.0f,
		          vBidir.y, vUp.y, vDir.y, 0.0f,
				  vBidir.z, vUp.z, vDir.z, 0.0f,
				  0.0f,     0.0f,  0.0f,   1.0f);// *
		//   MatrixTranslation(-vPos);
}

// ******************************************************************************** //
// Setup texture matrix
Matrix MatrixToTex2DMatrix(const Matrix& m)
{
	// Cross out the z-axis for texture transformation
	return Matrix(m.m11, m.m12, m.m14, 0.0f,
		            m.m21, m.m22, m.m24, 0.0f,
					m.m41, m.m42, m.m44, 0.0f,
					0.0f,  0.0f,  0.0f,  1.0f);
}

// ******************************************************************************** //
// Calculate a mirror matrix for given plane
// TODO: transponieren? TEST
Matrix MatrixMirror(const Plane& p)
{
	return Matrix(-2.0f*p.a*p.a + 1.0f,	-2.0f*p.b*p.a,		-2.0f*p.c*p.a,		  0.0f,
					-2.0f*p.a*p.b,		-2.0f*p.b + 1.0f,	-2.0f*p.c*p.b,		  0.0f,
					-2.0f*p.a*p.c,		-2.0f*p.b*p.c,		-2.0f*p.c*p.c + 1.0f, 0.0f,
					-2.0f*p.a*p.d,		-2.0f*p.b*p.d,		-2.0f*p.c*p.d,		  1.0f);
}


// ******************************************************************************** //
// Solve the equation system Ax=v with Gauß-Jordan method.
// The return value is true iff an unique solution exists. The solution is saved
// in _pV_X
bool MatrixSolveEquation(Matrix _A, Vec4& _vX)
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
}

// ******************************************************************************** //
// Creates an orthogonal base for a direction vector
Matrix MatrixOrthonormal(const Vec3& vNormal)
{
	// Calculate a second orthogonal vector with scalar product
	Vec3 v2 = (vNormal.x==1.0f)?Vec3(0.0f, 1.0f, 0.0f): normalize((vNormal.y != 0.0f)? Vec3(1.0f, -vNormal.x/vNormal.y, 0.0f) : Vec3(1.0f, 0.0f, -vNormal.x/vNormal.z));
	// 3. Vector is simple the cross product
	Vec3 v3 = cross(vNormal, v2);
	return Matrix(v2.x,		v2.y,		v2.z,		0.0f,
				  v3.x,		v3.y,		v3.z,		0.0f,
				  vNormal.x,	vNormal.y,	vNormal.z,	0.0f,
				  0.0f,		0.0f,		0.0f,		1.0f);
	//return Matrix(vNormal.x,	vNormal.y,	vNormal.z,	0.0f,
	//				v2.x,		v2.y,		v2.z,		0.0f,
	//				v3.x,		v3.y,		v3.z,		0.0f,
	//				0.0f,		0.0f,		0.0f,		1.0f);
}



// ******************************************************************************** //
// ******************************************************************************** //
// ******************************************************************************** //
// The following functions all simulate a 3x3 matrix

// ******************************************************************************** //
// Setup translation matrix for (matrix * vector) transformations
Matrix2x3 Matrix2x3Translation(const Vec2& v)
{
	return Matrix2x3(1.0f, 0.0f, v.x,
					 0.0f, 1.0f, v.y);
}

// ******************************************************************************** //
// Setup translation matrix for (matrix * vector) transformations
Matrix2x3 Matrix2x3Translation(const float x, const float y)
{
	return Matrix2x3(1.0f, 0.0f, x,
					 0.0f, 1.0f, y);
}

// ******************************************************************************** //
// Calculate rotation around the "z" axis
Matrix2x3 Matrix2x3Rotation(const float f)
{
	float fCos = cos(f);
	float fSin = sin(f);
	return Matrix2x3(fCos, -fSin, 0.0f,
					 fSin , fCos, 0.0f);
}

// ******************************************************************************** //
// Setup scaling matrix
Matrix2x3 Matrix2x3Scaling(const Vec2& v)
{
	return Matrix2x3(v.x , 0.0f, 0.0f,
					 0.0f, v.y , 0.0f);
}

// ******************************************************************************** //
// Setup scaling matrix
Matrix2x3 Matrix2x3Scaling(const float x, const float y)
{
	return Matrix2x3(x   , 0.0f, 0.0f,
					 0.0f, y   , 0.0f);
}

// ******************************************************************************** //
// Setup proportional scaling matrix
Matrix2x3 Matrix2x3Scaling(const float f)
{
	return Matrix2x3(f , 0.0f, 0.0f,
					 0.0f, f , 0.0f);
}

// ******************************************************************************** //
// Create a base in R^2 from two vectors
Matrix2x3 Matrix2x3Axis(const Vec2& vXAxis, const Vec2& vYAxis)
{
	return Matrix2x3(vXAxis.x, vXAxis.y, 0.0f,
					 vYAxis.x, vYAxis.y, 0.0f);
}

// ******************************************************************************** //
// Invert matrix
Matrix2x3 Matrix2x3Invert(const Matrix2x3& m)
{
	float fDet = 1.0f/Matrix2x3Det(m);
	// simulate 3x3 Matrix with (0,0,1) in the 3rd row
	return Matrix2x3(m.m22*fDet, -m.m12*fDet, (m.m12*m.m23-m.m13*m.m22)*fDet,
					 -m.m21*fDet, m.m11*fDet, (m.m13*m.m21-m.m11*m.m23)*fDet);
}

// ******************************************************************************** //
// Setup shearing matrix
Matrix2x3 Matrix2x3Transvection(const Vec2& v)
{
	return Matrix2x3(1.0f, v.x , 0.0f,
					 v.y , 1.0f, 0.0f);
}

// ******************************************************************************** //
// Setup shearing matrix
Matrix2x3 Matrix2x3Transvection(const float x, const float y)
{
	return Matrix2x3(1.0f, x   , 0.0f,
					 y   , 1.0f, 0.0f);
}

// ******************************************************************************** //
// Compute determinant
float Matrix2x3Det(const Matrix2x3& m)
{
	return m.m11 * m.m22 -
           m.m12 * m.m21;
}

};