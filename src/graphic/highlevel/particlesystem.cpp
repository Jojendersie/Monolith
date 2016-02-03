#include "particlesystem.hpp"

using namespace ei;

namespace Graphic {

// ************************************************************************* //
// System class
/*int ParticleSystem::AddParticle(const Vec3& _position, const Vec3& _scale)
{
	int id = (int)m_particles.size();
	m_partPositions.push_back(_position);
	m_partScale.push_back(_position);
	// Append a component free particle
	// TODO: correct initialized?
	m_particles.push_back(Particle());
	return id;
}

void ParticleSystem::AddComponent(int _particleID, const VeloctiyComponent& _component)
{
	m_particles[_particleID].components.PushBack(_component);
	m_coVelocity.push_back(_component);
	m_coVelocity.back().particleID = _particleID;
}

void ParticleSystem::AddComponent(int _particleID, const RotationComponent& _component)
{
	m_particles[_particleID].components.PushBack(_component);
	m_coRotation.push_back(_component);
	m_coRotation.back().particleID = _particleID;
}

void ParticleSystem::AddComponent(int _particleID, const PointSpawnerComponent& _component)
{
	m_particles[_particleID].components.PushBack(_component);
	m_coPointSpawner.push_back(_component);
	m_coPointSpawner.back().particleID = _particleID;
}

void ParticleSystem::AddComponent(int _particleID, const BoxSpawnerComponent& _component)
{
	m_particles[_particleID].components.PushBack(_component);
	m_coBoxSpawner.push_back(_component);
	m_coBoxSpawner.back().particleID = _particleID;
}*/

} // namespace Graphic