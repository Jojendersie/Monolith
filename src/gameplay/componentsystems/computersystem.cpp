#include "computersystem.hpp"

namespace Mechanics {

	void ComputerSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		m_batteries.Estimate(_deltaTime, _requirements);
		m_drives.Estimate(_deltaTime, _requirements);
		m_reactors.Estimate(_deltaTime, _requirements);
		m_sensors.Estimate(_deltaTime, _requirements);
		m_shields.Estimate(_deltaTime, _requirements);
		m_storage.Estimate(_deltaTime, _requirements);
		m_weapons.Estimate(_deltaTime, _requirements);
		for(auto sys : m_subSystems)
			sys.Estimate(_deltaTime, _requirements);

		m_energyDemand = m_batteries.m_energyDemand
			+ m_drives.m_energyDemand
			+ m_sensors.m_energyDemand
			+ m_shields.m_energyDemand
			+ m_storage.m_energyDemand
			+ m_weapons.m_energyDemand;

		m_energyMaxOut = m_batteries.m_energyMaxOut
			+ m_reactors.m_energyMaxOut;
	}

	void ComputerSystem::Process(float _deltaTime, SystemRequierements& _provided)
	{
		// Dummy implementation
		// later this is done by the script
		m_drives.m_energyIn = 100.0f;
		m_weapons.m_energyIn = 100.0f;

		m_batteries.Process(_deltaTime, _provided);
		m_drives.Process(_deltaTime, _provided);
		m_reactors.Process(_deltaTime, _provided);
		m_sensors.Process(_deltaTime, _provided);
		m_shields.Process(_deltaTime, _provided);
		m_storage.Process(_deltaTime, _provided);
		m_weapons.Process(_deltaTime, _provided);
		for(auto sys : m_subSystems)
			sys.Process(_deltaTime, _provided);
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