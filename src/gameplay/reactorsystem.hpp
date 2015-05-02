#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class ReactorSystem: public ComponentSystem
	{
	public:
		ReactorSystem(Ship& _theShip) : ComponentSystem(_theShip, "Reactor") {}

		virtual void Estimate(float _deltaTime) override;
		virtual void OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type) override;
		virtual void OnRemove(const Math::IVec3& _position, Voxel::ComponentType _type) override;
	private:
		float m_totalEnergyOut;		///< J/s energy production.
	};

}