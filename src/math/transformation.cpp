#include "transformation.hpp"

namespace Math {

	// Get an approximated floating point transformation.
	Mat4x4 Transformation::getTransformation()
	{
		return Mat4x4::Rotation(m_rotation) * Mat4x4::Translation((float)m_x, (float)m_y, (float)m_z);
	}

	// Get a transformation matrix for this object in the reference system.
	Mat4x4 Transformation::getTransformation(const Transformation& _reference)
	{
		return Mat4x4::Rotation(m_rotation)
			* Mat4x4::Translation(float(m_x-_reference.m_x), float(m_y-_reference.m_y), float(m_z-_reference.m_z))
			* Mat4x4::Rotation(~_reference.m_rotation);
	}

	// Get the distance between two transformed points in space.
	float Transformation::distance(const Transformation& _object)
	{
		return (float)sqrt((m_x-_object.m_x) * (m_x-_object.m_x)
			             + (m_y-_object.m_y) * (m_y-_object.m_y)
			             + (m_z-_object.m_z) * (m_z-_object.m_z));
	}

	// Get the squared distance between two transformed points in space.
	float Transformation::distanceSq(const Transformation& _object)
	{
		return float((m_x-_object.m_x) * (m_x-_object.m_x)
			       + (m_y-_object.m_y) * (m_y-_object.m_y)
			       + (m_z-_object.m_z) * (m_z-_object.m_z));
	}

} // namespace Math