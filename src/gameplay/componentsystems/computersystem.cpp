#include "computersystem.hpp"
#include "gameplay/ship.hpp"

namespace Mechanics {

	ComputerSystem::ComputerSystem(Ship& _theShip, const std::string& _name) :
			ComponentSystem(_theShip, _name, _theShip.AllocNewSystemID()),
			m_batteries(_theShip, m_id),
			m_drives(_theShip, m_id),
			m_reactors(_theShip, m_id),
			m_sensors(_theShip, m_id),
			m_shields(_theShip, m_id),
			m_storage(_theShip, m_id),
			m_weapons(_theShip, m_id)
	{}

	ComputerSystem::~ComputerSystem()
	{
		m_ship.ReleaseSystemID(m_id);
	}

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
		m_drives.m_energyIn = Math::min(100.0f, m_drives.m_energyDemand);
		m_weapons.m_energyIn = Math::min(100.0f, m_weapons.m_energyDemand);

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

	void ComputerSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type, uint8_t _assignment)
	{
		if(m_id == _assignment) {
			if(Voxel::TypeInfo::IsComputer(_type)) m_enabled = true;
			else if(Voxel::TypeInfo::IsReactor(_type)) m_reactors.OnAdd(_position, _type, _assignment);
			else if(Voxel::TypeInfo::IsStorage(_type)) m_storage.OnAdd(_position, _type, _assignment);
			else if(Voxel::TypeInfo::IsBattery(_type)) m_batteries.OnAdd(_position, _type, _assignment);
			else if(Voxel::TypeInfo::IsDrive(_type)) m_drives.OnAdd(_position, _type, _assignment);
			else if(Voxel::TypeInfo::IsWeapon(_type)) m_weapons.OnAdd(_position, _type, _assignment);
			else if(Voxel::TypeInfo::IsShield(_type)) m_shields.OnAdd(_position, _type, _assignment);
			else if(Voxel::TypeInfo::IsSensor(_type)) m_sensors.OnAdd(_position, _type, _assignment);
		} else {
			// Push recursively
			for(auto sys : m_subSystems)
				sys.OnAdd(_position, _type, _assignment);
		}
	}

	void ComputerSystem::ClearSystem()
	{
		// Disable this system until its computer is found
		m_enabled = false;
		m_batteries.ClearSystem();
		m_drives.ClearSystem();
		m_reactors.ClearSystem();
		m_sensors.ClearSystem();
		m_shields.ClearSystem();
		m_storage.ClearSystem();
		m_weapons.ClearSystem();
		for(auto sys : m_subSystems)
			sys.ClearSystem();
	}
}