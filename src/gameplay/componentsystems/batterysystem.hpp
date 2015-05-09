#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class BatterySystem: public ComponentSystem
	{
	public:
		BatterySystem(class Ship& _theShip) : ComponentSystem(_theShip, "Batteries") {}
	private:
	};

}