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
		virtual void OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type, uint8_t _assignment) override;
		virtual void ClearSystem() override;
	private:
		Math::Vec3 m_currentThrust;
		Math::Vec3 m_currentTorque;

		/// Precomputed system parameters
		Math::SphericalFunction m_maxThrust;	///< Amplitude of thrust into a direction [kN], and yzw = how does this influence torque?
	//	Math::SphericalFunction m_thrustToTorqueX;
	//	Math::SphericalFunction m_thrustToTorqueY;
	//	Math::SphericalFunction m_thrustToTorqueZ;
		Math::SphericalFunction m_maxTorque;	///< Amplitude of maximum torque for a special axis [kNm], and yzw = how much does this influence the thrust?
		// If maximum torque is generated how much does this influence the thrust
	//	Math::SphericalFunction m_torqueToThrustX;
	//	Math::SphericalFunction m_torqueToThrustY;
	//	Math::SphericalFunction m_torqueToThrustZ;
		//Math::SphericalFunction m_energyUsage;	///< How many of the drives can fire in the desired direction?
		float m_maxEnergyDrain;

		friend class Ship;
	};

}