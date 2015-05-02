#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class DriveSystem: public ComponentSystem
	{
	public:
		DriveSystem(class Ship& _theShip) : ComponentSystem(_theShip, "Drives") {}
	private:
	};

}