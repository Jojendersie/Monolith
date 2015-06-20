#include "weaponsystem.hpp"
#include "gameplay\ship.hpp"

using namespace Math;

namespace Mechanics {

	WeaponSystem::WeaponSystem(Ship& _theShip)
		: ComponentSystem(_theShip, "Weapons")
	{
		//add some test data
		WeaponInformation weapon;

		weapon.cooldown = 0.f;
		weapon.cooldownBase = 2.f;

		//100% effiency
		weapon.damage = 10;
		weapon.cost = 10.f;

		m_weapons.push_back(weapon);
	}


	void WeaponSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		m_energyMaxOut = 0.f;

		m_energyDemand = 0.f;
		for (auto& weapon : m_weapons)
			m_energyDemand += min(0.f - weapon.cooldown, _deltaTime / weapon.cooldownBase * weapon.cost);
	}


	void WeaponSystem::Process(float _deltaTime, SystemRequierements& _provided)
	{
		//loading times and energy given are proportional
		float energyRatio = m_energyIn / m_energyDemand;

		//load weapons with the given energy
		for (auto& weapon : m_weapons)
			weapon.cooldown -= _deltaTime / weapon.cooldownBase * energyRatio * weapon.cost;

		//fire when commanded
		if (!m_firing) return;

		for (auto& weapon : m_weapons)
		{
			if (weapon.cooldown <= 0.f)
			{
				Math::Ray ray(weapon.position, m_ship.GetRotation().XAxis());
				Voxel::Model::ModelData::HitResult hit;
				float distance;

				//when the player ship is not in the way
				if (!m_ship.GetVoxelTree().RayCast(ray, 1, hit, distance))
				{
				}
			}
		}
	}


	void WeaponSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type) 
	{
		WeaponInformation weapon;
		weapon.position = Vec3(_position) + 0.5f;

		weapon.cooldown = 0.f;
		weapon.cooldownBase = 2.f;

		//100% effiency
		weapon.damage = 10;
		weapon.cost = 10.f;
	}
}