#include "camera.hpp"
#include "voxel/model.hpp"
#include "graphic/core/uniformbuffer.hpp"

using namespace ei;
using namespace Math;

namespace Input {

	// ********************************************************************* //
	Camera::Camera( const Math::FixVec3& _position, const ei::Quaternion& _rotation,
				float _fov, float _aspect ) :
		m_transformation( _position, _rotation ),
		m_fov( _fov ),
		m_aspect( _aspect ),
		m_worldRotation( _rotation ),
		m_attachMode( REFERENCE_ONLY ),
		m_phi(0.0f),
		m_theta(0.0f),
		m_nearPlane(5.0f),
		m_farPlane(50000.0f)
	{
		UpdateMatrices();
	}

	// ********************************************************************* //
	void Camera::Set( Graphic::UniformBuffer& _cameraUBO )
	{
		_cameraUBO["CameraR0"] = transpose(m_transformation.GetRotationMatrix()(0));
		_cameraUBO["CameraR1"] = transpose(m_transformation.GetRotationMatrix()(1));
		_cameraUBO["CameraR2"] = transpose(m_transformation.GetRotationMatrix()(2));
		_cameraUBO["Projection"] = Vec4(GetProjection()(0,0), GetProjection()(1,1), GetProjection()(2,2), GetProjection()(2,3));
		_cameraUBO["ProjectionInverse"] = m_inverseProjection;
		_cameraUBO["NearPlane"] = m_nearPlane;
		_cameraUBO["FarPlane"] = m_farPlane;
		_cameraUBO["CameraPosition"] = Vec3(m_transformation.GetPosition());
	}

	// ********************************************************************* //
	ei::Vec3 Camera::GetReferencePosition() const
	{
		if( m_attachMode != REFERENCE_ONLY ) return m_transformation.GetInverseRotationMatrix() * m_referencePos;
		return ei::Vec3(m_attachedTo->GetPosition() - m_transformation.GetPosition());
	}

	// ********************************************************************* //
	void Camera::UpdateMatrices()
	{
		m_worldRotation = ei::Quaternion( 0.0f, -m_phi, 0.0f ) * ei::Quaternion( -m_theta, 0.0f, 0.0f );
		if( m_attachMode != REFERENCE_ONLY ) {
			NormalizeReference();
		}
		m_projection = ei::perspectiveGL( m_fov, m_aspect, m_nearPlane, m_farPlane );
		// construct explicit invert-vector
		m_inverseProjection = Vec4(1.0f/GetProjection()(0,0), 1.0f/GetProjection()(1,1), 1.0f/GetProjection()(2,2), -GetProjection()(2,3) / GetProjection()(2,2));

		// Compute frustum planes from viewProjection
		// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
		m_frustum[0] = transpose(m_projection(3) + m_projection(0));
		m_frustum[1] = transpose(m_projection(3) - m_projection(0));
		m_frustum[2] = transpose(m_projection(3) + m_projection(1));
		m_frustum[3] = transpose(m_projection(3) - m_projection(1));
		m_frustum[4] = transpose(m_projection(3) + m_projection(2));
		m_frustum[5] = transpose(m_projection(3) - m_projection(2));
	}


	// ********************************************************************* //
	void Camera::Rotate( float _theta, float _phi )
	{
		m_phi += _phi;
		m_theta += _theta;
		if( m_attachedTo ) {
			// Do an object relative rotation.
			// In soft case the reference point is recomputed since we don't
			// want to jump back but center movement at the object.
			if( m_attachMode == REFERENCE_ONLY )
				m_referencePos = m_transformation.Transform( m_attachedTo->GetPosition() );
			// Additionally the rotation is either relative to the world or to the object.
			m_worldRotation = ei::Quaternion( 0.0f, -m_phi, 0.0f ) * ei::Quaternion( -m_theta, 0.0f, 0.0f );
			if( m_attachMode != FOLLOW_AND_ROTATE )
			{
				m_transformation.SetRotation( m_worldRotation );
			}
			NormalizeReference();
		} else {
			//m_worldRotation *= ~Math::Quaternion( _theta, _phi, 0.0f );
			m_worldRotation = ei::Quaternion( 0.0f, -m_phi, 0.0f ) * ei::Quaternion( -m_theta, 0.0f, 0.0f );
			m_transformation.SetRotation( m_worldRotation );
		}
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
		m_transformation.Translate( transpose(m_transformation.GetRotationMatrix()(1) * _dy - m_transformation.GetRotationMatrix()(0) * _dx) );
		// Make soft attachment
		m_attachMode = REFERENCE_ONLY;
	}

	// ********************************************************************* //
	void Camera::Scroll( float _dz )
	{
		if( m_attachedTo )
		{
			// Increasing scroll speed with increasing distance.
			_dz = m_referencePos[2] * _dz * 0.01f;
			// Avoid scrolling into the object
			_dz = ei::min( _dz, -m_attachedTo->GetRadius() * 0.75f + m_referencePos[2] );
		} // Linear movement otherwise

		m_transformation.Translate( transpose(m_transformation.GetRotationMatrix()(2) * _dz) );
		m_referencePos[2] -= _dz;
	}

	// ********************************************************************* //
	void Camera::ZoomAt( const Voxel::Model& _model, AttachMode _mode )
	{
		// Rotate camera back to the world space if necessary
		if( m_attachedTo && _mode == FOLLOW_AND_ROTATE )
		{
			// TODO: Get model theta and phi to transform the relative rotation
			// to an absolute one.
		}

		m_attachedTo = &_model;
		m_attachMode = _mode;
		m_referencePos[0] = 0.0f;
		m_referencePos[1] = 0.0f;
		// Compute required distance to the model
		m_referencePos[2] = 2.0f * _model.GetRadius() / m_fov;

		NormalizeReference();
	}

	// ********************************************************************* //
	void Camera::Attach( const Voxel::Model* _model, AttachMode _mode )
	{
		// Rotate camera back to the world space if necessary
		if( m_attachedTo && _mode == FOLLOW_AND_ROTATE )
		{
			// TODO: Get model theta and phi to transform the relative rotation
			// to an absolute one.
		}

		m_attachedTo = _model;
		m_attachMode = _mode;
		// Compute actual reference frame.
		m_referencePos = m_transformation.Transform( _model->GetPosition() );

		NormalizeReference();
	}

	// ********************************************************************* //
	void Camera::NormalizeReference()
	{
		// Object might be rotated -> camera must follow to look on the same side
		if( m_attachMode == FOLLOW_AND_ROTATE )
		{
			m_transformation.SetRotation( m_worldRotation * ~m_attachedTo->GetRotation() );
		}
		// Transform by rotation inverse (which is multiplying from right for
		// rotations)
		m_transformation.SetPosition( m_attachedTo->GetPosition() - FixVec3(m_transformation.GetInverseRotationMatrix() * m_referencePos) );
	}

	// ********************************************************************* //
	bool Camera::IsVisible( const ei::Sphere& _S ) const
	{
		// TODO: potential optimization: Dot-Product-Culling

		// Check against all 6 planes. If the sphere is outside any half space
		// it is outside of the whole volume.
		for( int i=0; i<6; ++i )
		{
			if( m_frustum[i].DotCoords( _S.center ) <= -_S.radius )
				return false;
		}

		return true;
	}

	// ********************************************************************* //
	WorldRay Camera::GetRay(const ei::Vec2& _screenSpaceCoordinate) const
	{
		WorldRay ray;
		// Compute view space position of a point on the near plane
		Vec3 nearPoint = ei::Vec3( m_inverseProjection[0] * _screenSpaceCoordinate[0],
							   m_inverseProjection[1] * _screenSpaceCoordinate[1],
							   1.0f );	// 1.0 result of inverse project of any coordinate
										// Division by 
		ray.origin = m_transformation.TransformInverse(nearPoint);
		ray.direction = normalize(m_transformation.GetInverseRotationMatrix() * nearPoint);
		return ray;
	}

} // namespace Input