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
ParticleSystems::SystemActions::SystemActions() :
	m_particleVertices(VertexArrayBuffer::PrimitiveType::POINT)
{
}

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
	m_positions->Touch();
}

void PositionComponents::Remove(size_t _idx)
{
	m_positions->Remove<ei::Vec3>((int)_idx);
	m_positions->Touch();
}

void PositionComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_positions);
}

// ************************************************************************* //
// Color components
ColorComponents::ColorComponents()
{
	m_colors = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::COLOR, 3}}), false);
}

void ColorComponents::Add(const uint32& _pos)
{
	m_colors->Add(_pos);
	m_colors->Touch();
}

void ColorComponents::Remove(size_t _idx)
{
	m_colors->Remove<uint32>((int)_idx);
	m_colors->Touch();
}

void ColorComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_colors);
}

// System wide color
PSColorComponent::PSColorComponent()
{
	m_systemColor = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::COLOR, 3}}), false);
	// Default to black
	m_systemColor->Add<uint32>(0);
}

void PSColorComponent::SetColor(uint32 _color)
{
	*(uint32*)m_systemColor->GetDirectAccess() = _color;
	m_systemColor->Touch();
}

void PSColorComponent::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_systemColor);
}


} // namespace ParticleSystem
} // namespace Graphic