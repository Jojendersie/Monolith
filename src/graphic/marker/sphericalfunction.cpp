#include "graphic/marker/sphericalfunciton.hpp"
#include "graphic/core/device.hpp"
#include "utilities/color.hpp"
#include <ei/vector.hpp>
#include "math/sphericalfunction.hpp"
#include "graphic/content.hpp"

using namespace ei;

namespace Graphic {
namespace Marker {

	struct sfVertex
	{
		Vec3 position;
		//Vec3 normal;
		Utils::Color8U color;
	};

	static const Vec4 COLOR_GRADIENT[] = {
		Vec4(0.0f, 0.0f, 0.0f, 0.1f),
		Vec4(1.0f, 0.0f, 0.0f, 0.6f),
		Vec4(1.0f, 1.0f, 0.0f, 0.8f),
		Vec4(1.0f, 1.0f, 1.0f, 0.9f)
	};

	static Utils::Color8U ValueToColor( float _value )
	{
		// Some constant scale - hopefully all visualized functions are in the same scale
		_value *= 0.8f;
		if( _value <= 1.0f ) return Utils::Color8U( lerp( COLOR_GRADIENT[0], COLOR_GRADIENT[1], _value ) );
		if( _value <= 2.0f ) return Utils::Color8U( lerp( COLOR_GRADIENT[1], COLOR_GRADIENT[2], _value - 1.0f ) );
		return Utils::Color8U( lerp( COLOR_GRADIENT[2], COLOR_GRADIENT[3], _value - 2.0f ) );
	};

	// ********************************************************************* //
	static void SubDivide( const Math::SphericalFunction& _function,
		const Vec3& _v1,
		const Vec3& _v2,
		const Vec3& _v3,
		int _div, DataBuffer& _out )
	{
		if( _div == 0 )
		{
			sfVertex vertex;
			// Add triangle now
			// Pos, Normal, Skip Tex
			float f = _function(_v1)[0];
			vertex.position = _v1 * f; //vertex.normal = normalize(vertex.position);
			vertex.color = ValueToColor( f );
			_out.Add( vertex );
			f = _function(_v3)[0];
			vertex.position = _v3 * f; //vertex.normal = normalize(vertex.position);
			vertex.color = ValueToColor( f );
			_out.Add( vertex );
			f = _function(_v2)[0];
			vertex.position = _v2 * f; //vertex.normal = normalize(vertex.position);
			vertex.color = ValueToColor( f );
			_out.Add( vertex );
		} else {
			// Subdivision scheme:
			//			 3
			//			/\
			//		  6/__\5
			//		  /\  /\
			//		 /__\/__\ 
			//		1	 4   2
			// Calculate a new vertex on the edge. Then project it to the sphere by normalization.
			Vec3 v4 = normalize( Vec3( (_v1 + _v2) * 0.5f ) );
			Vec3 v5 = normalize( Vec3( (_v3 + _v2) * 0.5f ) );
			Vec3 v6 = normalize( Vec3( (_v3 + _v1) * 0.5f ) );

			// Recursion for the 4 new triangles
			--_div;
			SubDivide( _function, _v1,  v4, v6, _div, _out );
			SubDivide( _function,  v4, _v2, v5, _div, _out );
			SubDivide( _function,  v5, _v3, v6, _div, _out );
			SubDivide( _function,  v4,  v5, v6, _div, _out );
		}
	}

	// ********************************************************************* //
	// Create a visualization of a spherical function
	SphericalFunction::SphericalFunction( const Math::SphericalFunction& _function ) :
		m_mesh( VertexArrayBuffer::PrimitiveType::TRIANGLE_LIST, {{VertexAttribute::VEC3}, {VertexAttribute::COLOR}} )
	{
		// Create a tessellated sphere and project the vertices to the function
		// value.
		// Subdivide a tetrahedron
		Vec3 v1 = normalize( Vec3( 0.943f, 0.000f, -0.333f ) );
		Vec3 v2 = normalize( Vec3( -0.471f, 0.816f, -0.333f ) );
		Vec3 v3 = normalize( Vec3( 0.000f, 0.000f,  1.000f ) );
		Vec3 v4 = normalize( Vec3( -0.471f,-0.816f, -0.333f ) );
		auto vbGuard = m_mesh.GetBuffer(0);
		SubDivide( _function, v1, v2, v3, 5, *vbGuard );
		SubDivide( _function, v1, v4, v2, 5, *vbGuard );
		SubDivide( _function, v4, v1, v3, 5, *vbGuard );
		SubDivide( _function, v3, v2, v4, 5, *vbGuard );
	}

	// ********************************************************************* //
	// Draw the function visualization with an alpha rendering effect (without z-test).
	void SphericalFunction::Draw( const ei::Mat4x4& _worldViewProjection )
	{
		// Setup pipeline
		Resources::GetUBO(UniformBuffers::SIMPLE_OBJECT)["WorldViewProjection"] = _worldViewProjection;
		Device::SetEffect( Resources::GetEffect(Effects::ALPHA_BACK) );
		Graphic::Device::DrawVertices( m_mesh, 0, m_mesh.GetNumVertices() );
		Device::SetEffect( Resources::GetEffect(Effects::ALPHA_FRONT) );
		Graphic::Device::DrawVertices( m_mesh, 0, m_mesh.GetNumVertices() );
	}

} // namespace Marker
} // namespace Graphic