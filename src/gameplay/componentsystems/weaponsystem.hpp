#pragma once

#include <vector>

#include "componentsystem.hpp"
#include "graphic/highlevel/particlesystem.hpp"

namespace Mechanics {

	struct WeaponInformation
	{
		//type to identify on change
		Voxel::ComponentType type;

		float cooldown; ///< cooldown until next shot is available
		float cooldownBase; ///< cooldown after a shot
		int damage; ///< damage that one shot deals

		float cost; ///< energy requirement for one shot

		ei::Vec3 position; ///< relative position
	};

	class WeaponSystem: public ComponentSystem
	{
	public:
		WeaponSystem(class Ship& _theShip, unsigned _id);

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;

		virtual void Process(float _deltaTime, SystemRequierements& _provided) override;

		virtual void OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) override;

		void SetTarget(const ei::Vec3& _target) { m_target = _target; };

		void SetFiring(bool _firing) { m_firing = _firing; };

		//same thing as above, but the compiler does not allow this calls to be casted to void*
		static void _SetFiring(WeaponSystem* _this, int _firing) { _this->SetFiring(_firing ? true : false); }
	private:

		ei::Vec3 m_target; ///< target in local space
		bool m_firing;		///< should the weapons fire when loaded

		std::vector < WeaponInformation > m_weapons;

		Graphic::ParticleSystems::System<Graphic::PSComponent::POSITION | Graphic::PSComponent::VELOCITY | Graphic::PSComponent::LIFETIME | Graphic::PSComponent::COLOR | Graphic::PSComponent::SIZE> m_particles;
	};

}