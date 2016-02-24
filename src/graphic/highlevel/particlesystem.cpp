#include "particlesystem.hpp"

using namespace ei;

namespace Graphic {

// ************************************************************************* //
// Manager class
std::vector<ParticleSystems::SystemActions*> ParticleSystems::Manager::m_registeredSystems;

void ParticleSystems::Manager::Register(class SystemActions* _system)
{
	m_registeredSystems.push_back(_system);
}

void ParticleSystems::Manager::Release(class SystemActions* _system)
{
	for(size_t i = 0; i < m_registeredSystems.size(); ++i)
	{
		if(m_registeredSystems[i] == _system)
		{
			m_registeredSystems[i] = m_registeredSystems.back();
			m_registeredSystems.pop_back();
			return;
		}
	}
}

// ************************************************************************* //
void ParticleSystems::Manager::Simulate(float _deltaTime)
{
	for(auto sys : m_registeredSystems)
	{
		sys->Simulate(_deltaTime);
	}
}

} // namespace Graphic