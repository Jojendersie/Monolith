#pragma once

#include "predeclarations.hpp"
#include <ei/vector.hpp>
#include "math/fixedpoint.hpp"

namespace Math {

	/// \brief A Rotate * Translation rigid transformation object with large
	///		coordinate support.
	class Transformation
	{
	public:
		Transformation() : m_position(Fix(0.0)), m_rotation(0.0f, 0.0f, 0.0f) {}
		Transformation(Fix _x, Fix _y, Fix _z, const ei::Quaternion& _rotation) : m_position(_x, _y, _z), m_rotation(_rotation)	{ UpdateMatrices(); }
		Transformation(const FixVec3& _position, const ei::Quaternion& _rotation) : m_position(_position), m_rotation(_rotation)	{ UpdateMatrices(); }

		/// \brief Set a total new position.
		void SetPosition( const Math::FixVec3& _position )	{ m_position = _position; }
		/// \brief Read position
		const Math::FixVec3& GetPosition() const			{ return m_position; }
		/// \brief Override rotation
		void SetRotation( const ei::Quaternion& _rotation );
		/// \brief Read rotation
		const ei::Quaternion& GetRotation() const			{ return m_rotation; }
		const ei::Mat3x3& GetRotationMatrix() const			{ return m_rotationMatrix; }
		const ei::Mat3x3& GetInverseRotationMatrix() const	{ return m_inverseRotationMatrix; }

		/// \brief Move the object.
		void Translate( const ei::Vec3& _vector )				{ m_position[0] += Fix(_vector[0]); m_position[1] += Fix(_vector[1]); m_position[2] += Fix(_vector[2]); }

		void Rotate( const ei::Quaternion& _rotation )			{ m_rotation *= _rotation; UpdateMatrices(); }
		void Rotate( float _yaw, float _pitch, float _roll )	{ m_rotation *= ei::Quaternion( _yaw, _pitch, _roll ); UpdateMatrices(); }
		void Rotate( const ei::Vec3& _axis, float _angle )		{ m_rotation *= ei::Quaternion( _axis, _angle ); UpdateMatrices(); }

		/// \brief Get an approximated floating point transformation.
		ei::Mat4x4 GetTransformation() const;

		/// \brief Get a transformation matrix for this object in the
		///		reference system.
		ei::Mat4x4 GetTransformation( const Transformation& _reference ) const;

		/// \brief Transform a position into the space of this transformation
		///		approximated.
		ei::Vec3 Transform( const Math::FixVec3& _position ) const;

		/// \brief Transform a relative position back to the global world space.
		Math::FixVec3 TransformInverse( const ei::Vec3& _position ) const;

		/// \brief Get the distance between two transformed points in space.
		float Distance(const Transformation& _object) const;

		/// \brief Get the squared distance between two transformed points in space.
		float DistanceSq(const Transformation& _object) const;
	protected:
		Math::FixVec3 m_position;				///< World-position
		ei::Quaternion m_rotation;				///< Standard rotation component
		ei::Mat3x3 m_rotationMatrix;			///< Same rotation in matrix form (read only)
		ei::Mat3x3 m_inverseRotationMatrix;		///< Inverse rotation matrix (read only)

		// Recompute the two matrices based on the quaternion
		void UpdateMatrices();
	};

} // namespace Math