#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class StorageSystem: public ComponentSystem
	{
	public:
		StorageSystem(Ship& _theShip) : ComponentSystem(_theShip, "Storage") {}
	private:
	};

}