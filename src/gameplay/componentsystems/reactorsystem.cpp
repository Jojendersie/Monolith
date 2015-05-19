#include "reactorsystem.hpp"
#include "voxel/voxel.hpp"

namespace Mechanics {

	void ReactorSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		// Simply produce as much as the components can afford
		m_energyMaxOut = m_totalEnergyOut * _deltaTime;
	}

	void ReactorSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type)
	{
		m_totalEnergyOut += Voxel::TypeInfo::GetEnergyOut( _type );
	}

	void ReactorSystem::OnRemove(const Math::IVec3& _position, Voxel::ComponentType _type)
	{
		m_totalEnergyOut -= Voxel::TypeInfo::GetEnergyOut( _type );
	}
}