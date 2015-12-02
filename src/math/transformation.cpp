#include "transformation.hpp"
#include <ei/vector.hpp>
#include "input/camera.hpp"

using namespace ei;

namespace Math {

	// ********************************************************************* //
	void Transformation::SetRotation( const ei::Quaternion& _rotation )
	{
		m_rotation = _rotation;
		UpdateMatrices();
	}

	// ********************************************************************* //
	// Get an approximated floating point transformation.
	Mat4x4 Transformation::GetTransformation() const
	{
		return translation(Vec3((float)m_position[0], (float)m_position[1], (float)m_position[2])) * rotationH(m_rotation);
	}

	// ********************************************************************* //
	// Get a transformation matrix for this object in the reference system.
	Mat4x4 Transformation::GetTransformation(const Transformation& _reference) const
	{
		return rotationH(_reference.m_rotation)
			* translation(Vec3(float(m_position[0]-_reference.m_position[0]), float(m_position[1]-_reference.m_position[1]), float(m_position[2]-_reference.m_position[2])))
			* rotationH(m_rotation);
	}

	// ********************************************************************* //
	// Transform a position into the space of this transformation approximated.
	Vec3 Transformation::Transform( const FixVec3& _position ) const
	{
		Vec3 pos(float(_position[0] - m_position[0]),
			float(_position[1] - m_position[1]),
			float(_position[2] - m_position[2]) );
		return m_rotationMatrix * pos;
	}

	// ********************************************************************* //
	// Transform a relative position back to the global world space.
	FixVec3 Transformation::TransformInverse( const Vec3& _position ) const
	{
		Vec3 pos = m_inverseRotationMatrix * _position;
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

	// ********************************************************************* //
	void Transformation::UpdateMatrices()
	{
		m_rotation = normalize(m_rotation);
		m_rotationMatrix = rotation(m_rotation);
		m_inverseRotationMatrix = transpose(m_rotationMatrix);
	}

} // namespace Math