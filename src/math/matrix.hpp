#pragma once

#include "math.hpp"

namespace Math {

// Define used symbols
class Matrix;
class Matrix2x3;
class Plane;
Matrix invert(const Matrix& m);
Matrix2x3 invert(const Matrix2x3& m);

// ******************************************************************************** //
// Die 4D - Matrix class (for 3D Transformations)
class Matrix
{
public:
	// Variables
	union
	{
		struct
		{
			float m11, m12, m13, m14,	// Elements of the matrix
				  m21, m22, m23, m24,
				  m31, m32, m33, m34,
				  m41, m42, m43, m44;
		};

		struct
		{
			float m1[4],				// Row vectors
				 m2[4],
				 m3[4],
				 m4[4];
		};

/*		struct
		{
			Vec4 v1,					// Row vectors
				 v2,
				 v3,
				 v4;
		};*/

		float		m[4][4];			// 2 dimensional array access
	};


	// Constructors
	Matrix() {/*dwMatrixID = Or_MatrixIDCounter++;*/}

	Matrix(const Matrix& m) : m11(m.m11), m12(m.m12), m13(m.m13), m14(m.m14),
                              m21(m.m21), m22(m.m22), m23(m.m23), m24(m.m24),
							  m31(m.m31), m32(m.m32), m33(m.m33), m34(m.m34),
							  m41(m.m41), m42(m.m42), m43(m.m43), m44(m.m44) {/*dwMatrixID = Or_MatrixIDCounter++;*/}

	Matrix(float _m11, float _m12, float _m13, float _m14,
		   float _m21, float _m22, float _m23, float _m24,
		   float _m31, float _m32, float _m33, float _m34,
		   float _m41, float _m42, float _m43, float _m44) : m11(_m11), m12(_m12), m13(_m13), m14(_m14),
			                                                 m21(_m21), m22(_m22), m23(_m23), m24(_m24),
															 m31(_m31), m32(_m32), m33(_m33), m34(_m34),
															 m41(_m41), m42(_m42), m43(_m43), m44(_m44) {/*dwMatrixID = Or_MatrixIDCounter++;*/}

	Matrix(const float* pfValue);

	// Casting-operators
	operator float* ()					{return (float*)(this);}
	operator const float* () const		{return (float*)(this);}

	// Access operators
	float& operator () (int iRow, int iColumn) {return m[iRow][iColumn];}
	float operator () (int iRow, int iColumn) const {return m[iRow][iColumn];}
	inline float& operator[](int i) { return *((float*)this + i); }

	// Assignment operators
	Matrix& operator = (const Matrix& m);
	
	Matrix& operator += (const Matrix& m)
	{
		m11 += m.m11; m12 += m.m12; m13 += m.m13; m14 += m.m14;
		m21 += m.m21; m22 += m.m22; m23 += m.m23; m24 += m.m24;
		m31 += m.m31; m32 += m.m32; m33 += m.m33; m34 += m.m34;
		m41 += m.m41; m42 += m.m42; m43 += m.m43; m44 += m.m44;
		return *this;
	}

	Matrix& operator -= (const Matrix& m)
	{
		m11 -= m.m11; m12 -= m.m12; m13 -= m.m13; m14 -= m.m14;
		m21 -= m.m21; m22 -= m.m22; m23 -= m.m23; m24 -= m.m24;
		m31 -= m.m31; m32 -= m.m32; m33 -= m.m33; m34 -= m.m34;
		m41 -= m.m41; m42 -= m.m42; m43 -= m.m43; m44 -= m.m44;
		return *this;
	}

	Matrix& operator *= (const Matrix& m)
	{
		float r1 = m11, r2 = m12, r3 = m13;
		m11 = m.m11 * m11 + m.m21 * m12 + m.m31 * m13 + m.m41 * m14;
		m12 = m.m12 * r1  + m.m22 * m12 + m.m32 * m13 + m.m42 * m14;
		m13 = m.m13 * r1  + m.m23 * r2  + m.m33 * m13 + m.m43 * m14;
		m14 = m.m14 * r1  + m.m24 * r2  + m.m34 * r3  + m.m44 * m14;
		r1 = m21, r2 = m22, r3 = m23;
		m21 = m.m11 * m21 + m.m21 * m22 + m.m31 * m23 + m.m41 * m24;
		m22 = m.m12 * r1  + m.m22 * m22 + m.m32 * m23 + m.m42 * m24;
		m23 = m.m13 * r1  + m.m23 * r2  + m.m33 * m23 + m.m43 * m24;
		m24 = m.m14 * r1  + m.m24 * r2  + m.m34 * r3  + m.m44 * m24;
		r1 = m31, r2 = m32, r3 = m33;
		m31 = m.m11 * m31 + m.m21 * m32 + m.m31 * m33 + m.m41 * m34;
		m32 = m.m12 * r1  + m.m22 * m32 + m.m32 * m33 + m.m42 * m34;
		m33 = m.m13 * r1  + m.m23 * r2  + m.m33 * m33 + m.m43 * m34;
		m34 = m.m14 * r1  + m.m24 * r2  + m.m34 * r3  + m.m44 * m34;
		r1 = m41, r2 = m42, r3 = m43;
		m41 = m.m11 * m41 + m.m21 * m42 + m.m31 * m43 + m.m41 * m44;
		m42 = m.m12 * r1  + m.m22 * m42 + m.m32 * m43 + m.m42 * m44;
		m43 = m.m13 * r1  + m.m23 * r2  + m.m33 * m43 + m.m43 * m44;
		m44 = m.m14 * r1  + m.m24 * r2  + m.m34 * r3  + m.m44 * m44;
		 return *this;
	}

	Matrix& operator *= (const float f)
	{
		m11 *= f; m12 *= f; m13 *= f; m14 *= f;
		m21 *= f; m22 *= f; m23 *= f; m24 *= f;
		m31 *= f; m32 *= f; m33 *= f; m34 *= f;
		m41 *= f; m42 *= f; m43 *= f; m44 *= f;
		return *this;
	}
	
	Matrix& operator /= (const Matrix& m)
	{
		return *this *= invert(m);
	}

	Matrix& operator /= (float f)
	{
		f = 1/f;
		m11 *= f; m12 *= f; m13 *= f; m14 *= f;
		m21 *= f; m22 *= f; m23 *= f; m24 *= f;
		m31 *= f; m32 *= f; m33 *= f; m34 *= f;
		m41 *= f; m42 *= f; m43 *= f; m44 *= f;
		return *this;
	}

	// Unary operators
	Matrix operator + () const
	{
		return *this;
	}

	Matrix operator - () const
	{
		return Matrix(-m11, -m12, -m13, -m14,
					  -m21, -m22, -m23, -m24,
					  -m31, -m32, -m33, -m34,
					  -m41, -m42, -m43, -m44);
	}
};

typedef Matrix* MatrixP;

// Arithmetical operators
inline Matrix operator + (const Matrix& a, const Matrix& b)	{return Matrix(a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m14 + b.m14, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23, a.m24 + b.m24, a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33, a.m34 + b.m34, a.m41 + b.m41, a.m42 + b.m42, a.m43 + b.m43, a.m44 + b.m44);}
inline Matrix operator - (const Matrix& a, const Matrix& b)	{return Matrix(a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13, a.m14 - b.m14, a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23, a.m24 - b.m24, a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33, a.m34 - b.m34, a.m41 - b.m41, a.m42 - b.m42, a.m43 - b.m43, a.m44 - b.m44);}
inline Matrix operator - (const Matrix& m)					{return Matrix(-m.m11, -m.m12, -m.m13, -m.m14, -m.m21, -m.m22, -m.m23, -m.m24, -m.m31, -m.m32, -m.m33, -m.m34, -m.m41, -m.m42, -m.m43, -m.m44);}

inline Matrix operator * (const Matrix& a,
						  const Matrix& b)
{
	return Matrix(b.m11 * a.m11 + b.m21 * a.m12 + b.m31 * a.m13 + b.m41 * a.m14,
				  b.m12 * a.m11 + b.m22 * a.m12 + b.m32 * a.m13 + b.m42 * a.m14,
				  b.m13 * a.m11 + b.m23 * a.m12 + b.m33 * a.m13 + b.m43 * a.m14,
				  b.m14 * a.m11 + b.m24 * a.m12 + b.m34 * a.m13 + b.m44 * a.m14,
				  b.m11 * a.m21 + b.m21 * a.m22 + b.m31 * a.m23 + b.m41 * a.m24,
				  b.m12 * a.m21 + b.m22 * a.m22 + b.m32 * a.m23 + b.m42 * a.m24,
				  b.m13 * a.m21 + b.m23 * a.m22 + b.m33 * a.m23 + b.m43 * a.m24,
				  b.m14 * a.m21 + b.m24 * a.m22 + b.m34 * a.m23 + b.m44 * a.m24,
				  b.m11 * a.m31 + b.m21 * a.m32 + b.m31 * a.m33 + b.m41 * a.m34,
				  b.m12 * a.m31 + b.m22 * a.m32 + b.m32 * a.m33 + b.m42 * a.m34,
				  b.m13 * a.m31 + b.m23 * a.m32 + b.m33 * a.m33 + b.m43 * a.m34,
				  b.m14 * a.m31 + b.m24 * a.m32 + b.m34 * a.m33 + b.m44 * a.m34,
				  b.m11 * a.m41 + b.m21 * a.m42 + b.m31 * a.m43 + b.m41 * a.m44,
				  b.m12 * a.m41 + b.m22 * a.m42 + b.m32 * a.m43 + b.m42 * a.m44,
				  b.m13 * a.m41 + b.m23 * a.m42 + b.m33 * a.m43 + b.m43 * a.m44,
				  b.m14 * a.m41 + b.m24 * a.m42 + b.m34 * a.m43 + b.m44 * a.m44);
}

inline Matrix operator * (const Matrix& m,
						  const float f)
{
	return Matrix(m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
			      m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
				  m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
				  m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f);
}

inline Matrix operator * (const float f,
						  const Matrix& m)
{
	return Matrix(m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
			      m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
				  m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
				  m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f);
}

// Multiply vector from left (interpret v as row vector with a fourth component of one)
Vec3 operator * (const Vec3& v, const Matrix& m);

// Multiply vector from right (interpret v as col vector with a fourth component of one)
Vec3 operator * (const Matrix& m, const Vec3& v);

// Multiply 4D-vector from left (interpret v as row vector)
Vec4 operator * (const Vec4& v, const Matrix& m);

// Multiply 4D-vector from right
Vec4 operator * (const Matrix& m, const Vec4& v);

inline Matrix operator / (const Matrix& a, const Matrix& b) {return a * invert(b);}

inline Matrix operator / (const Matrix& m,
						  float f)
{
	f = 1/f;
	return Matrix(m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
			      m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
				  m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
				  m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f);
}

// ******************************************************************************** //
// Comparison operators
inline bool operator == (const Matrix& a,
						 const Matrix& b)
{
	if(abs(a.m11 - b.m11) > EPSILON) return false;
	if(abs(a.m12 - b.m12) > EPSILON) return false;
	if(abs(a.m13 - b.m13) > EPSILON) return false;
	if(abs(a.m14 - b.m14) > EPSILON) return false;
	if(abs(a.m21 - b.m21) > EPSILON) return false;
	if(abs(a.m22 - b.m22) > EPSILON) return false;
	if(abs(a.m23 - b.m23) > EPSILON) return false;
	if(abs(a.m24 - b.m24) > EPSILON) return false;
	if(abs(a.m31 - b.m31) > EPSILON) return false;
	if(abs(a.m32 - b.m32) > EPSILON) return false;
	if(abs(a.m33 - b.m33) > EPSILON) return false;
	if(abs(a.m34 - b.m34) > EPSILON) return false;
	if(abs(a.m41 - b.m41) > EPSILON) return false;
	if(abs(a.m42 - b.m42) > EPSILON) return false;
	if(abs(a.m43 - b.m43) > EPSILON) return false;
	return (abs(a.m44 - b.m44) <= EPSILON);
}

inline bool operator != (const Matrix& a,
						 const Matrix& b)
{
	if(abs(a.m11 - b.m11) > EPSILON) return true;
	if(abs(a.m12 - b.m12) > EPSILON) return true;
	if(abs(a.m13 - b.m13) > EPSILON) return true;
	if(abs(a.m14 - b.m14) > EPSILON) return true;
	if(abs(a.m21 - b.m21) > EPSILON) return true;
	if(abs(a.m22 - b.m22) > EPSILON) return true;
	if(abs(a.m23 - b.m23) > EPSILON) return true;
	if(abs(a.m24 - b.m24) > EPSILON) return true;
	if(abs(a.m31 - b.m31) > EPSILON) return true;
	if(abs(a.m32 - b.m32) > EPSILON) return true;
	if(abs(a.m33 - b.m33) > EPSILON) return true;
	if(abs(a.m34 - b.m34) > EPSILON) return true;
	if(abs(a.m41 - b.m41) > EPSILON) return true;
	if(abs(a.m42 - b.m42) > EPSILON) return true;
	if(abs(a.m43 - b.m43) > EPSILON) return true;
	return (abs(a.m44 - b.m44) > EPSILON);
}

// ******************************************************************************** //
// Declaration of functions for matrix creation

		// Returns the identity matrix
inline	Matrix	MatrixIdentity() {return Matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);}
		// Setup translation matrix for (vector * matrix) transformations
		Matrix	MatrixTranslation(const Vec3& v);
		// Setup translation matrix for (vector * matrix) transformations
		Matrix	MatrixTranslation(const float x, const float y, const float z);
		// Calculate rotation around the x axis
		Matrix	MatrixRotationX(const float f);
		// Calculate rotation around the y axis
		Matrix	MatrixRotationY(const float f);
		// Calculate rotation around the z axis
		Matrix	MatrixRotationZ(const float f);
		// Rotate around all three axis. This is the same as
		Matrix	MatrixRotation(const float x, const float y, const float z);
inline	Matrix	MatrixRotation(const Vec3& v)		{return MatrixRotation(v.x, v.y, v.z);}
		// Matrix from quaternion
		Matrix MatrixRotation(const Quaternion& _rotation);
		// Direct computation of MatrixRotation(x,y,z)*MatrixTranslation
		Matrix	MatrixRotation_Translatation(const Vec3& vR, const Vec3& vP);
		// Rotate around an arbitrary axis
		Matrix	MatrixRotationAxis(const Vec3& v, const float f);
		// Setup scaling matrix
		Matrix	MatrixScaling(const Vec3& v);
		// Setup proportional scaling matrix
		Matrix	MatrixScaling(const float f);
		// Setup an axis matrix - a vector base
		Matrix	MatrixAxis(const Vec3& vXAxis, const Vec3& vYAxis, const Vec3& vZAxis);
		// Calculate determinant of the upper left 3x3 sub matrix
		float	MatrixDet3(const Matrix& m);
		// Calculate the determinant of the whole 4x4 matrix with Laplace's formula
		float	MatrixDet(const Matrix& m);
		// Invert matrix
		Matrix	invert(const Matrix& m);
		// Transpose matrix
		Matrix	transpose(const Matrix& m);
		// OpenGL perspective projection matrix
		Matrix	MatrixProjection(const float fFOV, const float fAspect, const float fNearPlane, const float fFarPlane);
		// OpenGL orthogonal projection matrix
		Matrix	MatrixParallelProjection(const float fWidth, const float fHeigh, const float fNearPlane, const float fFarPlane);
		// OpenGL orthogonal projection matrix
		Matrix	MatrixParallelProjection(const float fLeft, const float fRight, const float fBottom, const float fTop, const float fNear, const float fFar);
		// Calculate camera matrix
		Matrix	MatrixCamera(const Vec3& vPos, const Vec3& vLookAt, const Vec3& vUp = Vec3(0.0f, 1.0f, 0.0f));
		// Calculate camera matrix
		Matrix	MatrixCamera(const Vec3& vPos, const Vec3& vDir, const Vec3& vUp, const Vec3& vBidir);
		// Transform to texture matrix
		Matrix	MatrixToTex2DMatrix(const Matrix& m);
		// Calculate a mirror matrix for given plane
		Matrix	MatrixMirror(const Plane& p);
		// Solve the equation system Ax=v with Gauss-Jordan method.
		bool	MatrixSolveEquation(Matrix _A, Vec4& _vX);
		// Creates an orthogonal base for a direction vector
		Matrix	MatrixOrthonormal(const Vec3& vNormal);
		// TODO: Setup shearing matrix
		Matrix	MatrixTransvection(const Vec3& v);


// ******************************************************************************** //
// 2x3 Matrix class for 2D Transformations
// The transformation is applyed as follows: Matrix*(u,v,1)' (2x3 * 3x1 = 2x1)
class Matrix2x3
{
public:
	// Variables
	union
	{
		struct
		{
			float m11, m12, m13,		// Elements of the matrix
				  m21, m22, m23;
		};

		struct
		{
			Vec3 v1;					// Row vectors
			Vec3 v2;
		};

		float		m[2][3];			// 2 dimensional array of elements
		float		n[6];				// 1 dimensional access
	};

	// Constructors
	Matrix2x3() {}

	Matrix2x3(const Matrix2x3& m) : m11(m.m11), m12(m.m12), m13(m.m13),
										m21(m.m21), m22(m.m22), m23(m.m23) {}

	Matrix2x3(float _m11, float _m12, float _m13,
			    float _m21, float _m22, float _m23) : m11(_m11), m12(_m12), m13(_m13),
													  m21(_m21), m22(_m22), m23(_m23) {}

	Matrix2x3(const float* pfValue) : m11(pfValue[0]), m12(pfValue[1]), m13(pfValue[2]),
									    m21(pfValue[3]), m22(pfValue[4]), m23(pfValue[5])	{}

	// Casting-operators
	operator float* ()					{return (float*)(n);}
	operator const float* () const		{return (float*)(n);}

	// Access operators
	float& operator () (int iRow, int iColumn) {return m[iRow][iColumn];}
	float operator () (int iRow, int iColumn) const {return m[iRow][iColumn];}

	// Assignment operators
	Matrix2x3& operator = (const Matrix2x3& m)
	{
		m11 = m.m11; m12 = m.m12; m13 = m.m13;
		m21 = m.m21; m22 = m.m22; m23 = m.m23;
		return *this;
	}
	
	Matrix2x3& operator += (const Matrix2x3& m)
	{
		m11 += m.m11; m12 += m.m12; m13 += m.m13;
		m21 += m.m21; m22 += m.m22; m23 += m.m23;
		return *this;
	}

	Matrix2x3& operator -= (const Matrix2x3& m)
	{
		m11 -= m.m11; m12 -= m.m12; m13 -= m.m13;
		m21 -= m.m21; m22 -= m.m22; m23 -= m.m23;
		return *this;
	}

	Matrix2x3& operator *= (const Matrix2x3& m)
	{
		float r1 = m11, r2 = m12;
		m11 = m.m11 * m11 + m.m21 * m12;
		m12 = m.m12 * r1  + m.m22 * m12;
		m13 = m.m13 * r1  + m.m23 * r2  + m13;
		r1 = m21, r2 = m22;
		m21 = m.m11 * m21 + m.m21 * m22;
		m22 = m.m12 * r1  + m.m22 * m22;
		m23 = m.m13 * r1  + m.m23 * r2  + m23;
		 return *this;
	}

	Matrix2x3& operator *= (const float f)
	{
		m11 *= f; m12 *= f; m13 *= f;
		m21 *= f; m22 *= f; m23 *= f;
		return *this;
	}
	
	Matrix2x3& operator /= (const Matrix2x3& m)
	{
		return *this *= invert(m);
	}

	Matrix2x3& operator /= (float f)
	{
		f = 1/f;
		m11 *= f; m12 *= f; m13 *= f;
		m21 *= f; m22 *= f; m23 *= f;
		return *this;
	}

	// Unary operators
	Matrix2x3 operator + () const
	{
		return *this;
	}

	Matrix2x3 operator - () const
	{
		return Matrix2x3(-m11, -m12, -m13,
						 -m21, -m22, -m23);
	}
};

typedef Matrix2x3* Matrix2x3P;


// ******************************************************************************** //
// Arithmetical operators
inline Matrix2x3 operator + (const Matrix2x3& a, const Matrix2x3& b)	{return Matrix2x3(a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23);}
inline Matrix2x3 operator - (const Matrix2x3& a, const Matrix2x3& b)	{return Matrix2x3(a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13, a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23);}
inline Matrix2x3 operator - (const Matrix2x3& m)						{return Matrix2x3(-m.m11, -m.m12, -m.m13, -m.m21, -m.m22, -m.m23);}

inline Matrix2x3 operator * (const Matrix2x3& a,
							 const Matrix2x3& b)
{
	return Matrix2x3(b.m11 * a.m11 + b.m21 * a.m12,
					   b.m12 * a.m11 + b.m22 * a.m12,
					   b.m13 * a.m11 + b.m23 * a.m12 + a.m13,
					   b.m11 * a.m21 + b.m21 * a.m22,
					   b.m12 * a.m21 + b.m22 * a.m22,
					   b.m13 * a.m21 + b.m23 * a.m22 + a.m23);
}

inline Matrix2x3 operator * (const Matrix2x3& m,
							 const float f)
{
	return Matrix2x3(m.m11 * f, m.m12 * f, m.m13 * f,
			           m.m21 * f, m.m22 * f, m.m23 * f);
}

inline Matrix2x3 operator * (const float f,
							 const Matrix2x3& m)
{
	return Matrix2x3(m.m11 * f, m.m12 * f, m.m13 * f,
			           m.m21 * f, m.m22 * f, m.m23 * f);
}

inline Vec2 operator * (const Vec2& v,
						const Matrix2x3& m)
{
	return Vec2(v.x * m.m11 + v.y * m.m21,
				v.x * m.m12 + v.y * m.m22);
}

inline Vec2 operator * (const Matrix2x3& m,
						const Vec2& v)
{
	// Multiply matrix from left: 2x3 * (2+1)x1 = 2x1
	return Vec2(m.m11*v.x + m.m12*v.y + m.m13,
				m.m21*v.x + m.m22*v.y + m.m23);
}

inline Matrix2x3 operator / (const Matrix2x3& a, const Matrix2x3& b) {return a * invert(b);}

inline Matrix2x3 operator / (const Matrix2x3& m,
							   float f)
{
	f = 1/f;
	return Matrix2x3(m.m11 * f, m.m12 * f, m.m13 * f,
			         m.m21 * f, m.m22 * f, m.m23 * f);
}

// ******************************************************************************** //
// Comparison operators
inline bool operator == (const Matrix2x3& a,
						 const Matrix2x3& b)
{
	if(abs(a.m11 - b.m11) > EPSILON) return false;
	if(abs(a.m12 - b.m12) > EPSILON) return false;
	if(abs(a.m13 - b.m13) > EPSILON) return false;
	if(abs(a.m21 - b.m21) > EPSILON) return false;
	if(abs(a.m22 - b.m22) > EPSILON) return false;
	if(abs(a.m23 - b.m23) > EPSILON) return false;
	return (abs(a.m23 - b.m23) <= EPSILON);
}

inline bool operator != (const Matrix2x3& a,
						 const Matrix2x3& b)
{
	if(abs(a.m11 - b.m11) > EPSILON) return true;
	if(abs(a.m12 - b.m12) > EPSILON) return true;
	if(abs(a.m13 - b.m13) > EPSILON) return true;
	if(abs(a.m21 - b.m21) > EPSILON) return true;
	if(abs(a.m22 - b.m22) > EPSILON) return true;
	if(abs(a.m23 - b.m23) > EPSILON) return true;
	return (abs(a.m23 - b.m23) > EPSILON);
}

// ******************************************************************************** //
// Declaration of functions for matrix creation. All functions simulate a 3x3 matrix
// with some parts left away.
		// Return identity matrix
inline	Matrix2x3	Matrix2x3Identity() {return Matrix2x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);}
		// Setup translation matrix for (matrix * vector) transformations
		Matrix2x3	Matrix2x3Translation(const Vec2& v);
		// Setup translation matrix for (matrix * vector) transformations
		Matrix2x3	Matrix2x3Translation(const float x, const float y);
		// Calculate rotation around the "z" axis
		Matrix2x3	Matrix2x3Rotation(const float f);
		// Setup scaling matrix
		Matrix2x3	Matrix2x3Scaling(const Vec2& v);
		// Setup scaling matrix
		Matrix2x3	Matrix2x3Scaling(const float x, const float y);
		// Setup proportional scaling matrix
		Matrix2x3	Matrix2x3Scaling(const float f);
		// Create a base in R^2 from two vectors
		Matrix2x3	Matrix2x3Axis(const Vec2& vXAxis, const Vec2& vYAxis);
		// Invert matrix
		Matrix2x3	invert(const Matrix2x3& m);
		// Setup shearing matrix
		Matrix2x3	Matrix2x3Transvection(const Vec2& v);
		// Setup shearing matrix
		Matrix2x3	Matrix2x3Transvection(const float x, const float y);
		// Compute determinant
		float		Matrix2x3Det(const Matrix2x3& m);


}; // namespace Math