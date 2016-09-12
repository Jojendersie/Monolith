#include "particlesystem.hpp"
#include <algorithm>
#include "graphic/core/device.hpp"
#include "graphic/content.hpp"
#include "input/camera.hpp"

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

void Manager::Draw( const Input::Camera& _camera )
{
	RenderType currentEffect = RenderType::INVALID;
	for(auto sys : m_registeredSystems)
	{
		// The draw call switches the effect automatically. Since systems are
		// sorted this should not happen more than once in each frame for each PS type.

		// TODO: culling
		sys->Draw( _camera );
	}
}


// ************************************************************************* //
// SystemAction class
ParticleSystems::SystemActions::SystemActions() :
	m_particleVertices(VertexArrayBuffer::PrimitiveType::POINT),
	m_systemTransformation()
{
	m_particleVertices.SetNumInstances(1);
}

void ParticleSystems::SystemActions::Draw( const Input::Camera& _camera )
{
	if(!GetNumParticles()) return;

	// Upload current transformation matrix
	Mat4x4 modelViewProj = m_systemTransformation.GetTransformation(_camera.Transformation());
	modelViewProj = _camera.GetProjection() * modelViewProj;
	Resources::GetUBO(UniformBuffers::SIMPLE_OBJECT)["WorldViewProjection"] = modelViewProj;

	// Set renderer and do draw call. The set has no effect if the renderer is
	// currently active.
	switch(m_renderer)
	{
	case RenderType::BLOB:
		Device::SetEffect(	Resources::GetEffect(Effects::BLOB_PARTICLE) );
	}
	m_particleVertices.Bind();
	Graphic::Device::DrawVertices( m_particleVertices, 0, GetNumParticles() );
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

void ColorComponents::Add(uint32 _color)
{
	m_colors->Add(_color);
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
	m_systemColor->Touch();
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


// ************************************************************************* //
// Size components
SizeComponents::SizeComponents()
{
	m_sizes = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::FLOAT, 1}}), false);
}

void SizeComponents::Add(float _size)
{
	m_sizes->Add(_size);
	m_sizes->Touch();
}

void SizeComponents::Remove(size_t _idx)
{
	m_sizes->Remove<float>((int)_idx);
	m_sizes->Touch();
}

void SizeComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_sizes);
}

PSSizeComponent::PSSizeComponent()
{
	m_systemSize = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::FLOAT, 1}}), true);
	// Default to 0.5 (well visible)
	m_systemSize->Add<float>(0.5f);
	m_systemSize->Touch();
}

void PSSizeComponent::SetParticleSize(float _size)
{
	*(float*)m_systemSize->GetDirectAccess() = _size;
	m_systemSize->Touch();
}

void PSSizeComponent::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_systemSize);
}


} // namespace ParticleSystem
} // namespace Graphic