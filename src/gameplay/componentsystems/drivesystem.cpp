#include "drivesystem.hpp"
#include "math/ray.hpp"
#include "gameplay/ship.hpp"

using namespace Math;

namespace Mechanics {

	DriveSystem::DriveSystem(class Ship& _theShip, unsigned _id) :
		ComponentSystem(_theShip, "Drive", _id)
	{
	}

	void DriveSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		// Compute how much of the required force can by provided by this drive system?
		// Decompose the vectors
		float currentThrustAbs = length( _requirements.thrust );
		float currentTorqueAbs = length( _requirements.torque );

		if( currentThrustAbs + currentTorqueAbs > 1e-5f )
		{
			// Get maximal drive properties for current directions
			Vec4 maxThrust(0.0f), maxTorque(0.0f);
			if( currentThrustAbs > 0.0f )
				maxThrust = m_maxThrust(_requirements.thrust);
			if( currentTorqueAbs > 0.0f )
				maxTorque = m_maxTorque(_requirements.torque);
			// Assumption: both the maxThrust and maxTorque require full energy.
			// Compute a relation how much is needed.
			float relTh = min(1.0f, currentThrustAbs / max(1e-6f, maxThrust[0]));
			float relTo = min(1.0f, currentTorqueAbs / max(1e-6f, maxTorque[0]));
			// Scale both down if they need more than 100%
			float sum = max(1.0f, relTh + relTo);
			relTh /= sum;
			relTo /= sum;
			// Apply what is remaining, additionally add the side effects from both components
			m_currentThrust = maxThrust[0] * relTh * _requirements.thrust / max(1e-6f, currentThrustAbs);
//			m_currentThrust[0] += maxTorque[1] * relTo;
//			m_currentThrust[1] += maxTorque[2] * relTo;
//			m_currentThrust[2] += maxTorque[3] * relTo;
			m_currentTorque = maxTorque[0] * relTo * _requirements.torque / max(1e-6f, currentTorqueAbs);
//			m_currentTorque[0] += maxThrust[1] * relTh;
//			m_currentTorque[1] += maxThrust[2] * relTh;
//			m_currentTorque[2] += maxThrust[3] * relTh;
			// TODO: the current resulting Torque and thrust are not optimal. Iterative refinement might help!
	
			// How large is a percentage of how much energy must be used now
			m_energyDemand = m_maxEnergyDrain * (relTh + relTo);
		} else 
			m_currentThrust = m_currentTorque = m_energyDemand = 0.0f;
	}

	void DriveSystem::Process(float _deltaTime, SystemRequierements& _provided)
	{
		if(m_energyDemand)
		{
			// Scale to available energy
			float e = m_energyIn / m_energyDemand;

			_provided.thrust = m_currentThrust * e;
			_provided.torque = m_currentTorque * e;
		}
	}

	void DriveSystem::OnAdd(const Math::IVec3& _position, Voxel::ComponentType _type, uint8_t _assignment)
	{
		// Do not need to test the assignment (computer checks this for us)
		Assert(_assignment == m_id, "Component added to the wrong drive system");
		float thrust = Voxel::TypeInfo::GetThrust(_type);
		float drain = Voxel::TypeInfo::GetEnergyDrain(_type);
		m_maxEnergyDrain += drain;
		// Iterate over a virtual cube map to increase sampling density.
		// For each direction in the cube map make a visibility test.
		Math::CubeMap<6, float> directionGenerator;
		SphericalFunction newThrust, newTorque;
		auto endit = directionGenerator.end();
		Vec3 center = _position+Vec3(0.5f);
		Vec3 centerDir = _position + 0.5f - m_ship.GetCenter();
		// Count number of samples per direction to normalize.
		int thrustCount[26] = {0};
		int torqueCount[26] = {0};
		for(auto it = directionGenerator.begin(); it != endit; ++it)
		{
			// Do ray casts directly in model space
			Vec3 direction = it.direction();
			Math::Ray ray(center, -direction);
			float distance = 100000.0f;
			ray.m_origin += ray.m_direction * 1.414213562f;
			Voxel::Model::ModelData::HitResult hit;
			float force = thrust;
			// Only if nothing is visible the drive can fire into this direction.
			// In occluded directions there is thrust=0. Because this results in bad
			// game play use 10% instead.
			if(m_ship.GetVoxelTree().RayCast( ray, 0, hit, distance ))
				force *= 0.1f;

			// Divide the force into a rotation and a forward thrust.
			Vec3 axis = cross(centerDir, direction);
			float torque = length(axis) * force;

			int idx = newThrust.getSplatIndex( direction );
			newThrust[idx] += force;
			newThrust[idx][1] += axis[0] * torque;
			newThrust[idx][2] += axis[1] * torque;
			newThrust[idx][3] += axis[2] * torque;
			++thrustCount[idx];
			// Torque must be update by splatting.
			if( torque > 1e-6f ) {
				idx = newTorque.getSplatIndex(axis);
				newTorque[idx] += torque;
				newTorque[idx][1] = force * direction[0];
				newTorque[idx][2] = force * direction[1];
				newTorque[idx][3] = force * direction[2];
				++torqueCount[idx];
			}
		}
		for(int i=0; i<26; ++i)
		{
			m_maxThrust[i] += newThrust[i] / max((float)thrustCount[i], 1e-6f);
			m_maxTorque[i] += newTorque[i] / max((float)torqueCount[i], 1e-6f);
		}
	}

	void DriveSystem::ClearSystem()
	{
		m_maxTorque = Math::SphericalFunction();
		m_maxThrust = Math::SphericalFunction();
	}

}