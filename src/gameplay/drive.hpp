#pragma once
/// \file drive.hpp
/// \brief Computations to determine the drive properties of a ship.

#include "predeclarations.hpp"

namespace Mechanics {

	/// \brief Compute the function of how much force a voxel can create in
	///		which direction.
	/// \param [in] _model The model which contains the drive.
	/// \param [in] _unit Octree position (level 0) of the thruster.
	/// \param [in] _centralForce Part of the force in direction of the center.
	/// \param [in] _tangentialForce Part of the force in tangential direction
	///		to the center.
	void computeSingleDriveThrust( const Voxel::Model& _model,
								   const Math::IVec3& _unit,
								   Math::SphericalFunction& _centralForce,
								   Math::SphericalFunction& _tangentialForce );

} // namespace Mechanics