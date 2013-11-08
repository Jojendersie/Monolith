#pragma once

#include "chunk.hpp"
#include "../math/math.hpp"

namespace Voxel {

	/// \brief A model is a high level abtraction with graphics and
	///		gameplay elements.
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
		void Draw( Graphic::UniformBuffer& _objectConstants, const Math::Matrix& _viewProjection );

		/// \brief Set a voxel in the model and update mass properties.
		/// \details This method overwrites all covered coxels.
		/// \param [in] _position Position inside the given level.
		/// \param [in] _level Depth in the grid hierarchy. 0 is the maximum
		///		size and 5 is the level of the smallest voxel grid.
		void Set( const Math::IVec3& _position, int _level, VoxelType _type );
	protected:
		Chunk** m_chunks;				///< A loose collection of chunks. TODO: octree
		int m_numChunks;

		Math::Vec3 m_center;			///< The center of gravity.
		Math::Quaternion m_rotation;	///< The rotation around the center

		Math::Quaternion m_rotatoryVelocity;	///< Current chage of rotation per second
		Math::Vec3 m_velocity;			///< Velocity in m/s (vector length)
		float m_mass;					///< Mass (inertia) of the full model
		float m_rotatoryMomentum;		///< Inertia of rotation for the full model.
	};
};