#include "wireframerenderer.hpp"
#include "../core/device.hpp"
#include "../content.hpp"

using namespace ei;

namespace Graphic {
namespace Marker {

	struct ColorPointVertex
	{
		Vec3 position;
		Utils::Color8U color;
	};

	// ********************************************************************* //
	WireframeRenderer::WireframeRenderer( const Utils::Color32F& _color, float _fading ) :
		m_mesh( VertexArrayBuffer::PrimitiveType::LINE, {{VertexAttribute::VEC3, 0}, {VertexAttribute::COLOR, 3}} ),
		m_color( _color ),
		m_fading( max(0.01f, _fading) ),
		m_lineWidth( 1.0f )
	{
		LOG_LVL0("Created new wireframe renderer");
	}

	// ********************************************************************* //
	void WireframeRenderer::AddSegment( const Vec3& _v1, const Vec3& _v2, const Utils::Color32F& _color1, const Utils::Color32F& _color2 )
	{
		auto vbGuard = m_mesh.GetBuffer(0);
		ColorPointVertex vertex;
		vertex.position = _v1;
		vertex.color = _color1;
		vbGuard->Add( vertex );
		vertex.position = _v2;
		vertex.color = _color2;
		vbGuard->Add( vertex );
	}

	// ********************************************************************* //
	void WireframeRenderer::AddLine( const Vec3& _start, const Vec3& _end, float _transparency )
	{
		Utils::Color32F color(m_color.R(), m_color.G(), m_color.B(), _transparency * m_color.A());
		AddSegment( _start, _end, color, color );
	}

	// ********************************************************************* //
	void WireframeRenderer::AddLine( const Vec3& _start, const Vec3& _end, float _transparencyA, float _transparencyB )
	{
		Utils::Color32F colorA(m_color.R(), m_color.G(), m_color.B(), _transparencyA * m_color.A());
		Utils::Color32F colorB(m_color.R(), m_color.G(), m_color.B(), _transparencyB * m_color.A());
		AddSegment( _start, _end, colorA, colorB );
	}

	// ********************************************************************* //
	void WireframeRenderer::AddCircle( const Vec3& _center, const Vec3& _periphery, int _numPoints )
	{
	}

	// ********************************************************************* //
	void WireframeRenderer::Commit()
	{
		// Force upload now
		m_mesh.Bind();
	}

	// ********************************************************************* //
	void WireframeRenderer::Draw( const Mat4x4& _worldViewProjection )
	{
		// Setup pipeline
		Device::SetEffect( Resources::GetEffect(Effects::BEAM) );
		Resources::GetUBO(UniformBuffers::OBJECT_WIRE)["WorldViewProjection"] = _worldViewProjection;
		Resources::GetUBO(UniformBuffers::OBJECT_WIRE)["LineWidth"] = m_lineWidth;
		Resources::GetUBO(UniformBuffers::OBJECT_WIRE)["BlendSlope"] = 1.0f / m_fading;
		Resources::GetUBO(UniformBuffers::OBJECT_WIRE)["BlendOffset"] = (1.0f-m_fading) / m_fading;

		Device::DrawVertices( m_mesh, 0, m_mesh.GetNumVertices() );
	}

} // namespace Marker
} // namespace Graphic