#pragma once

#include "chunk.hpp"
#include "../math/math.hpp"

namespace Voxel {

	/// \brief A model is a high level abtraction with graphics and
	///		gameplay elements.
	class Model
	{
	public:
		/// \brief Draw a full model.
		/// \details This method sets the texture map???
		///
		///		In the first step the culling for the whole model is done
		///		and then each chunk is rendered.
		///
		///		The effect must be set outside.
		/// \param [out] _ObjectConstants A reference to the constant buffer
		///		which must be filled.
		/// \param [in] _mViewProjection The actual view projection matrix. TODO: camera mit culling
		void Draw( Graphic::UniformBuffer& _ObjectConstants, const Math::Matrix& _mViewProjection );

		/// \brief Set a voxel in the model and update mass properties.
		/// \details This method overwrites all covered coxels.
		/// \param [in] _vPosition Position inside the given level.
		/// \param [in] _iLevel Depth in the grid hierarchy. 0 is the maximum
		///		size and 5 is the level of the smallest voxel grid.
		void Set( const Math::IVec3& _vPosition, int _iLevel, VoxelType _Type );
	private:
		Chunk** m_aChunks;				///< A loose collection of chunks. TODO: octree
		int m_iNumChunks;

		Math::Vec3 m_vPosition;			///< The center of gravity.
		Math::Quaternion m_vRotation;	///< The rotation around the center

		Math::Quaternion m_vRotatoryVelocity;	///< Current chage of rotation per second
		Math::Vec3 m_vVelocity;			///< Velocity in m/s (vector length)
		float m_fMass;					///< Mass (inertia) of the full model
		float m_fRotatoryMomentum;		///< Inertia of rotation for the full model.
	};
};