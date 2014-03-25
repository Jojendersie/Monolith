#pragma once

#include <unordered_map>
#include "sparseoctree.hpp"
#include "component.hpp"
#include "material.hpp"
#include "../math/math.hpp"
#include "../math/vector.hpp"
#include "../predeclarations.hpp"

namespace Voxel {

	/// \brief An internal used struct as an octree traversal param.
	struct DrawParam;

	/// \brief A model is a high level abstraction with graphics and
	///		game play elements.
	class Model
	{
	public:
		typedef SparseVoxelOctree<Component, Model> ModelData;

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
		/// \param [in] _camera The actual camera for transformation, culling
		///		and LOD computations.
		void Draw( const Input::Camera& _camera );

		/// \brief Set a voxel in the model and update mass properties.
		/// \see SparseVoxelOctree::Set.
		void Set( const Math::IVec3& _position, int _level, VoxelType _type )	{ m_voxelTree.Set( _position, _level, Component(_type) ); }

		/// \brief Returns the type of a voxel on a certain grid level and
		///		position.
		///	\details If the position is outside the return value is UNDEFINED. For
		///		levels other than 0 the returned value will be some
		///		approximating LOD (majority) of the children.
		VoxelType Get( const Math::IVec3& _position, int _level ) const;

		/// \brief Get the center of gravity (mass center)
		Math::Vec3 GetCenter() const						{ return m_center + m_position; }
		/// \brief Set the position of the model based on its current center of gravity
		void SetCenter(const Math::Vec3& _position)			{ m_position = _position - m_center; }
		/// \brief Rotate without any physical influence (rotatory velocity
		///		will be unchanged, no test for breaking of)
		void Rotate( const Math::Quaternion& _rotation )	{ m_rotation *= _rotation; }

		/// \brief Get the bounding radius of the sphere centered at the center
		///		of gravity.
		float GetRadius() const { return m_boundingSphereRadius; }

		/// \brief Get the model transformation matrix
		/// \param [out] _out A space where the matrix can be stored.
		/// \return The reference to _out
		Math::Mat4x4& GetModelMatrix( Math::Mat4x4& _out ) const;

		/// \brief Do an update of physical properties.
		/// \details If a voxel is deleted _newType is NONE. If a new voxel
		///		is created _oldType is NONE. Overwrite operations define both
		///		(deletion and creation).
		/// \param [in] _position Which voxel was exchanged? The fourth
		///		component is the size with logarithmic scale of the voxel.
		///		0 denotes the highest detail 2^0.
		///	\param [in] _oldType The type of the voxel which was before.
		void Update( const Math::IVec4& _position, const Component& _oldType, const Component& _newType );

		bool RayCast( const Math::Ray& _ray, int _targetLevel, ModelData::HitResult& _hit ) const;
	protected:
		std::unordered_map<Math::IVec4, Chunk> m_chunks;
		int m_numVoxels;				///< Count the number of voxels for statistical issues

		Math::Vec3 m_position;			///< Model position in the space.
		Math::Vec3 m_center;			///< The center of gravity (relative to the model).
		Math::Quaternion m_rotation;	///< The rotation around the center.

		Math::Quaternion m_rotatoryVelocity;	///< Current change of rotation per second
		Math::Vec3 m_velocity;			///< Velocity in m/s (vector length)
		float m_mass;					///< Mass (inertia) of the full model
		float m_rotatoryMomentum;		///< Inertia of rotation for the full model.

		float m_boundingSphereRadius;	///< Bounding sphere radius (to the center of gravity) for culling etc.

		//ComputeBounding
		ModelData m_voxelTree;
		
		/// \brief  Decide for one voxel if it has the correct detail level and
		///		is visible (culling).
		/// \details If the voxel is drawn the traversal is stopped and a chunk
		///		is created/rendered.
		//static bool DecideToDraw(const Math::IVec4& _position, VoxelType _type, bool _hasChildren, DrawParam* _param);
		
		friend class Chunk;
		friend struct DecideToDraw;
	};
};