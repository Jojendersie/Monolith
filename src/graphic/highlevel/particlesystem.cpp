#include "particlesystem.hpp"

using namespace ei;

namespace Graphic {

// ************************************************************************* //
// System class
std::vector<ParticleSystem::SubSystemActions*> ParticleSystem::m_registeredSystems;

void ParticleSystem::Simulate(float _deltaTime)
{
	for(auto sys : m_registeredSystems)
	{
		sys->Simulate(_deltaTime);
	}
}

} // namespace Graphic