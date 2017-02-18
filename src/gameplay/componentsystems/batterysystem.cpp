#include "batterysystem.hpp"
#include "voxel/voxel.hpp"


namespace Mechanics{

	void BatterySystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		m_energyMaxOut = ei::min(m_charge, m_maxEnergyOutput * _deltaTime);

		m_energyDemand = ei::min(m_capacity - m_charge, m_maxEnergyDrain * _deltaTime);
	}

	void BatterySystem::Process(float _deltaTime, SystemRequierements& _provided)
	{
		m_charge += ei::min(m_capacity - m_charge, m_energyIn);
		// the energy loss is negative
		m_charge = ei::max(m_charge + m_energyLoss, 0.f);
	}

	void BatterySystem::OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment)
	{
		//todo: calculate different types individually to correctly
		// simulate combinations with different charge speeds and capacities
		m_maxEnergyOutput += Voxel::TypeInfo::GetEnergyOut(_type);
		m_maxEnergyDrain += Voxel::TypeInfo::GetEnergyDrain(_type);
		m_capacity += Voxel::TypeInfo::GetCapacity(_type);
	}

	void BatterySystem::ClearSystem()
	{
		m_capacity = 0;
		m_charge = 0;
		m_maxEnergyDrain = 0;
		m_maxEnergyOutput = 0;
	}
}