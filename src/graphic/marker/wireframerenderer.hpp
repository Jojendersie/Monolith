#pragma once

#include "../../predeclarations.hpp"
#include "../../utilities/color.hpp"
#include "../core/vertexbuffer.hpp"

namespace Graphic {
namespace Marker {

	class WireframeRenderer
	{
	public:
		/// \brief Create a new empty wireframe renderer and setup the effect.
		/// \param [in] _color The color of a maximal visible line. Fading can
		///		increase the transparency.
		///	\param [in] _fading A relative fading length. 0 means hard ends and
		///		1 a fading till the middle point from both directions.
		///		This option is ignored for cicles
		WireframeRenderer( const Utils::Color32F& _color, float _fading );

		/// \brief Add a single line with or without fading.
		///	\param [in] _start One end of the line. Permuting the points does
		///		not change the outcome.
		///	\param [in] _end The other end of the line.
		///	\param [in] _transparency Use color transparency * _transparency.
		void AddLine( const Math::Vec3& _start, const Math::Vec3& _end, float _transparency );

		/// \copydoc AddLine
		/// \param [in] _transparencyA Use color transparency * _transparency at start point.
		/// \param [in] _transparencyB Use color transparency * _transparency at end point.
		void AddLine( const Math::Vec3& _start, const Math::Vec3& _end, float _transparencyA, float _transparencyB );

		/// \brief Add a circular line.
		/// \param [in] _center The center of the circle
		/// \param [in] _periphery One arbitrary point on the circle.
		/// \param [in] _numPoints The number of points on the circle - the
		///		more the smoother.
		void AddCircle( const Math::Vec3& _center, const Math::Vec3& _periphery, int _numPoints );

		/// \brief Draw all the lines with the glowing wireframe effect.
		void Draw( const Math::Mat4x4& _worldViewProjection );

		void SetLineWidth( float _width )	{ m_lineWidth = _width; }
		float GetLineWidth() const			{ return m_lineWidth; }

	private:
		/// \brief A buffer with line geometry
		VertexBuffer m_mesh;
		Utils::Color32F m_color;		// The color of the lines.
		float m_fading;					// How fast are lines faded out. This number is relative to the line. A 1.0 means to fade from each end to the middle.
		float m_lineWidth;				// Thickness of the beam

		/// \brief Creates a line segment in the vertex buffer.
		void AddSegment( const Math::Vec3& _v1, const Math::Vec3& _v2, const Utils::Color32F& _color1, const Utils::Color32F& _color2 );
	};

} // namespace Marker
} // namespace Graphic