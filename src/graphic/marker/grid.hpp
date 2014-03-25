#pragma once

#include "wireframerenderer.hpp"

namespace Graphic {
namespace Marker {

	class Grid
	{
	public:
		/// \brief Create a new 2D grid in the xz-plane.
		/// \details The grid is placed around 0. A the border it will fade out
		///		smoothly.
		/// \param [in] _resolutionX Number of lines in x direction.
		/// \param [in] _resolutionZ Number of lines in z direction.
		/// \param [in] _spacing Distance between two lines.
		/// \param [in] _color The color of a maximal visible line. Fading can
		///		increase the transparency.
		///	\param [in] _elliptic Fade grid lines elliptic.
		Grid( int _resolutionX, int _resolutionZ, float _spacing, const Utils::Color32F& _color, bool _elliptic );

		/// \brief Create a new 3D grid.
		/// \details The grid is placed around 0. A the border it will fade out
		///		smoothly.
		/// \param [in] _resolutionX Number of lines in x direction.
		/// \param [in] _resolutionY Number of lines in y direction.
		/// \param [in] _resolutionZ Number of lines in z direction.
		/// \param [in] _spacing Distance between two lines.
		/// \param [in] _color The color of a maximal visible line. Fading can
		///		increase the transparency.
		Grid( int _resolutionX, int _resolutionY, int _resolutionZ, float _spacing, const Utils::Color32F& _color );

		/// \brief Draw all the lines with the glowing wireframe effect.
		void Draw( const Math::Mat4x4& _worldViewProjection );
	private:
		WireframeRenderer m_renderer;

		/// \brief Create a 2D grid at a specified y coordinate.
		void Create2DSlice( int _resolutionX, int _resolutionZ, float _coordinateY, float _spacing, bool _elliptic );
	};

} // namespace Marker
} // namespace Graphic