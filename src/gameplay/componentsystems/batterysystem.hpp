#pragma once

#include "componentsystem.hpp"

namespace Mechanics {

	class BatterySystem: public ComponentSystem
	{
	public:
		BatterySystem(class Ship& _theShip, unsigned _id) : ComponentSystem(_theShip, "Batteries", _id) {}

		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;
		virtual void Process(float _deltaTime, SystemRequierements& _provided) override;
		virtual void OnAdd(const ei::IVec3& _position, Voxel::ComponentType _type, uint8 _assignment) override;
		virtual void ClearSystem() override;

		float RelativeCharge() const { return m_charge / m_capacity; }
	public:
		float m_capacity;
		float m_charge;
		float m_maxEnergyDrain;
		float m_maxEnergyOutput;
	};

}