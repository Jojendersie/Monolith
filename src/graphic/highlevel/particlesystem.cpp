#include "particlesystem.hpp"
#include <algorithm>
#include "graphic/core/device.hpp"
#include "graphic/content.hpp"

using namespace ei;

namespace Graphic {
namespace ParticleSystems{

// ************************************************************************* //
// Manager class
std::vector<SystemActions*> Manager::m_registeredSystems;

void Manager::Register(class SystemActions* _system)
{
	// Insert sorted after render type to avoid many renderer switches
	m_registeredSystems.insert(
		std::upper_bound(m_registeredSystems.begin(), m_registeredSystems.end(), _system,
			[](const SystemActions* _lhs, const SystemActions* _rhs){ return _lhs->getRenderType() < _rhs->getRenderType(); }),
		_system
	);
}

void Manager::Release(class SystemActions* _system)
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
void Manager::Simulate(float _deltaTime)
{
	for(auto sys : m_registeredSystems)
	{
		sys->Simulate(_deltaTime);
	}
}


// ************************************************************************* //
// SystemAction class
void ParticleSystems::SystemActions::Draw()
{
	// Upload the data (position, //size) if available
//	UploadAndBind();

	// Set renderer and do draw call. The set has no effect if the renderer is
	// currently active.
	switch(m_renderer)
	{
	case RenderType::BLOB:
		Device::SetEffect(	Resources::GetEffect(Effects::BACKGROUNDSTAR) );
		// Manual draw call, because of special vertex definitions (independent buffers)
//		GL_CALL(glDrawArrays, GL_POINTS, 0, m_numParticles);
	}
}


// ************************************************************************* //
// Position Components
PositionComponents::PositionComponents()
{
	m_positions = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::VEC3, 0}}), false);
}

void PositionComponents::Add(const ei::Vec3& _pos)
{
	m_positions->Add(_pos);
}

void PositionComponents::Remove(size_t _idx)
{
	m_positions->Remove<ei::Vec3>((int)_idx);
}

} // namespace ParticleSystem
} // namespace Graphic