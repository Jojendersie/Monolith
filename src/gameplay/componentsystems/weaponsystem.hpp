#pragma once

#include <vector>

#include "componentsystem.hpp"

namespace Mechanics {

	struct WeaponInformation
	{
		//type to identify on change
		Voxel::ComponentType type;

		float cooldown; ///< cooldown until next shot is avialable
		float cooldownBase; ///< cooldown after a shot
		int damage; ///< damage that one shot deals

		float cost; ///< energy requirement for one shot

		Math::Vec3 position; ///< relative position
	};

	class WeaponSystem: public ComponentSystem
	{
	public:
		WeaponSystem(class Ship& _theShip);

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;

		virtual void Process(float _deltaTime, SystemRequierements& _provided) override;

		virtual void OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type) override;

		void SetTarget(Math::Vec3& _target) { m_target = _target; };

		void SetFiring(bool _firing) { m_firing = _firing; };
	private:

		Math::Vec3 m_target; ///< target in local space
		bool m_firing;		///< should the weapons fire when loaded

		std::vector < WeaponInformation > m_weapons;
	};

}