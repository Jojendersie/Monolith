#pragma once

#include "vector.hpp"
#include "matrix.hpp"

namespace Math {

	struct Ray
	{
		Vec3 m_start;			///< Start of the ray
		Vec3 m_direction;		///< Non normalized direction
		Vec3 m_directionInv;	///< Precomputed (1 / direction) vector for intersection tests.

		/// \brief Create a ray from start and direction
		/// \param [in] _start Start point
		/// \param [in] _direction Non normalized direction vector.
		Ray( const Vec3& _start, const Vec3& _direction ) :
			m_start(_start),
			m_direction(_direction),
			m_directionInv(1.0f / _direction)
		{}

	};

	/// \brief Transforms a ray (position + direction).
	inline Ray operator * (const Ray& _ray, const Mat4x4& _A)
	{
		return Ray( _ray.m_start * _A, _A.TransformDirection( _ray.m_direction ) );
	}

} // namespace Math