#pragma once

#include "vector.hpp"
#include "ray.hpp"
#include "sphere.hpp"

namespace Math {
namespace Intersect {

	enum struct Side {
		LEFT   = 0,
		RIGHT  = 1,
		BOTTOM = 2,
		TOP    = 3,
		FRONT  = 4,
		BACK   = 5
	};

	/// \brief Test a grid aligned box against a ray.
	/// \param [in] _ray The ray.
	/// \param [in] _min Minimum corner of the cube.
	/// \param [in] _edgeLength The edge length of the cube. Must be positive.
	/// \param [in,out] _t Threshold for maximal hit distance and output of
	///		the first point of the ray which intersects with the box if there
	///		is a collision.
	///	\return true if the ray start inside the box hits it from outside.
	///		false if ray and box have no point in common.
	bool RayAACube( const Math::Ray& _ray, const Math::IVec3& _min, int _edgeLength, float& _t );

	/// \brief Test a grid aligned box against a ray and finds the side of collision.
	/// \param [in] _ray The ray.
	/// \param [in] _min Minimum corner of the cube.
	/// \param [in] _edgeLength The edge length of the cube. Must be positive.
	/// \param [out] _t First point of the ray which intersects with the box
	///		if there was a collision.
	///	\param [out] _side The side which is hit. This might be in negative
	///		direction of the ray, if it starts inside.
	///	\return true if the ray start inside the box hits it from outside.
	///		false if ray and box have no point in common.
	bool RayAACube( const Math::Ray& _ray, const Math::IVec3& _min, int _edgeLength, float& _t, Side&  _side );

	/// \brief Find the closest point on the ray to the sphere center.
	/// \param [in] _ray The ray.
	/// \param [in] _ray The sphere.
	/// \param [out] _t The ray parameter such that ray.start + t * ray.direction is the 
	/// \return true if there is any intersection between ray and sphere.
	bool RaySphere( const Math::Ray& _ray, const Math::Sphere& _sphere, float& _t );
}
} // namespace Math