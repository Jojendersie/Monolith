#include "drivesystem.hpp"

using namespace Math;

namespace Mechanics {

	DriveSystem::DriveSystem(class Ship& _theShip, unsigned _id) :
		ComponentSystem(_theShip, "Drive", _id),
		m_maxTorque([](const Vec3& _dir){ return 1.0f; }),
		m_maxThrust([](const Vec3& _dir){ return 1.0f; })
	{
	}

	// TODO: Use Time!!!
	void DriveSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		// Compute how much of the required force can by provided by this drive system?
		// Decompose the vectors
		m_currentThrust = length( _requirements.thrust );
		m_currentTorque = length( _requirements.torque );
		m_currentThrustDir = _requirements.thrust / max(1e-6f, m_currentThrust);
		m_currentTorqueDir = _requirements.torque / max(1e-6f,m_currentTorque);

		if( m_currentThrust + m_currentTorque > 0.0f )
		{
			// Get maximal drive properties for current directions
			float maxThrust = 0.0f, maxTorque = 0.0f;
			if( m_currentThrust > 0.0f )
				maxThrust = 1.0f;//m_maxThrust(m_currentThrustDir);
			if( m_currentTorque > 0.0f )
				maxTorque = 1.0f;//m_maxTorque(m_currentTorqueDir);
			// Limit to maximum and normalize the sum
			m_currentThrust = min(m_currentThrust, maxThrust);
			m_currentTorque = min(m_currentTorque, maxTorque);
			float sumMax = maxThrust + maxTorque;
			float sum = m_currentThrust + m_currentTorque;
			float norm = min(1.0f, sumMax / sum);
			m_currentThrust *= norm; m_currentTorque *= norm;
			// How large is now a percentage of how much energy must be used*
			m_energyDemand = m_maxEnergyDrain * (m_currentThrust + m_currentTorque) / sumMax;
		} else 
			m_energyDemand = 0.0f;
	}

	void DriveSystem::Process(float _deltaTime, SystemRequierements& _provided)
	{
		if(m_energyDemand)
		{
			// Scale to available energy
			float e = m_energyIn / m_energyDemand;

			_provided.thrust = m_currentThrustDir * (m_currentThrust * e);
			_provided.torque = m_currentTorqueDir * (m_currentTorque * e);
			// TODO: more coupling. Compute torque from thrust direction and thrust from
			// target torque. I.e. use the drive for both components independently but compute real possible
			// effect together.
		}
	}
}