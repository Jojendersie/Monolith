#pragma once

#include "../predeclarations.hpp"
#include "vector.hpp"

namespace Math {

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

		/// \brief Allow implicit cast between Mat4x4/... <-> Matrix<4,4>, ...
		operator Derived&()					{return *reinterpret_cast<Derived*>(this);}
		operator const Derived&() const		{return *reinterpret_cast<const Derived*>(this);}

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
		Derived Transposed() const;

		/// \brief Column vector access.
		Vector<n, float>& Column(int _index)				{ return *(Vector<n, float>*)&m_data[_index*n]; }
		const Vector<n, float>& Column(int _index) const	{ return *(Vector<n, float>*)&m_data[_index*n]; }
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
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
				m_data[r+c*n] += _m.m_data[r+c*m];
		return *this;
	}

	// ********************************************************************* //
	// Subtract two matrices of the same size
	template<int n, class D> template<class D2>
	D& Matrix<n,D>::operator -= (const Matrix<n,D2>& _m)
	{
		for(int c=0; c<n; ++c)
			for(int r=0; r<n; ++r)
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

	template<int n, class D>
	inline D operator * (float _s, const Matrix<n,D>& _m)
	{
		D result(_m);
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
	D Matrix<n,D>::Transposed() const
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
		static Mat4x4 Rotation(const Vec3& _angles) {return Rotation(_angles[0], _angles[1], _angles[2]);}	///< Rotate around all three axis. This is the same as RotationZ(z) * RotationX(x) * RotationY(y)
		static Mat4x4 Rotation(const Vec3& _axis, float _angle);		///< Rotate around an arbitrary axis
		static Mat4x4 Rotation(const Quaternion& _rotation);			///< Matrix from quaternion

		static Mat4x4 Axis(const Vec3& _xAxis, const Vec3& _yAxis, const Vec3& _zAxis);		///< Setup an axis matrix - a vector base (columnwise)

		static Mat4x4 Projection(float _FOV, float _aspect, float _near, float _far);			///< OpenGL perspective projection matrix
		static Mat4x4 ParallelProjection(float _width, float _heigh, float _near, float _far);	///< OpenGL orthogonal projection matrix
		static Mat4x4 ParallelProjection(float _left, float _right, float _bottom, float _top, float _near, float _far);	///< OpenGL orthogonal projection matrix
		static Mat4x4 Camera(const Vec3& _position, const Vec3& _lookAt, const Vec3& _up = Vec3(0.0f, 1.0f, 0.0f));	///< Calculate camera matrix.
		static Mat4x4 Camera(const Vec3& _position, const Vec3& _direction, const Vec3& _up, const Vec3& _bidir);	///< Calculate camera matrix
		static Mat4x4 Mirror(const Plane& p);							///< Calculate a mirror matrix for given plane
		static Mat4x4 Orthonormal(const Vec3& _normal);					///< Creates an orthonormal base for a direction vector

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
		Mat4x4 Inverse() const;

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






	// ********************************************************************* //
	/// \brief 3x3 transformation matrices for rigid transformations.
	class Mat3x3: public Matrix<3, Mat3x3>
	{
	public:
		/// \brief Standard constructor creates an uninitialized matrix
		Mat3x3()	{}

		/// \brief Create a matrix filled with a constant.
		explicit Mat3x3(float _s) : Matrix(_s)	{}

		/// \brief Construction from explicit elements.
		Mat3x3(float _11, float _12, float _13,
			   float _21, float _22, float _23,
			   float _31, float _32, float _33)
		{
			m_data[0] = _11; m_data[3] = _12; m_data[6] = _13;
			m_data[1] = _21; m_data[4] = _22; m_data[7] = _23;
			m_data[2] = _31; m_data[5] = _32; m_data[8] = _33;
		}

		static Mat3x3 Scaling(const Vec3& _scale);						///< Setup a translation matrix
		static Mat3x3 Scaling(float _uniformScale);						///< Setup a translation matrix
		static Mat3x3 Scaling(float _sx, float _sy, float _sz);			///< Setup a translation matrix

		static Mat3x3 RotationX(const float _a);						///< Calculate rotation around the x axis
		static Mat3x3 RotationY(const float _a);						///< Calculate rotation around the y axis
		static Mat3x3 RotationZ(const float _a);						///< Calculate rotation around the z axis
		static Mat3x3 Rotation(float _a, float _b, float _c);			///< Rotate around all three axis. This is the same as RotationZ(_c) * RotationX(_b) * RotationY(_a)
		static Mat3x3 Rotation(const Vec3& _angles) {return Rotation(_angles[0], _angles[1], _angles[2]);}	///< Rotate around all three axis. This is the same as RotationZ(z) * RotationX(x) * RotationY(y)
		static Mat3x3 Rotation(const Vec3& _axis, float _angle);		///< Rotate around an arbitrary axis
		static Mat3x3 Rotation(const Quaternion& _rotation);			///< Matrix from quaternion

		static Mat3x3 Axis(const Vec3& _xAxis, const Vec3& _yAxis, const Vec3& _zAxis);		///< Setup an axis matrix - a vector base (columnwise)
		const Vec3& XAxis() const { return *(Vec3*)(m_data+0); }
		const Vec3& YAxis() const { return *(Vec3*)(m_data+3); }
		const Vec3& ZAxis() const { return *(Vec3*)(m_data+6); }

		static Mat3x3 Orthonormal(const Vec3& _normal);					///< Creates an orthonormal base for a direction vector

		/// \brief Compute the inverse with determinant method
		Mat3x3 Inverse() const;

		/// \brief Calculate the determinant of the whole 3x3 matrix with
		///		Laplace's formula
		float Det() const;
	};

	/// \brief Computes _v * _A.
	Vec3 operator * (const Vec3& _v, const Mat3x3& _A);
	/// \brief Computes _A * _v.
	Vec3 operator * (const Mat3x3& _A, const Vec3& _v);

}; // namespace Math