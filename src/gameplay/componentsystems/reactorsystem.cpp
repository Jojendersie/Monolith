#include "reactorsystem.hpp"
#include "voxel/voxel.hpp"

using namespace ei;

namespace Mechanics {

	void ReactorSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		// Simply produce as much as the components can afford
		m_energyMaxOut = m_totalEnergyOut * _deltaTime;
	}

	void ReactorSystem::OnAdd(const IVec3& _position, Voxel::ComponentType _type, uint8 _assignment)
	{
		m_totalEnergyOut += Voxel::TypeInfo::GetEnergyOut( _type );
	}

	void ReactorSystem::ClearSystem()
	{
		m_totalEnergyOut = 0;
	}
}