#include "componentsystem.hpp"

namespace Mechanics {

	ComponentSystem::ComponentSystem(Ship& _theShip, const std::string& _name) :
		m_energyDemand(0.0f),
		m_energyIn(0.0f),
		m_energyMaxOut(0.0f),
		m_energyLoss(0.0f),
		m_name(_name),
		m_ship(_theShip)
	{
	}

	ComponentSystem::~ComponentSystem()
	{
	}
}