#pragma once

#include "vector3.hpp"
#include <list>

namespace Math {

	struct Sphere
	{
		Vec3 m_center;		///< Center of the sphere
		float m_radius;		///< Radius of the sphere
		float m_radiusSqr;	///< Squared radius of the sphere

		Sphere( const Vec3& _center, float _r=0.0f ) :
			m_center(_center),
			m_radius(_r),
			m_radiusSqr(_r*_r)
		{}

		/// \brief Construct an optimal bounding sphere from two points
		Sphere( const Vec3& _p1, const Vec3& _p2 );

		/// \brief Construct an optimal bounding sphere from three points
		/// \details This is no construction of a sphere with all three points
		///		on the surface!
		Sphere( const Vec3& _p1, const Vec3& _p2, const Vec3& _p3 );

		/// \brief Construct an optimal bounding sphere from four points
		/// \details This is no construction of a sphere with all four points
		///		on the surface!
		Sphere( const Vec3& _p1, const Vec3& _p2, const Vec3& _p3, const Vec3& _p4 );

		/// \brief Checks if a point lies inside the sphere.
		/// \details A point on the boundary is counted as inside to.
		bool Encloses( const Vec3& _p )
		{
			return (m_center - _p).LengthSq() <= m_radiusSqr;
		}
	};

	/// \brief Welzl's algorithm to find the optimal bounding sphere for a set
	///		of points.
	/// \param [in*] _pointSet A list of all points. The order of the points
	///		changes during executation.
	Sphere MinimalBoundingSphere( std::list<Vec3>& _pointSet );

} // namespace Math