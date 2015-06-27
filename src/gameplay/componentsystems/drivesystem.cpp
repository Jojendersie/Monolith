#include "drivesystem.hpp"
#include "math/ray.hpp"
#include "gameplay/ship.hpp"

using namespace Math;

namespace Mechanics {

	DriveSystem::DriveSystem(class Ship& _theShip, unsigned _id) :
		ComponentSystem(_theShip, "Drive", _id),
		m_maxTorque([](const Vec3& _dir){ return 1.0f; }),
		m_maxThrust([](const Vec3& _dir){ return 1.0f; })
	{
	}

	void DriveSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		// Compute how much of the required force can by provided by this drive system?
		// Decompose the vectors
		m_currentThrust = length( _requirements.thrust );
		m_currentTorque = length( _requirements.torque );
		m_currentThrustDir = _requirements.thrust / max(1e-6f, m_currentThrust);
		m_currentTorqueDir = _requirements.torque / max(1e-6f, m_currentTorque);

		if( m_currentThrust + m_currentTorque > 1e-5f )
		{
			// Get maximal drive properties for current directions
			float maxThrust = 0.0f, maxTorque = 0.0f;
			if( m_currentThrust > 0.0f )
				maxThrust = m_maxThrust(m_currentThrustDir);
			if( m_currentTorque > 0.0f )
				maxTorque = m_maxTorque(m_currentTorqueDir);
			//float requiredEnergy = m_energyUsage();
			// Limit to maximum and normalize the sum
			m_currentThrust = min(m_currentThrust, maxThrust);
			m_currentTorque = min(m_currentTorque, maxTorque);
			float sumMax = maxThrust + maxTorque;
			float sum = m_currentThrust + m_currentTorque;
			float norm = min(1.0f, sumMax / max(1e-6f, sum));
			m_currentThrust *= norm; m_currentTorque *= norm;
			// How large is a percentage of how much energy must be used now
			m_energyDemand = m_maxEnergyDrain * (m_currentThrust + m_currentTorque) / max(1e-6f, sumMax);
		} else 
			m_currentThrust = m_currentTorque = m_energyDemand = 0.0f;
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

	void DriveSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type, uint8_t _assignment)
	{
		// Do not need to test the assignment (computer checks this for us)
		Assert(_assignment == m_id, "Component added to the wrong drive system");
		float thrust = Voxel::TypeInfo::GetThrust(_type);
		float drain = Voxel::TypeInfo::GetEnergyDrain(_type);
		m_maxEnergyDrain += drain;
		// For each direction in the cube map make a visibility test
		Math::CubeMap<3> newTorque;
		Math::CubeMap<3> newThrust;
		auto endit = newThrust.end();
		Vec3 center = _position+Vec3(0.5f);
		Vec3 centerDir = _position + 0.5f - m_ship.GetCenter();
		float centerDist = length(centerDir);
		centerDir /= max(centerDist, 1.0f);
		for(auto itthr = newThrust.begin(), iteng = m_energyUsage.begin(); itthr != endit; ++itthr, ++iteng)
		{
			// Do ray casts directly in model space
			Math::Ray ray(center, -itthr.direction());
			float distance = 100000.0f;
			ray.m_origin += ray.m_direction * 1.414213562f;
			Voxel::Model::ModelData::HitResult hit;
			// TODO: more fine grained sampling
			float force = thrust;
			// In occluded directions there is thrust=0. Because this results in bad
			// game play use 10% instead.
			if(m_ship.GetVoxelTree().RayCast( ray, 0, hit, distance ))
				force *= 0.1f;

			// Only if nothing is visible the drive can fire into this direction.
			iteng.value() += drain;
			// Divide the force into a rotation and a forward thrust.
			Vec3 axis = cross(centerDir, ray.m_direction);
			float sinForce = length(axis);
			float cosForce = sqrt( saturate( 1.0f - sinForce * sinForce ) );
			itthr.value() = cosForce * force;
			// Torque must be update by splatting.
			if( sinForce > 1e-6f )
				newTorque.splat(axis, centerDist * sinForce * force);
		}
		m_maxThrust += newThrust;
		m_maxTorque += newTorque;
	}

	void DriveSystem::ClearSystem()
	{
		m_maxTorque = Math::CubeMap<3>();
		m_maxThrust = Math::CubeMap<3>();
	}

}