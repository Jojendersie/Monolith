#pragma once

#include "fixedpoint.hpp"
#include "quaternion.hpp"
#include "matrix.hpp"

namespace Math {

	/// \brief A Rotate * Translation rigid transformation object with large
	///		coordinate support.
	class Transformation
	{
	public:
		Transformation() : m_x(0ll), m_y(0ll), m_z(0ll) {}
		Transformation(Fix _x, Fix _y, Fix _z, const Quaternion& _rotation) : m_x(_x), m_y(_y), m_z(_z), m_rotation(_rotation)	{}

		/// \brief Move the object.
		void translate( const Vec3& _vector )	{ m_x += Fix(_vector[0]); m_y += Fix(_vector[1]); m_z += Fix(_vector[2]); }

		void rotate( const Quaternion& _rot )					{ m_rotation *= _rot; }
		void rotate( float _yaw, float _pitch, float _roll )	{ m_rotation *= Quaternion( _yaw, _pitch, _roll ); }
		void rotate( const Vec3& _axis, float _angle )			{ m_rotation *= Quaternion( _axis, _angle ); }

		/// \brief Get an approximated floating point transformation.
		Mat4x4 getTransformation();

		/// \brief Get a transformation matrix for this object in the
		///		reference system.
		Mat4x4 getTransformation(const Transformation& _reference);

		/// \brief Get the distance between two transformed points in space.
		float distance(const Transformation& _object);

		/// \brief Get the squared distance between two transformed points in space.
		float distanceSq(const Transformation& _object);
	private:
		Fix m_x;				///< Position x coordinate
		Fix m_y;				///< Position y coordinate
		Fix m_z;				///< Position z coordinate
		Quaternion m_rotation;	///< Standard rotation component
	};

} // namespace Math