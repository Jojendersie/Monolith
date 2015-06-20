#pragma once

#include "componentsystem.hpp"
#include "math/sphericalfunction.hpp"

namespace Mechanics {

	class DriveSystem: public ComponentSystem
	{
	public:
		DriveSystem(class Ship& _theShip, unsigned _id);
		virtual void Estimate(float _deltaTime, SystemRequierements& _requirements) override;
		virtual void Process(float _deltaTime, SystemRequierements& _provided) override;
	private:
		Math::Vec3 m_currentThrustDir;	///< Normalized direction of thrust
		Math::Vec3 m_currentTorqueDir;	///< Normalized axis of torque
		float m_currentThrust;
		float m_currentTorque;

		/// Precomputed system parameters
		Math::CubeMap<3> m_maxTorque;
		Math::CubeMap<3> m_maxThrust;
		//Math::CubeMap<3> m_torqueThrust[3];	///< Coupling: what thrust is induced from the torque
		//Math::CubeMap<3> m_thrustTorque[3];	///< Coupling: what torque is induced from the thrust
		float m_maxEnergyDrain;
	};

}