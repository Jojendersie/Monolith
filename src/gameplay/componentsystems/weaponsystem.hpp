#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class WeaponSystem: public ComponentSystem
	{
	public:
		WeaponSystem(class Ship& _theShip) : ComponentSystem(_theShip, "Weapons") {}
	private:
	};

}