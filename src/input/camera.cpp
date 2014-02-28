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
		m_projection = Math::Mat4x4::Projection( m_fov, m_aspect, 5.0f, 50000.0f );
		m_mutex.unlock();
		m_viewProjection = m_view * m_projection;
		// TODO: construct inverse explicit
		m_inverseView = m_view.Inverse();
		m_inverseViewProjection = m_viewProjection.Inverse();

		// Compute frustum planes from viewProjection
		// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
		m_frustum[0] = m_viewProjection.Column(3) + m_viewProjection.Column(0);
		m_frustum[1] = m_viewProjection.Column(3) - m_viewProjection.Column(0);
		m_frustum[2] = m_viewProjection.Column(3) + m_viewProjection.Column(1);
		m_frustum[3] = m_viewProjection.Column(3) - m_viewProjection.Column(1);
		m_frustum[4] = m_viewProjection.Column(3) + m_viewProjection.Column(2);
		m_frustum[5] = m_viewProjection.Column(3) - m_viewProjection.Column(2);
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
			_dx *= m_referencePos[2] * m_fov;
			_dy *= m_referencePos[2] * m_fov;
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
			_dz = m_referencePos[2] * _dz * 0.01f;
			// Avoid scrolling into the object
			_dz = Math::max( _dz, m_attachedTo->GetRadius() * 0.75f - m_referencePos[2] );
		} // Linear movement otherwise

		m_position -= m_rotation.ZAxis() * _dz;
		m_referencePos[2] += _dz;
	}

	// ********************************************************************* //
	void Camera::ZoomAt( const Voxel::Model& _model )
	{
		m_attachedTo = &_model;
		m_hardAttached = true;
		m_referencePos[0] = 0.0f;
		m_referencePos[1] = 0.0f;
		// Compute required distance to the model
		m_referencePos[2] = 2.0f * _model.GetRadius() / m_fov;

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

	// ********************************************************************* //
	bool Camera::IsVisible( const Math::Sphere& _S ) const
	{
		// TODO: potential optimization: Dot-Product-Culling

		// Check against all 6 planes. If the sphere is outside any half space
		// it is outside of the whole volume.
		for( int i=0; i<6; ++i )
		{
			if( m_frustum[i].DotCoords( _S.m_center ) <= -_S.m_radius )
				return false;
		}

		return true;
	}

	// ********************************************************************* //
	Ray Camera::GetRay(const Vec2& _screenSpaceCoordinate) const
	{
		Vec4 nearPoint = Vec4(_screenSpaceCoordinate, -1.0f, 1.0f) * m_inverseViewProjection;
		Vec3 start = Vec3(nearPoint) / nearPoint[3];
		return Ray( start, normalize(start - m_position) );
	}

} // namespace Input