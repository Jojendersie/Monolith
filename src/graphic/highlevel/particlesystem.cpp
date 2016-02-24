#include "particlesystem.hpp"

using namespace ei;

namespace Graphic {

// ************************************************************************* //
// System class
std::vector<ParticleSystem::SubSystemActions*> ParticleSystem::m_registeredSystems;

void ParticleSystem::Simulate()
{
	for(auto sys : m_registeredSystems)
	{
		sys->Simulate();
	}
}

} // namespace Graphic