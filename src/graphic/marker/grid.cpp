#include "grid.hpp"

namespace Graphic {
namespace Marker {

	Grid::Grid( int _resolutionX, int _resolutionZ, float _spacing, const Utils::Color32F& _color, bool _elliptic, Graphic::Content* _stateObjects ) :
		m_renderer( _color, _elliptic ? 1.0f : (4.0f / (_resolutionX + _resolutionZ)), _stateObjects )
	{
		// Add grid lines along z axis (translated in x)
		for( int x = 0; x < _resolutionX; ++x )
		{
			float length = _spacing * (_resolutionZ + 1) * 0.5f;
			float transparent = 1.0f;
			float xOff = x - (_resolutionX - 1) * 0.5f;
			// The farther away from the center the smaller the circle-cut.
			if( _elliptic )
			{
				float r = xOff / (_resolutionX * 0.5f);
				transparent = sqrt(1.0f - r*r);
				length *= transparent;
			}
			m_renderer.AddLine( Math::Vec3(xOff * _spacing, 0.0f, -length),
								Math::Vec3(xOff * _spacing, 0.0f, length),
								transparent );
		}

		// Add grid lines along x axis (translated in z)
		for( int z = 0; z < _resolutionZ; ++z )
		{
			float length = _spacing * (_resolutionX + 1) * 0.5f;
			float transparent = 1.0f;
			float zOff = z - (_resolutionZ - 1) * 0.5f;
			// The farther away from the center the smaller the circle-cut.
			if( _elliptic )
			{
				float r = zOff / (_resolutionZ * 0.5f);
				transparent = sqrt(1.0f - r*r);
				length *= transparent;
			}
			m_renderer.AddLine( Math::Vec3(-length, 0.0f, zOff * _spacing),
								Math::Vec3(length, 0.0f, zOff * _spacing),
								transparent );
		}
	}

	void Grid::Draw( const Math::Mat4x4& _worldViewProjection )
	{
		m_renderer.Draw( _worldViewProjection );
	}

} // namespace Marker
} // namespace Graphic