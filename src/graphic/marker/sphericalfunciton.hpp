#pragma once

#include "graphic/core/vertexbuffer.hpp"

namespace Graphic {
namespace Marker {

	class SphericalFunction
	{
	public:
		/// \brief Create a visualization of a spherical function
		SphericalFunction( const Math::SphericalFunction& _function );

		/// \brief Draw the function visualization with an alpha rendering
		///		effect (without z-test).
		void Draw( const ei::Mat4x4& _worldViewProjection );
	private:

		VertexBuffer m_mesh;
	};

} // namespace Marker
} // namespace Graphic