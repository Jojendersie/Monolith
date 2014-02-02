#pragma once

#include "vector.hpp"

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

} // namespace Math