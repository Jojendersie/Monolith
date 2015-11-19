#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class ReactorSystem: public ComponentSystem
	{
	public:
		ReactorSystem(Ship& _theShip, unsigned _id) : ComponentSystem(_theShip, "Reactor", _id) {}

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;
		virtual void OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) override;
		virtual void ClearSystem() override;
	private:
		float m_totalEnergyOut;		///< J/s energy production.
	};

}