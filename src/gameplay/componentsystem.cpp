#include "componentsystem.hpp"

namespace Mechanics {

	ComponentSystem::ComponentSystem(const std::string& _name) :
		m_energyDemand(0.0f),
		m_energyIn(0.0f),
		m_energyMaxOut(0.0f),
		m_energyLoss(0.0f),
		m_name(_name)
	{
	}

	ComponentSystem::~ComponentSystem()
	{
	}
}