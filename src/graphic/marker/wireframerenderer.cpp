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
		m_fading( max(0.01f, _fading) )
	{
		m_effect = &_stateObjects->wireEffect;
		m_objectUBO = &_stateObjects->objectUBO;

		LOG_LVL0("Created new wireframe renderer");
	}

	// ********************************************************************* //
	void WireframeRenderer::AddSegment( const Vec3& _v1, const Vec3& _v2, const Utils::Color32F& _color )
	{
		ColorPointVertex vertex;
		vertex.position = _v1;
		vertex.color = _color;
		m_mesh.Add( vertex );
		vertex.position = _v2;
		m_mesh.Add( vertex );
	}

	// ********************************************************************* //
	void WireframeRenderer::AddLine( const Vec3& _start, const Vec3& _end, float _transparency )
	{
		AddSegment( _start, _end, Utils::Color32F(m_color.R(), m_color.G(), m_color.B(), _transparency * m_color.A()) );
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
		(*m_objectUBO)["Corner000"] = Vec4( (1.0f-m_fading) / m_fading, 1.0f / m_fading, 0.0f, 0.0f );

		Device::DrawVertices( m_mesh, 0, m_mesh.GetNumVertices() );
	}

} // namespace Marker
} // namespace Graphic