#pragma once

#include "chunk.hpp"
#include "../math/math.hpp"
#include "../predeclarations.hpp"
#include <poolallocator.hpp>

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
		Chunk** m_chunks;				///< A loose collection of chunks. TODO: octree
		int m_numChunks;

		Math::Vec3 m_position;			///< Model position in the space.
		Math::Vec3 m_center;			///< The center of gravity (relative to the model).
		Math::Quaternion m_rotation;	///< The rotation around the center.

		Math::Quaternion m_rotatoryVelocity;	///< Current change of rotation per second
		Math::Vec3 m_velocity;			///< Velocity in m/s (vector length)
		float m_mass;					///< Mass (inertia) of the full model
		float m_rotatoryMomentum;		///< Inertia of rotation for the full model.

		float m_boundingSphereRadius;	///< Bounding sphere radius (to the center of gravity) for culling etc.

		//ComputeBounding

		/// \brief A Sparse-Voxel-Octree-Node
#		pragma pack(push,1)
		struct SVON {
			SVON* children;
			VoxelType type;

			/// \brief Set a voxel on the highest resolution.
			/// \param [in] _currentSize The voxel size in the current recursion
			///		level. This can be used to compute the full actual position.
			///	\param [in] _position Position where to set the new voxel.
			///	\param [in] _size Logarithmic size of the voxel to be set.
			///	\param [in] _type The new type.
			/// \param [in] _model The model for all update steps if voxels are
			///		overwritten.
			void Set(int _currentSize, const Math::IVec3& _position, int _size, VoxelType _type, Model* _model);

			/// \brief Set all children to NONE to do an correct update and than
			///		delete the memory block.
			/// \param [in] _model The model for all update steps if voxels are
			///		overwritten.
			void RemoveSubTree(const Math::IVec3& _position, int _size, Model* _model);

			/// \brief Check if all children have the same type.
			bool IsUniform();

			int ComputeChildIndex(const Math::IVec3& _targetPosition, int _targetSize, int _childSize);
		};
#		pragma pack(pop)

		/// \brief An own allocator for all nodes of this model's octree
		Jo::Memory::PoolAllocator m_SVONAllocator;

		//SVON* NewSVON()					{ return (SVON*)m_SVONAllocator.Alloc(); }
		//void DeleteSVON(SVON* _node)	{ m_SVONAllocator.Free(_node); }

		SVON* m_voxelTree;	///< The differences in the model to the blue print.
	};
};