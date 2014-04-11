#pragma once

#include "wireframerenderer.hpp"

namespace Graphic {
namespace Marker {

	class Box
	{
	public:
		/// \brief Create a wireframe box.
		/// \details The box is centered around 0. The fading allows to make
		///		the middle of edges more transparent than the borders
		///	\param [in] _fading A fading of 0.0 means constant transparency
		///		whereas 1.0 shows merely the corners.
		Box( const Math::Vec3& _size, float _fading, const Utils::Color32F& _color );

		/// \brief Draw all the lines with the glowing wireframe effect.
		void Draw( const Math::Mat4x4& _worldViewProjection );
	private:
		WireframeRenderer m_renderer;
	};

} // namespace Marker
} // namespace Graphic