#include "intersection.hpp"
#include <assert.h>

namespace Math {
namespace Intersect {

	// ********************************************************************* //
	bool RayAACube( const Math::Ray& _ray, const Math::IVec3& _min, int _edgeLength, float& _t )
	{
		// Box intersection algorithm:
		// http://people.csail.mit.edu/amy/papers/box-jgt.pdf
		// Projection to all planes
		float min, max, tmin, tmax;
		if( _ray.m_direction[0] >= 0 )
		{
			min = (_min[0] - _ray.m_origin[0]) * _ray.m_directionInv[0];
			max = (_min[0] + _edgeLength - _ray.m_origin[0]) * _ray.m_directionInv[0];
		} else {
			max = (_min[0] - _ray.m_origin[0]) * _ray.m_directionInv[0];
			min = (_min[0] + _edgeLength - _ray.m_origin[0]) * _ray.m_directionInv[0];
		}
		if( _ray.m_direction[1] >= 0 )
		{
			tmin = (_min[1] - _ray.m_origin[1]) * _ray.m_directionInv[1];
			tmax = (_min[1] + _edgeLength - _ray.m_origin[1]) * _ray.m_directionInv[1];
		} else {
			tmax = (_min[1] - _ray.m_origin[1]) * _ray.m_directionInv[1];
			tmin = (_min[1] + _edgeLength - _ray.m_origin[1]) * _ray.m_directionInv[1];
		}

		if(	(min > tmax) || (tmin > max) )
			return false;

		if( tmin > min ) min = tmin;
		if( tmax < max ) max = tmax;

		if( _ray.m_direction[2] >= 0 )
		{
			tmin = (_min[2] - _ray.m_origin[2]) * _ray.m_directionInv[2];
			tmax = (_min[2] + _edgeLength - _ray.m_origin[2]) * _ray.m_directionInv[2];
		} else {
			tmax = (_min[2] - _ray.m_origin[2]) * _ray.m_directionInv[2];
			tmin = (_min[2] + _edgeLength - _ray.m_origin[2]) * _ray.m_directionInv[2];
		}

		if(	(min > tmax) || (tmin > max) )
			return false;

		_t = Math::max(Math::max(min, tmin), 0.0f);

		return 0.0f < max && 0.0f < tmax;
	}


	// ********************************************************************* //
	bool RayAACube( const Math::Ray& _ray, const Math::IVec3& _min, int _edgeLength, Side&  _side )
	{
		// Box intersection algorithm:
		// http://people.csail.mit.edu/amy/papers/box-jgt.pdf
		// Projection to all planes
		float min, max, tmin, tmax;
		Side tside;
		if( _ray.m_direction[0] >= 0 )
		{
			min = (_min[0] - _ray.m_origin[0]) * _ray.m_directionInv[0];
			max = (_min[0] + _edgeLength - _ray.m_origin[0]) * _ray.m_directionInv[0];
			_side = Side::LEFT;
		} else {
			max = (_min[0] - _ray.m_origin[0]) * _ray.m_directionInv[0];
			min = (_min[0] + _edgeLength - _ray.m_origin[0]) * _ray.m_directionInv[0];
			_side = Side::RIGHT;
		}
		if( _ray.m_direction[1] >= 0 )
		{
			tmin = (_min[1] - _ray.m_origin[1]) * _ray.m_directionInv[1];
			tmax = (_min[1] + _edgeLength - _ray.m_origin[1]) * _ray.m_directionInv[1];
			tside = Side::BOTTOM;
		} else {
			tmax = (_min[1] - _ray.m_origin[1]) * _ray.m_directionInv[1];
			tmin = (_min[1] + _edgeLength - _ray.m_origin[1]) * _ray.m_directionInv[1];
			tside = Side::TOP;
		}

		if(	(min > tmax) || (tmin > max) )
			return false;

		if( tmin > min ) { min = tmin; _side = tside; }
		if( tmax < max ) max = tmax;

		if( _ray.m_direction[2] >= 0 )
		{
			tmin = (_min[2] - _ray.m_origin[2]) * _ray.m_directionInv[2];
			tmax = (_min[2] + _edgeLength - _ray.m_origin[2]) * _ray.m_directionInv[2];
			tside = Side::FRONT;
		} else {
			tmax = (_min[2] - _ray.m_origin[2]) * _ray.m_directionInv[2];
			tmin = (_min[2] + _edgeLength - _ray.m_origin[2]) * _ray.m_directionInv[2];
			tside = Side::BACK;
		}

		if(	(min > tmax) || (tmin > max) )
			return false;

		if( tmin > min ) _side = tside;

		return 0.0f < max && 0.0f < tmax;
	}


	// ********************************************************************* //
	bool RaySphere( const Math::Ray& _ray, const Math::Sphere& _sphere, float& _t )
	{
		// Calculate projection and hypotenuse - two sides of the right-angled
		// triangle.
		Vec3 p = _sphere.m_center - _ray.m_origin;
		float hypotenuseSq = lengthSq( p );
		_t = dot(p, _ray.m_direction);

		// Compute the third length and compare it to the radius
		return (hypotenuseSq - _t*_t) <= _sphere.m_radiusSqr;
	}

}
} // namespace Math