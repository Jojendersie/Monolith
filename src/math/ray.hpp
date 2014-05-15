#pragma once

#include "vector.hpp"
#include "matrix.hpp"

namespace Math {

	/// \brief A ray container for world-space ray casting.
	/// \details The WorldRay is a data class only. To do any computation
	///		transform it into the reference system with the \ref Ray
	///		constructor before.
	struct WorldRay
	{
		FixVec3 origin;
		Vec3 direction;
	};
	

	/// \brief A ray class for analytical geometry computations.
	struct Ray
	{
		Vec3 m_origin;			///< Start of the ray
		Vec3 m_direction;		///< Normalized direction
		Vec3 m_directionInv;	///< Precomputed (1 / direction) vector for intersection tests.

		/// \brief Create a ray from start and direction
		/// \param [in] _start Start point
		/// \param [in] _direction Normalized direction vector.
		Ray( const Vec3& _start, const Vec3& _direction ) :
			m_origin(_start)
		{
			float len = length(_direction);
			m_direction = _direction / len;
			m_directionInv = len / _direction;
		}

		/// \brief Create relative ray from world space ray
		Ray( const WorldRay& _ray, const Transformation& _reference );
	};

	/// \brief Transforms a ray (position + direction).
	inline Ray operator * (const Ray& _ray, const Mat4x4& _A)
	{
		return Ray( _ray.m_origin * _A, _A.TransformDirection( _ray.m_direction ) );
	}

} // namespace Math