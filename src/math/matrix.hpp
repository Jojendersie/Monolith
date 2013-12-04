#pragma once

#include "math.hpp"

namespace Math {

	class Plane;

	class NoExtraMatrixFunctions;

	// ********************************************************************* //
	/// \brief Class of quadratic n x n matrices.
	/// \details This class uses column major order.
	template<int n, class Derived = NoExtraMatrixFunctions>
	class Matrix
	{
	public:
		/// \brief Standard constructor creates an uninitialized matrix
		Matrix()	{}
		/// \brief Create a matrix filled with a constant.
		explicit Matrix(float _s)	{ for(int i=0; i<n*n; ++i) m_data[i] = _s; }

		// TODO: move, copy, assign?

		/// \brief Casting to a float array of size n²
		operator float* ()					{return m_data;}
		/// \brief Casting to a const float array of size n²
		operator const float* () const		{return m_data;}

		/// \brief Allow implicit cast between Mat4x4/... <-> Matrix<4,4>
		operator Derived&()					{return *reinterpret_cast<Derived*>(this);}
		operator const Derived&() const		{return *reinterpret_cast<Derived*>(this);}

		/// \brief Write access to elements
		float& operator () (int _row, int _column) {return m_data[_row + _column*n];}
		/// \brief Read access to elements
		float operator () (int _row, int _column) const {return m_data[_row + _column*n];}

		template<class D2> Derived& operator += (const Matrix<n,D2>& _m);		///< Add two matrices of the same size
		template<class D2> Derived& operator -= (const Matrix<n,D2>& _m);		///< Subtract two matrices of the same size
		Derived& operator *= (float _s);						///< Multiplication with a scalar
		Derived& operator /= (float _s);						///< Division with a scalar
		template<class D2> Derived operator + (const Matrix<n,D2>& _m) const;	///< Add two matrices of the same size
		template<class D2> Derived operator - (const Matrix<n,D2>& _m) const;	///< Subtract two matrices of the same size
		Derived operator * (float _s) const;					///< Multiplication with a scalar
		Derived operator / (float _s) const;					///< Division with a scalar

		/// \brief Multiply two matrices which share at least one dimension (n)
		template<class D2> Derived& operator *= (const Matrix<n,D2>& _m);

		/// \brief Multiply two matrices which share at least one dimension (n)
		template<class D2> Derived operator * (const Matrix<n,D2>& _m) const;

		/// \brief Computes multiply-add: _op1<m,n> * _op2<n,k> + _op3<m,k>
		template<class D2, class D3> static Derived Mad(const Matrix& _op1, const Matrix<n,D2>& _op2, const Matrix<n,D3>& _op3);

		///< Unary add
		const Derived& operator + () const;
		///< Unary minus
		Derived operator - () const;

		///< Equality check with a small epsilon.
		template<class D2> bool operator == (const Matrix<n,D2>& _op) const;

		///< Inequality check with a small epsilon.
		template<class D2> bool operator != (const Matrix<n,D2>& _op) const;

		/// \brief Create the identity matrix
		static Derived Identity();

		/// \brief Swap rows and columns
		Derived Transpose() const;
	protected:
		float m_data[n*n];
	};

	// ********************************************************************* //
	// Implementations of Matrix member functions and global operators
	// ********************************************************************* //

	// ********************************************************************* //
	// Add two matrices of the same size
	template<int n, class D> template<class D2>
	D& Matrix<n,D>::operator += (const Matrix<n,D2>& _m)
	{
		for(int c<0; c<n; ++c)
			for(int r<0; r<n; ++r)
				m_data[r+c*n] += _m.m_data[r+c*m];
		return *this;
	}

	// ********************************************************************* //
	// Subtract two matrices of the same size
	template<int n, class D> template<class D2>
	D& Matrix<n,D>::operator -= (const Matrix<n,D2>& _m)
	{
		for(int c<0; c<n; ++c)
			for(int r<0; r<n; ++r)
				m_data[r+c*n] -= _m.m_data[r+c*n];
		return *this;
	}

	// ********************************************************************* //
	// Multiplication with a scalar
	template<int n, class D>
	D& Matrix<n,D>::operator *= (float _s)
	{
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				m_data[r+c*n] *= _s;
		return *this;
	}

	// ********************************************************************* //
	// Division with a scalar
	template<int n, class D>
	D& Matrix<n,D>::operator /= (float _s)
	{
		// Multiply with inverse is faster
		_s = 1.0f/_s;
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				m_data[r+c*n] *= _s;
		return *this;
	}

	// ********************************************************************* //
	// Add two matrices of the same size
	template<int n, class D> template<class D2>
	D Matrix<n,D>::operator + (const Matrix<n,D2>& _m) const
	{
		D result(*this);
		result += _m;
		return result;
	}

	// ********************************************************************* //
	// Subtract two matrices of the same size
	template<int n, class D> template<class D2>
	D Matrix<n,D>::operator - (const Matrix<n,D2>& _m) const
	{
		D result(*this);
		result -= _m;
		return result;
	}

	// ********************************************************************* //
	// Multiplication with a scalar
	template<int n, class D>
	D Matrix<n,D>::operator * (float _s) const
	{
		D result(*this);
		result *= _s;
		return result;
	}

	// ********************************************************************* //
	// Division with a scalar
	template<int n, class D>
	D Matrix<n,D>::operator / (float _s) const
	{
		D result(*this);
		result /= _s;
		return result;
	}

	// ********************************************************************* //
	// Multiply two matrices which share at least one dimension (n)
	template<int n, class D> template<class D2>
	D& Matrix<n,D>::operator *= (const Matrix<n,D2>& _m)
	{
		// Matrix multiplication cannot be done in place -> use non assigning
		// multiplication
		*this = *this * _m;
		return *this;
	}

	// ********************************************************************* //
	// Multiply two matrices which share at least one dimension (n)
	template<int n, class D> template<class D2>
	D Matrix<n,D>::operator * (const Matrix<n,D2>& _m) const
	{
		D result(0.0f);
		// Fast column wise multiplication
		for(int h=0; h<n; ++h)
			for(int c=0; c<n; ++c)
				for(int r=0; r<n; ++r) {
					result(r,h) += (*this)(r,c) * _m(c,h);
				}
		return result;
	}

	// ********************************************************************* //
	// Computes multiply-add: _op1<m,n> * _op2<n,k> + _op3<m,k>
	template<int n, class D> template<class D2, class D3>
	D Matrix<n,D>::Mad(const Matrix<n,D>& _op1, const Matrix<n,D2>& _op2, const Matrix<n,D3>& _op3)
	{
		// This copying makes the add
		D result(_op3);
		// Fast column wise multiplication
		for(int h=0; h<n; ++h)
			for(int c=0; c<n; ++c)
				for(int r=0; r<n; ++r)
					result(r,h) += _op1(r,c) * _op2(c,h);
		return result;
	}

	// ********************************************************************* //
	// Unary add
	template<int n, class D>
	const D& Matrix<n,D>::operator + () const
	{
		return *this;
	}

	// ********************************************************************* //
	// Unary minus
	template<int n, class D>
	D Matrix<n,D>::operator - () const
	{
		Matrix<m,n,D> result;
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				result(r,c) = -m_data[r+c*m];
		return result;
	}

	// ********************************************************************* //
	// Create the identity matrix
	template<int n, class D>
	D Matrix<n,D>::Identity()
	{
		D result(0.0f);
		for( int i=0; i<n; ++i )
			result(i,i) = 1.0f;
		return result;
	}

	// ********************************************************************* //
	// Swap rows and columns
	template<int n, class D>
	D Matrix<n,D>::Transpose() const
	{
		D result;
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				result(c,r) = m_data[r+c*n];
		return result;
	}

	// ********************************************************************* //
	// Equality check with a small epsilon.
	template<int n, class D> template<class D2>
	bool Matrix<n,D>::operator == (const Matrix<n,D2>& _op) const
	{
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				if( abs(m_data(r,c)-_op(r,c)) > EPSILON ) return false;
		return true;
	}

	// ********************************************************************* //
	// Inequality check with a small epsilon.
	template<int n, class D> template<class D2>
	bool Matrix<n,D>::operator != (const Matrix<n,D2>& _op) const
	{
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				if( abs(m_data(r,c)-_op(r,c)) > EPSILON ) return true;
		return false;
	}













	// ********************************************************************* //
	/// \brief Standard transformation matrices for 3D graphics.
	class Mat4x4: public Matrix<4, Mat4x4>
	{
	public:
		/// \brief Standard constructor creates an uninitialized matrix
		Mat4x4()	{}

		/// \brief Create a matrix filled with a constant.
		explicit Mat4x4(float _s) : Matrix(_s)	{}

		/// \brief Construction from explicit elements.
		Mat4x4(float _11, float _12, float _13, float _14,
			   float _21, float _22, float _23, float _24,
			   float _31, float _32, float _33, float _34,
			   float _41, float _42, float _43, float _44)
		{
			m_data[0] = _11; m_data[4] = _12; m_data[8]  = _13; m_data[12] = _14;
			m_data[1] = _21; m_data[5] = _22; m_data[9]  = _23; m_data[13] = _24;
			m_data[2] = _31; m_data[6] = _32; m_data[10] = _33; m_data[14] = _34;
			m_data[3] = _41; m_data[7] = _42; m_data[11] = _43; m_data[15] = _44;
/*			m_data[0] = _11; m_data[1] = _12; m_data[2]  = _13; m_data[3] = _14;
			m_data[4] = _21; m_data[5] = _22; m_data[6]  = _23; m_data[7] = _24;
			m_data[8] = _31; m_data[9] = _32; m_data[10] = _33; m_data[11] = _34;
			m_data[12] = _41; m_data[13] = _42; m_data[14] = _43; m_data[15] = _44;*/
		}

		static Mat4x4 Translation(float _x, float _y, float _z);		///< Setup a translation matrix
		static Mat4x4 Translation(const Vec3& _vector);					///< Setup a translation matrix

		static Mat4x4 Scaling(const Vec3& _scale);						///< Setup a translation matrix
		static Mat4x4 Scaling(float _uniformScale);						///< Setup a translation matrix
		static Mat4x4 Scaling(float _sx, float _sy, float _sz);			///< Setup a translation matrix

		static Mat4x4 RotationX(const float _a);						///< Calculate rotation around the x axis
		static Mat4x4 RotationY(const float _a);						///< Calculate rotation around the y axis
		static Mat4x4 RotationZ(const float _a);						///< Calculate rotation around the z axis
		static Mat4x4 Rotation(float _a, float _b, float _c);			///< Rotate around all three axis. This is the same as RotationZ(_c) * RotationX(_b) * RotationY(_a)
		static Mat4x4 Rotation(const Vec3& _angles) {return Rotation(_angles.x, _angles.y, _angles.z);}	///< Rotate around all three axis. This is the same as RotationZ(z) * RotationX(x) * RotationY(y)
		static Mat4x4 Rotation(const Vec3& _axis, float _angle);		///< Rotate around an arbitrary axis
		static Mat4x4 Rotation(const Quaternion& _rotation);			///< Matrix from quaternion

		static Mat4x4 Axis(const Vec3& _xAxis, const Vec3& _yAxis, const Vec3& _zAxis);		///< Setup an axis matrix - a vector base

		static Mat4x4 Projection(float _FOV, float _aspect, float _near, float _far);			///< OpenGL perspective projection matrix
		static Mat4x4 ParallelProjection(float _width, float _heigh, float _near, float _far);	///< OpenGL orthogonal projection matrix
		static Mat4x4 ParallelProjection(float _left, float _right, float _bottom, float _top, float _near, float _far);	///< OpenGL orthogonal projection matrix
		static Mat4x4 Camera(const Vec3& _position, const Vec3& _lookAt, const Vec3& _up = Vec3(0.0f, 1.0f, 0.0f));	///< Calculate camera matrix.
		static Mat4x4 Camera(const Vec3& _position, const Vec3& _direction, const Vec3& _up, const Vec3& _bidir);	///< Calculate camera matrix
		static Mat4x4 Mirror(const Plane& p);							///< Calculate a mirror matrix for given plane
		static Mat4x4 Orthonormal(const Vec3& _normal);					///< Creates an orthonormal base for a direction vector
		static Mat4x4 Transvection(const Vec3& v);						///< Setup shearing matrix

		/// \brief Transform a position vector.
		/// \details the vectors is interpreted as (_v, 1) 4D row vector.
		///		Only the 4x3 sub-matrix is used (last column is ignored).
		Vec3 Transform(const Vec3& _v) const;

		/// \brief Transform a direction vector.
		/// \details the vectors is interpreted as _v 3D row vector.
		///		Only the 3x3 sub-matrix is used (last column is ignored).
		///		
		///		This transformation might be wrong because of scaling parts
		///		in the matrix.
		Vec3 TransformDirection(const Vec3& _v) const;

		/// \brief Transform a position vector and do division through the w part.
		/// \details the vectors is interpreted as (_v, 1) 4D row vector.
		Vec3 TransformPerspective(const Vec3& _v) const;

		/// \brief Compute the inverse with determinant method
		Mat4x4 Invert() const;

		/// \brief Calculate the determinant of the whole 4x4 matrix with
		///		Laplace's formula
		float Det() const;

		///< Solve the equation system Ax=v with Gauss-Jordan method.
		//bool	MatrixSolveEquation(Matrix _A, Vec4& _vX);
	};

	/// \brief Computes (_v,1) * _A which is the same as Transform().
	Vec3 operator * (const Vec3& _v, const Mat4x4& _A);
	/// \brief Computes _A * (_v,1) which is the same as Transpose().Transform().
	Vec3 operator * (const Mat4x4& _A, const Vec3& _v);
	/// \brief Computes _v * _A.
	Vec4 operator * (const Vec4& _v, const Mat4x4& _A);
	/// \brief Computes _A * _v.
	Vec4 operator * (const Mat4x4& _A, const Vec4& _v);







// ******************************************************************************** //
// 2x3 Matrix class for 2D Transformations
// The transformation is applyed as follows: Matrix*(u,v,1)' (2x3 * 3x1 = 2x1)
/*class Matrix2x3
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
		*/

}; // namespace Math