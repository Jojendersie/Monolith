#include "ray.hpp"
#include "transformation.hpp"

namespace Math {

// ************************************************************************* //
// Create relative ray from world space ray
Ray::Ray( const WorldRay& _ray, const Transformation& _reference )
{
	Mat3x3 rotation = Mat3x3::Rotation(_reference.GetRotation());
	// Get a relative start position
	m_origin = Vec3(float(_ray.origin[0] - _reference.GetPosition()[0]),
		float(_ray.origin[1] - _reference.GetPosition()[1]),
		float(_ray.origin[2] - _reference.GetPosition()[2]) );
	m_origin = rotation * m_origin;

	// Rotate direction into relative system
	m_direction = rotation * _ray.direction;

	// Normalize and precompute inverse
	float len = length(m_direction);
	m_direction = m_direction / len;
	m_directionInv = len / m_direction;
}

} // namespace Math