#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class BatterySystem: public ComponentSystem
	{
	public:
		BatterySystem(class Ship& _theShip, unsigned _id) : ComponentSystem(_theShip, "Batteries", _id) {}
	private:
	};

}