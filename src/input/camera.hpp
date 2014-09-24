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

		/// \brief Create a new free perspective camera.
		/// \param [in] _position Initial position.
		/// \param [in] _rotation Initial rotation.
		/// \param [in] _fov Field of view in radiance.
		/// \param [in] _aspect View port width/height.
		Camera( const Math::FixVec3& _position, const Math::Quaternion& _rotation,
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
		void ZoomAt( const Voxel::Model& _model );

		/// \brief Attach or detach any object.
		/// \details If the camera is not attached movements are done in camera-XZ
		///		and rotations around the camera position.
		void Attach( const Voxel::Model* _model );

		/// \brief Get the current movement reference model.
		const Voxel::Model* GetAttachedModel() const	{ return m_attachedTo; }

		/// \brief What is the position of the attached model in the camera system.
		const Math::Vec3& GetReferencePosition() const;

		/// \brief Recompute all the matrices.
		/// \details Standard input does not change one of the matrices
		///		(multi threading). This is the only function which will change
		///		them.
		void UpdateMatrices();
		const Math::Transformation& RenderState() const { return m_renderTransformation; }

		const Math::Mat4x4& GetProjection() const			{ return m_projection; }			///< Return projection matrix
		//const Math::Mat4x4& GetRotation() const				{ return m_rotationMatrix; }		///< Return inverse (view * projection) matrix

		/// \brief Checks a sphere against the frustum and returns true if any
		///		point of the sphere is inside.
		bool IsVisible( const Math::Sphere& _S ) const;

		/// \brief Determines the ray starting at camera's near plane in world space.
		Math::WorldRay GetRay(const Math::Vec2& _screenSpaceCoordinate) const;
	private:
		// Computed matrices used in rendering
		Math::Mat4x4 m_projection;
		Math::Transformation m_latestTransformation;
		Math::Transformation m_renderTransformation;
		Math::Vec4 m_inverseProjection;		///< A vector to invert projection calculations: (1/p(0,0), 1/p(1,1), 1/p(2,2), -p(3,2)/p(2,2); Usage: pos.xyz * invProj.xyz + vec3(0,0,invProj.w)
		Math::Mat3x3 m_rotationMatrix;		///< Precomputed matrix from quaternion
		Math::Plane m_frustum[6];			///< Left, Right, Bottom, Top, Near, Far all showing inwards in view space

		std::mutex m_mutex;					///< mutex between all update methods
		float m_fov;
		float m_aspect;

		float m_nearPlane;
		float m_farPlane;

		const Voxel::Model* m_attachedTo;	///< The camera can track this object and uses it as reference system
		Math::Vec3 m_referencePos;			///< Object position in view space. This reference frame is kept for a tracked object.
		bool m_hardAttached;				///< If the object is hard attached the camera position follows the object. If it is soft attached rotations are relative to the object.

		/// \brief Reset camera position such that the original object
		///		reference position is restored.
		void NormalizeReference();
	};
} // namespace Input