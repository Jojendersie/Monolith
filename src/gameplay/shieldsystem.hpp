#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class ShieldSystem: public ComponentSystem
	{
	public:
		ShieldSystem(Ship& _theShip) : ComponentSystem(_theShip, "Shield") {}
	private:
	};

}