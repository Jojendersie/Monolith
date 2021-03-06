#include "drivesystem.hpp"
#include "math/ray.hpp"
#include "gameplay/ship.hpp"
#include "utilities/color.hpp"

using namespace ei;
using namespace Math;

namespace Mechanics {

	// A coupling of 1 is physical correct, but less playable
	const float TORQUE_FORCE_COUPLING = 0.7f;
	const float DAMPING = 0.98f;
	// Denoising should remove very small values which increase instability.
	inline float denoise(float _x) { return int(_x * 128.0f) / 128.0f; }

	DriveSystem::DriveSystem(class Ship& _theShip, unsigned _id) :
		ComponentSystem(_theShip, "Drive", _id),
		m_particles(Graphic::ParticleSystems::RenderType::BLOB),
		m_rng(1091613067)
	{
		m_particles.SetParticleSize(0.15f);
	}

	void DriveSystem::Estimate(float _deltaTime, SystemRequierements& _requirements)
	{
		// Compute how much of the required force can by provided by this drive system?
		// Decompose the vectors
		float currentThrustAbs = denoise(DAMPING * len( _requirements.thrust ));
		float currentTorqueAbs = denoise(DAMPING * len( _requirements.torque ));

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
			// Apply what is remaining, additionally add the side effects from both components.
			// Use normalized direction _requirements.thrust / currentThrustAbs with new scale.
			m_currentThrust = maxThrust[0] * _requirements.thrust * (relTh / max(1e-6f, currentThrustAbs));
			m_currentThrust[0] += denoise(maxTorque[1] * relTo * TORQUE_FORCE_COUPLING);
			m_currentThrust[1] += denoise(maxTorque[2] * relTo * TORQUE_FORCE_COUPLING);
			m_currentThrust[2] += denoise(maxTorque[3] * relTo * TORQUE_FORCE_COUPLING);
			m_currentTorque = maxTorque[0] * _requirements.torque * (relTo / max(1e-6f, currentTorqueAbs));
			m_currentTorque[0] += denoise(maxThrust[1] * relTh * TORQUE_FORCE_COUPLING);
			m_currentTorque[1] += denoise(maxThrust[2] * relTh * TORQUE_FORCE_COUPLING);
			m_currentTorque[2] += denoise(maxThrust[3] * relTh * TORQUE_FORCE_COUPLING);
			// TODO: the current resulting Torque and thrust are not optimal. Iterative refinement might help!
	
			// How large is a percentage of how much energy must be used now
			m_energyDemand = m_maxEnergyDrain * _deltaTime * (relTh + relTo);
		} else {
			m_currentThrust = m_currentTorque = Vec3(0.0f);
			m_energyDemand = 0.0f;
		}
	}

	void DriveSystem::Process(float _deltaTime, SystemRequierements& _provided)
	{
		if (m_energyDemand)
		{
			// Scale to available energy
			float e = m_energyIn / m_energyDemand;

			_provided.thrust = m_currentThrust * e * _deltaTime;
			_provided.torque = m_currentTorque * e * _deltaTime;

			int numParticlesPerDrive = int(m_energyIn / (m_maxEnergyDrain * _deltaTime) * 150);
			if(numParticlesPerDrive > 5)
			{
				// In general particles are spawned with the ships velocity plus
				// some speed in thrust direction.
				Vec3 basicVelocity = - m_ship.GetRotationMatrix() * (_provided.thrust);
				Vec3 rotationAcceleration = m_ship.GetRotationMatrix() * (_provided.torque);
				for(size_t i = 0; i < m_thrusters.size(); ++i)
				{
					Vec3 thrusterPos = m_ship.GetRotationMatrix() * m_thrusters[i].relativePosition;
					Vec3 rotationVelocity = -cross(rotationAcceleration, thrusterPos);
					Vec3 thrustDir = normalize(basicVelocity + rotationVelocity);

					// Check if the drive can have thrust in the direction at all
					int numEffectiveParticles = int(numParticlesPerDrive * m_thrusters[i].freeField.value(m_ship.GetInverseRotationMatrix() * thrustDir));
					for(int j = 0; j < numEffectiveParticles; ++j)
					{
						Vec3 inThrusterPos(m_rng.Normal(0.02f), m_rng.Normal(0.02f), m_rng.Normal(0.02f));
						Vec3 localPos = thrusterPos + inThrusterPos;
						m_particles.AddParticle(
							ei::Vec3(m_ship.GetPosition() - m_particles.GetPosition()) + localPos,
							m_ship.GetVelocity() + thrustDir,
							max(0.2f, m_rng.Uniform(1.0f, 2.5f) - lensq(inThrusterPos) * 2.5f),
							Utils::Color8U(0.1f, 0.2f, 0.5f).RGBA());
					}
				}
			}
		}
	}

	void DriveSystem::OnAdd(const IVec3& _position, Voxel::ComponentType _type, uint8 _assignment)
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
		Vec3 center = _position + Vec3(0.5f);
		Vec3 centerDir = center - m_ship.GetCenter();
		float centerDistance = len(centerDir);
		Thruster newThruster;
		newThruster.relativePosition = centerDir;
		// Count number of samples per direction to normalize.
		int thrustCount[26] = {0};
		int torqueCount[26] = {0};
		byte freeThrustCount[26] = {0};
		Voxel::Model::ModelData::HitResult hit; // Dummy
		for(auto it = directionGenerator.begin(); it != endit; ++it)
		{
			// Do ray casts directly in model space
			Vec3 direction = it.direction();
			Ray ray(center, -direction);
			float distance = 100000.0f;
			ray.origin += ray.direction * 1.414213562f;
			
			float force = thrust;
			int idx = newThrust.getSplatIndex( direction );

			// Only if nothing is visible the drive can fire into this direction.
			// In occluded directions there is thrust=0. Because this results in bad
			// game play use 10% instead.
			if(m_ship.GetVoxelTree().RayCast( ray, 0, hit, distance ))
				force *= 0.1f;
			else
				++freeThrustCount[idx];

			// Divide the force into a rotation and a forward thrust.
			Vec3 axis = normalize(cross(centerDir, direction));	//// EI-CHECK normalize here could be wrong
			newThrust[idx][0] += force;
			newThrust[idx][1] += axis[0] * force;
			newThrust[idx][2] += axis[1] * force;
			newThrust[idx][3] += axis[2] * force;
			++thrustCount[idx];

			// Now assume direction is not the direction of force but the rotation axis.
			// Inverting the relation means, that axis is the direction in which we get
			// the most torque.
			ray = Ray(center, axis);
			distance = 100000.0f;
			force = thrust;
			if(m_ship.GetVoxelTree().RayCast( ray, 0, hit, distance ))
				force *= 0.1f;
			float torque = len(axis) * force;
			newTorque[idx][0] += torque;
			// Using axis is correct (it is the same direction as used in the ray cast).
			newTorque[idx][1] += force * axis[0];
			newTorque[idx][2] += force * axis[1];
			newTorque[idx][3] += force * axis[2];
			++torqueCount[idx];
		}
		// The bitmask and the cube-maps have different indices.
		// The map connects them both such that the results can be reused for the mask.
		static byte REMAP[26] = {25, 16, 8, 22, 13, 5, 19, 11, 2, 23, 14, 6, 20, 12, 3, 17, 9, 0, 24, 15, 7, 18, 10, 1, 21, 4};
		for(int i=0; i<26; ++i)
		{
			m_maxThrust[i] += newThrust[i] / max((float)thrustCount[i], 1e-6f);
			m_maxTorque[i] += newTorque[i] / max((float)torqueCount[i], 1e-6f);
			newThruster.freeField.setBit(REMAP[i], freeThrustCount[i] / float(thrustCount[i]) >= 0.5f);
		}
		m_thrusters.push_back(newThruster);
	}

	void DriveSystem::ClearSystem()
	{
		m_maxTorque = Math::SphericalFunction();
		m_maxThrust = Math::SphericalFunction();
		m_thrusters.clear();
	}

}