#include "transformation.hpp"
#include "math/vector.hpp"
#include "input/camera.hpp"

namespace Math {

	// ********************************************************************* //
	// Get an approximated floating point transformation.
	Mat4x4 Transformation::GetTransformation() const
	{
		return Mat4x4::Rotation(m_rotation) * Mat4x4::Translation((float)m_position[0], (float)m_position[1], (float)m_position[2]);
	}

	// ********************************************************************* //
	// Get a transformation matrix for this object in the reference system.
	Mat4x4 Transformation::GetTransformation(const Transformation& _reference) const
	{
		return Mat4x4::Rotation(m_rotation)
			* Mat4x4::Translation(float(m_position[0]-_reference.m_position[0]), float(m_position[1]-_reference.m_position[1]), float(m_position[2]-_reference.m_position[2]))
			* Mat4x4::Rotation(~_reference.m_rotation);
	}

	// ********************************************************************* //
	// Get a transformation matrix for this object in the reference camera system.
	Mat4x4 Transformation::GetTransformation(const Input::Camera& _reference) const
	{
		return Mat4x4::Rotation(m_rotation)
			* Mat4x4::Translation(float(m_position[0]-_reference.m_position[0]), float(m_position[1]-_reference.m_position[1]), float(m_position[2]-_reference.m_position[2]))
			*_reference.GetRotation();
	}

	// ********************************************************************* //
	// Transform a position into the space of this transformation approximated.
	Vec3 Transformation::Transform( const Math::FixVec3& _position ) const
	{
		Math::Vec3 pos(float(_position[0] - m_position[0]),
			float(_position[1] - m_position[1]),
			float(_position[2] - m_position[2]) );
		return pos * Mat3x3::Rotation(m_rotation);
	}

	// ********************************************************************* //
	// Transform a relative position back to the global world space.
	Math::FixVec3 Transformation::TransformInverse( const Vec3& _position ) const
	{
		// TODO: potential optimization: precompute the matrix or use quaternion multiplication
		Vec3 pos = Mat3x3::Rotation(m_rotation) * _position;
		return FixVec3(Fix(pos[0]) + m_position[0], Fix(pos[1]) + m_position[1], Fix(pos[2]) + m_position[2]);
	}

	// ********************************************************************* //
	// Get the distance between two transformed points in space.
	float Transformation::Distance(const Transformation& _object) const
	{
		return (float)sqrt(sq(m_position[0] - _object.m_position[0])
			             + sq(m_position[1] - _object.m_position[1])
			             + sq(m_position[2] - _object.m_position[2]));
	}

	// ********************************************************************* //
	// Get the squared distance between two transformed points in space.
	float Transformation::DistanceSq(const Transformation& _object) const
	{
		return float(sq(m_position[0] - _object.m_position[0])
				   + sq(m_position[1] - _object.m_position[1])
				   + sq(m_position[2] - _object.m_position[2]));
	}

} // namespace Math