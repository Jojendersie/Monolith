#include "computersystem.hpp"

namespace Mechanics {

	void ComputerSystem::Estimate(float _deltaTime)
	{
		m_energyDemand = m_batteries.m_energyDemand
			+ m_drives.m_energyDemand
			+ m_sensors.m_energyDemand
			+ m_shields.m_energyDemand
			+ m_storage.m_energyDemand
			+ m_weapons.m_energyDemand;

		m_energyMaxOut = m_batteries.m_energyMaxOut
			+ m_reactors.m_energyMaxOut;
	}

	void ComputerSystem::Process(float _deltaTime)
	{
		// Dummy implementation
		// later this is done by the script
		m_drives.m_energyIn = 100.0f;
		m_weapons.m_energyIn = 100.0f;
	}

	void ComputerSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type)
	{
	}

	void ComputerSystem::OnRemove(const Math::IVec3& _position, Voxel::ComponentType _type)
	{
	}

	void ComputerSystem::OnNeighborChange(const Math::IVec3& _position)
	{
	}
}