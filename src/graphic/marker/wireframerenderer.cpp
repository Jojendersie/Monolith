#include "wireframerenderer.hpp"
#include "../core/device.hpp"
#include "../content.hpp"

using namespace Math;

namespace Graphic {
namespace Marker {

	struct ColorPointVertex
	{
		Vec3 position;
		Utils::Color8U color;
	};

	// ********************************************************************* //
	WireframeRenderer::WireframeRenderer( const Utils::Color32F& _color, float _fading, Graphic::Content* _stateObjects ) :
		m_mesh( "pc", VertexBuffer::PrimitiveType::LINE ),
		m_color( _color ),
		m_fading( max(0.01f, _fading) ),
		m_lineWidth( 1.0f )
	{
		m_effect = &_stateObjects->beamEffect;
		m_objectUBO = &_stateObjects->wireObjectUBO;

		LOG_LVL0("Created new wireframe renderer");
	}

	// ********************************************************************* //
	void WireframeRenderer::AddSegment( const Vec3& _v1, const Vec3& _v2, const Utils::Color32F& _color1, const Utils::Color32F& _color2 )
	{
		ColorPointVertex vertex;
		vertex.position = _v1;
		vertex.color = _color1;
		m_mesh.Add( vertex );
		vertex.position = _v2;
		vertex.color = _color2;
		m_mesh.Add( vertex );
	}

	// ********************************************************************* //
	void WireframeRenderer::AddLine( const Vec3& _start, const Vec3& _end, float _transparency )
	{
		Utils::Color32F color(m_color.R(), m_color.G(), m_color.B(), _transparency * m_color.A());
		AddSegment( _start, _end, color, color );
	}

	// ********************************************************************* //
	void WireframeRenderer::AddLine( const Math::Vec3& _start, const Math::Vec3& _end, float _transparencyA, float _transparencyB )
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
	void WireframeRenderer::Draw( const Math::Mat4x4& _worldViewProjection )
	{
		// Updating somewhere else would require a finalize call or similar
		// for the WireframRenderer. I assume this if does not cost much.
		if( m_mesh.IsDirty() ) m_mesh.Commit();

		// Setup pipeline
		Device::SetEffect( *m_effect );
		(*m_objectUBO)["WorldViewProjection"] = _worldViewProjection;
		(*m_objectUBO)["LineWidth"] = m_lineWidth;
		(*m_objectUBO)["BlendSlope"] = 1.0f / m_fading;
		(*m_objectUBO)["BlendOffset"] = (1.0f-m_fading) / m_fading;

		Device::DrawVertices( m_mesh, 0, m_mesh.GetNumVertices() );
	}

} // namespace Marker
} // namespace Graphic