#include "drive.hpp"
#include "voxel/model.hpp"

namespace Mechanics {

	// ********************************************************************* //
	void computeSingleDriveThrust( const Voxel::Model& _model,
							  const Math::IVec3& _unit,
							  Math::SphericalFunction& _centralForce,
							  Math::SphericalFunction& _tangentialForce )
	{
		// I. First compute the total force the engine can create and store it to
		// one of the functions.

		// II. For each vector compute the central and tangential part in respect
		// to the model center and compute the two model-centered approximations.

	}

} // namespace Mechanics