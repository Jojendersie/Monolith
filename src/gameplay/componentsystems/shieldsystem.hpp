#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class ShieldSystem: public ComponentSystem
	{
	public:
		ShieldSystem(Ship& _theShip, unsigned _id) : ComponentSystem(_theShip, "Shield", _id) {}
	private:
	};

}