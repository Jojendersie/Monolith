#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class SensorSystem: public ComponentSystem
	{
	public:
		SensorSystem(Ship& _theShip, unsigned _id) : ComponentSystem(_theShip, "Sensors", _id) {}
	private:
	};

}