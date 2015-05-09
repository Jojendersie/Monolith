#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class SensorSystem: public ComponentSystem
	{
	public:
		SensorSystem(Ship& _theShip) : ComponentSystem(_theShip, "Sensors") {}
	private:
	};

}