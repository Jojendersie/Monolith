#include "drivesystem.hpp"

namespace Mechanics {

	DriveSystem::DriveSystem(class Ship& _theShip) :
		ComponentSystem(_theShip, "Drive")
	{
	}

}