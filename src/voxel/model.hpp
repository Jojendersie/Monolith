#pragma once

#include <unordered_map>
#include "sparseoctree.hpp"
#include "voxel.hpp"
#include "../math/math.hpp"
#include "../predeclarations.hpp"

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
		/// \param [in] _camera The actual camera for transformation, culling
		///		and LOD computations.
		void Draw( Graphic::UniformBuffer& _objectConstants, const Input::Camera& _camera );

		/// \brief Set a voxel in the model and update mass properties.
		/// \see SparseVoxelOctree::Set.
		void Set( const Math::IVec3& _position, int _level, VoxelType _type )	{ m_voxelTree.Set( _position, _level, _type ); }

		/// \brief Returns the type of a voxel on a certain grid level and
		///		position.
		///	\details If the position is outside the return value is UNDEFINED. For
		///		levels other than 0 the returned value will be some
		///		approximating LOD (majority) of the children.
		VoxelType Get( const Math::IVec3& _position, int _level ) const			{ return m_voxelTree.Get(_position, _level); }

		/// \brief Get the center of gravity (mass center)
		Math::Vec3 GetCenter() const		{ return m_center + m_position; }
		/// \brief Get the bounding radius of the sphere centered at the center
		///		of gravity.
		float GetRadius() const { return m_boundingSphereRadius; }

		/// \brief Do an update of physical properties.
		/// \details If a voxel is deleted _newType is NONE. If a new voxel
		///		is created _oldType is NONE. Overwrite operations define both
		///		(deletion and creation).
		/// \param [in] _position Which voxel was exchanged?
		/// \param [in] _size Logarithmic scale of the voxel 0 means the
		///		highest detail 2^0.
		///	\param [in] _oldType The type of the voxel which was before.
		void Update( const Math::IVec3& _position, int _size, VoxelType _oldType, VoxelType _newType );

	protected:
		//std::unordered_map<IVec4, Chunk> m_chunks;
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
		SparseVoxelOctree<VoxelType, Model> m_voxelTree;
		

		friend class Chunk;
	};
};