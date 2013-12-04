#pragma once

#include "chunk.hpp"
#include "../math/math.hpp"

namespace Voxel {

	/// \brief A model is a high level abstraction with graphics and
	///		game play elements.
	class Model
	{
	public:
		/// \brief Constructs an empty model without any chunk
		Model();
		~Model();

		/// \brief Draw a full model.
		/// \details This method sets the texture map???
		///
		///		In the first step the culling for the whole model is done
		///		and then each chunk is rendered.
		///
		///		The effect must be set outside.
		/// \param [out] _objectConstants A reference to the constant buffer
		///		which must be filled.
		/// \param [in] _viewProjection The actual view projection matrix. TODO: camera mit culling
		void Draw( Graphic::UniformBuffer& _objectConstants, const Math::Mat4x4& _viewProjection );

		/// \brief Set a voxel in the model and update mass properties.
		/// \details This method overwrites all covered voxels.
		/// \param [in] _position Position inside the given level.
		/// \param [in] _level Depth in the grid hierarchy. 0 is the maximum
		///		size and 5 is the level of the smallest voxel grid.
		/// \param [in] _type Material to be set. VoxelType::NONE can delete
		///		a voxel.
		void Set( const Math::IVec3& _position, int _level, VoxelType _type );

		/// \brief Get the center of gravity (mass center)
		Math::Vec3 GetCenter() const		{ return m_center + m_position; }
		/// \brief Get the bounding radius of the sphere centered at the center
		///		of gravity.
		float GetRadius() const { return m_boundingSphereRadius; }

		friend bool CollisionTest(Math::Vec3&, Math::Vec3&, const Model, const Model);
		friend void CollisionCompute(Math::Vec3&, Math::Quaternion&, Math::Vec3&, Math::Quaternion&,
			Math::Vec3&, Math::Quaternion&, Math::Vec3&, Math::Quaternion&,
			const Model, const Model,const Math::Vec3, const Math::Vec3);

	protected:
		Chunk** m_chunks;				///< A loose collection of chunks. TODO: octree
		int m_numChunks;

		Math::Vec3 m_position;			///< Model position in the space.
		Math::Vec3 m_center;			///< The center of gravity (relative to the model).
		Math::Quaternion m_rotation;	///< The rotation around the center.

		Math::Quaternion m_rotatoryVelocity;	///< Current change of rotation per second
		Math::Vec3 m_velocity;			///< Velocity in m/s (vector length)
		float m_mass;					///< Mass (inertia) of the full model
		Math::Mat3 m_inertiaMomentum;		///< Inertia of rotation for the full model.

		float m_boundingSphereRadius;	///< Bounding sphere radius (to the center of gravity) for culling etc.
		Math::Polyhedron m_boundingBox;		///< Bounding Box for the model 
		//ComputeBounding
	};
	/// \brief Check if the models collide.
	/// \param [out] _collisionPoint where the Collision happens
	/// \param [out] _collisionNormal in which direction the Forces work
	/// \param [in] _modelOne against which the Collision Test is done
	/// \param [in] _modelTwo against which the Collision Test is done
	bool CollisionTest(Math::Vec3& _collisionPoint, Math::Vec3& _collisionNormal, const Model _modelOne, const Model _modelTwo);
	/// \brief Calculate a physical Collision for both the elastic and plastic case
	/// \param [out] _elastic^+ return values for the elastic case
	/// \param [out] _plastic^+ return values for the plastic case
	/// \param [in] _modelOne against which the Collision Test is done
	/// \param [in] _modelTwo against which the Collision Test is done
	/// \param [in] _collisionPoint Position where the contact has happened
	/// \param [in] _collisionNormal Direction in which Forces can be applied
	void CollisionCompute(Math::Vec3& _elasticVelocityOne, Math::Quaternion& _elasticRotaryOne, Math::Vec3& _elasticVelocityTwo, Math::Quaternion& _elasticRotaryTwo,
		Math::Vec3& _plasticVelocityOne, Math::Quaternion& _plasticRotaryOne, Math::Vec3& _plasticVelocityTwo, Math::Quaternion& _plasticRotaryTwo,
		const Model _modelOne, const Model _modelTwo, const Math::Vec3 _collisionPoint, const Math::Vec3 _collisionNormal);
};