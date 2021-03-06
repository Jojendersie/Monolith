#pragma once

#include "math/transformation.hpp"
#include "math/plane.hpp"
namespace Voxel { class Model; }

#include <mutex>


namespace Input {

	/// \brief A free camera in 3D space.
	class Camera
	{
	public:
		enum AttachMode
		{
			REFERENCE_ONLY,			///< The rotation center is at the objects position.
			FOLLOW,					///< Rotation center at object and relative position is hold.
			FOLLOW_AND_ROTATE,		///< Rotation center at object, relative position is hold and camera is rotated relative to the object rotation.
		};

		/// \brief Create a new free perspective camera.
		/// \param [in] _position Initial position.
		/// \param [in] _rotation Initial rotation.
		/// \param [in] _fov Field of view in Y-direction in radiants.
		/// \param [in] _aspect View port width/height.
		Camera( const Math::FixVec3& _position, const ei::Quaternion& _rotation,
				float _fov, float _aspect );

		/// \brief Returns the x-axis of the camera from the current matrix
		//Math::Vec3 XAxis()	{ return Math::Vec3(m_view.m11, m_view.m21, m_view.m31); }
		/// \brief Returns the y-axis of the camera from the current matrix
		//Math::Vec3 YAxis()	{ return Math::Vec3(m_view.m12, m_view.m22, m_view.m32); }
		/// \brief Returns the z-axis of the camera from the current matrix.
		/// \details The z-axis is the view direction!
		//Math::Vec3 ZAxis()	{ return Math::Vec3(m_view.m13, m_view.m23, m_view.m33); }

		/// \brief Set all matrices, and further information in the camera uniform buffer.
		void Set( Graphic::UniformBuffer& _cameraUBO );

		/// \brief Set the cameras position relative to the attached object.
		void SetRelativePosition(const ei::Vec3& _pos);

		/// \brief Move in camera space.
		void Move( float _dx, float _dy );

		/// \brief Zoom in and out to the reference point.
		/// \details This effectively moves the camera in view direction.
		///
		///		Scrolling keeps the attachment type untouched.
		void Scroll( float _dz );

		/// \bief Do a relative rotation (object space or camera space).
		/// \param [in] _theta Rotation around y-axis
		/// \param [in] _phi Rotation towards/away y-axis.
		void Rotate( float _theta, float _phi );

		/// \brief Keeps the rotation and resets the position such that the
		///		given model should fit to the screen.
		/// \details The given model is tracked afterwards.
		/// \param [in] _model The model to zoom at. It is also tracked afterwards.
		void ZoomAt( const Voxel::Model& _model, AttachMode _mode );

		/// \brief Attach or detach any object.
		/// \details If the camera is not attached movements are done in camera-XZ
		///		and rotations around the camera position.
		void Attach( const Voxel::Model* _model, AttachMode _mode );

		/// \brief Get the current movement reference model.
		const Voxel::Model* GetAttachedModel() const	{ return m_attachedTo; }

		/// \brief What is the position of the attached model in the camera system.
		ei::Vec3 GetReferencePosition() const;

		/// \brief Recompute all the matrices.
		/// \details Standard input does not change one of the matrices
		///		(multi threading). This is the only function which will change
		///		them.
		void UpdateMatrices();
		const Math::Transformation& Transformation() const { return m_transformation; }

		const ei::Mat4x4& GetProjection() const			{ return m_projection; }			///< Return projection matrix
		//const ei::Mat4x4& GetRotation() const				{ return m_rotationMatrix; }		///< Return inverse (view * projection) matrix

		/// \brief Checks a sphere against the frustum and returns true if any
		///		point of the sphere is inside.
		bool IsVisible( const ei::Sphere& _S ) const;

		/// \brief Determines the ray starting at camera's near plane in world space.
		Math::WorldRay GetRay(const ei::Vec2& _screenSpaceCoordinate) const;

		/// \brief Get the angle (radiant) of the camera rotation around Y-axis.
		float GetYRotation() const { return m_phi; }
		/// \brief Get the angle (radiant) of the camera rotation around horizontal-axis.
		float GetXRotation() const { return m_theta; }
	private:
		// Computed matrices used in rendering
		ei::Mat4x4 m_projection;
		Math::Transformation m_transformation;
		ei::Vec4 m_inverseProjection;		///< A vector to invert projection calculations: (1/p(0,0), 1/p(1,1), 1/p(2,2), -p(3,2)/p(2,2); Usage: pos.xyz * invProj.xyz + vec3(0,0,invProj.w)
		Math::Plane m_frustum[6];			///< Left, Right, Bottom, Top, Near, Far all showing inwards in view space

		float m_fov;						///< Field of view in Y-direction in radiants.
		float m_aspect;

		float m_nearPlane;
		float m_farPlane;

		float m_phi;
		float m_theta;

		const Voxel::Model* m_attachedTo;	///< The camera can track this object and uses it as reference system
		ei::Vec3 m_referencePos;			///< Object position in view space. This reference frame is kept for a tracked object.
		ei::Quaternion m_worldRotation;		///< World space rotation of the camera. This is equal to the rotation if not attached with FOLLOW_AND_ROTATE.
		AttachMode m_attachMode;			///< If the object is hard attached the camera position follows the object. If it is soft attached rotations are relative to the object.

		/// \brief Reset camera position such that the original object
		///		reference position is restored.
		void NormalizeReference();
	};
} // namespace Input