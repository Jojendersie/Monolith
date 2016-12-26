#include "weaponsystem.hpp"
#include "gameplay\ship.hpp"
#include "../firemanager.hpp"
#include "utilities/color.hpp"
#include "utilities/scriptengineinst.hpp"

using namespace ei;

namespace Mechanics {

	const float c_projVel = 300.f;

	WeaponSystem::WeaponSystem(Ship& _theShip, unsigned _id)
		: ComponentSystem(_theShip, "Weapons", _id),
		m_particles(Graphic::ParticleSystems::RenderType::BLOB) //RAY
	{
		m_firing = false;
	}


	void WeaponSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		m_energyMaxOut = 0.f;

		//prevent division by zero
		m_energyDemand = 0.0001f;
		for (auto& weapon : m_weapons)
			m_energyDemand += min(weapon.cooldown, _deltaTime / weapon.cooldownBase * weapon.cost);
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
				weapon.cooldown = weapon.cooldownBase;
				Ray ray(weapon.position, m_ship.GetRotationMatrix() * Vec3(0.f, 0.f, 1.f)/*zaxis(m_ship.GetRotation())*/);

				ray.origin = m_ship.GetRotationMatrix() * ray.origin;
				//when the player ship is not in the way
		//		if (!m_ship.GetVoxelTree().RayCast(ray, 1, hit, distance))
				{
					Vec3 basePos(m_ship.GetPosition() - m_particles.GetPosition());
					basePos += ray.origin;

					Math::WorldRay wRay;
					wRay.origin = m_ship.GetPosition();
					wRay.origin.x += Math::Fix(ray.origin.x);
					wRay.origin.y += Math::Fix(ray.origin.y);
					wRay.origin.z += Math::Fix(ray.origin.z);
					wRay.direction = ray.direction;
					float d = g_fireManager->FireRay(FireRayInfo(wRay, weapon.damage));

					for (int i = 0; i < 10; ++i)
					m_particles.AddParticle(basePos, //position
						ray.direction * c_projVel * i / 10.f,// velocity
						d / c_projVel, //life time
						Utils::Color8U(0.4f, 0.1f, 0.8f).RGBA(),
						1.f);

					//temporary, hit feedback should be done by the model?
					if (d != 100.f)
					{
						static Generators::Random rng(351298);
						Vec3 hitPos(basePos + ray.direction * d);

						for (int i = 0; i < 50; ++i)
							m_particles.AddParticle(hitPos, //position
								Vec3(rng.Uniform(0.1f, 3.0f), rng.Uniform(0.1f,3.0f), rng.Uniform(0.1f, 3.0f)),// velocity
								rng.Uniform(0.2f, 1.f), //life time
								Utils::Color8U(0.15f, 0.2f, 0.2f).RGBA(),
								0.5f);
					}
				}
			}
		}
	}


	void WeaponSystem::OnAdd(const IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) 
	{
		WeaponInformation weapon;
		weapon.position = _position + 0.5f - m_ship.GetCenter();
		weapon.position.z += 1.f; // begin firing outside of the voxel

		weapon.cooldown = 0.f;
		weapon.cooldownBase = 2.f;

		//100% efficiency
		weapon.damage = 200;
		weapon.cost = 10.f;

		m_weapons.push_back(weapon);
	}
}