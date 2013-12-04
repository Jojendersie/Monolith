#include "camera.hpp"
#include "../voxel/model.hpp"

using namespace Math;

namespace Input {

	// ********************************************************************* //
	Camera::Camera( const Math::Vec3& _position, const Math::Quaternion& _rotation,
				float _fov, float _aspect ) :
		m_position( _position ),
		m_rotation( _rotation ),
		m_fov( _fov ),
		m_aspect( _aspect )
	{
		UpdateMatrices();
	}

	// ********************************************************************* //
	void Camera::UpdateMatrices()
	{
		m_mutex.lock();
//		if( m_hardAttached ) NormalizeReference();
		m_view = Math::Mat4x4::Translation( -m_position ) * Math::Mat4x4::Rotation( m_rotation );
		m_projection = Math::Mat4x4::Projection( m_fov, m_aspect, 1.0f, 1000.0f );
		m_mutex.unlock();
		m_viewProjection = m_view * m_projection;
		m_inverseView = m_view.Invert();
		m_inverseViewProjection = m_viewProjection.Invert();
	}


	// ********************************************************************* //
	void Camera::Rotate( float _theta, float _phi )
	{
		m_mutex.lock();
		if( m_attachedTo ) {
			// Do an object relative rotation.
			// In soft case the reference point is recomputed since we don't
			// want to jump back but center movement at the object
			if( !m_hardAttached ) m_referencePos = m_view.Transform( m_attachedTo->GetCenter() );
			m_rotation = m_rotation * Math::Quaternion( -_theta, _phi, 0.0f );
			NormalizeReference();
		} else
			m_rotation = m_rotation * Math::Quaternion( _theta, -_phi, 0.0f );
		m_mutex.unlock();
	}

	// ********************************************************************* //
	void Camera::Move( float _dx, float _dy )
	{
		// Scale _dy and _dx depending on object distance
		if( m_attachedTo )
		{
			_dx *= m_referencePos.z * m_fov;
			_dy *= m_referencePos.z * m_fov;
		}
		// Compute actual xy directions in camera space
		m_position += m_rotation.YAxis() * _dy - m_rotation.XAxis() * _dx;
		// Make soft attachment
		m_hardAttached = false;
	}

	// ********************************************************************* //
	void Camera::Scroll( float _dz )
	{
		if( m_attachedTo )
		{
			// Increasing scroll speed with increasing distance.
			_dz = m_referencePos.z * _dz * 0.01f;
			// TODO: avoid scrolling into the object
		} // Linear movement otherwise

		m_position -= m_rotation.ZAxis() * _dz;
		m_referencePos.z += _dz;
	}

	// ********************************************************************* //
	void Camera::ZoomAt( const Voxel::Model& _model )
	{
		m_attachedTo = &_model;
		m_hardAttached = true;
		m_referencePos.x = 0.0f;
		m_referencePos.y = 0.0f;
		// Compute required distance to the model
		m_referencePos.z = _model.GetRadius() / m_fov;

		m_mutex.lock();
		NormalizeReference();
		m_mutex.unlock();
	}

	// ********************************************************************* //
	void Camera::Attach( const Voxel::Model* _model )
	{
		m_attachedTo = _model;
		// Compute actual reference frame.
		m_referencePos = _model->GetCenter() * m_view;
	}

	// ********************************************************************* //
	void Camera::NormalizeReference()
	{
		// Transform by rotation inverse (which is multiplying from right for
		// rotations)
		Math::Mat4x4 mCurrentView = Mat4x4::Rotation( m_rotation );
		m_position = m_attachedTo->GetCenter() - mCurrentView * m_referencePos;
	}

} // namespace Input