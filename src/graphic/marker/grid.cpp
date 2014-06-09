#include "grid.hpp"

namespace Graphic {
namespace Marker {

	// ********************************************************************* //
	// Create a new 2D grid in the xz-plane.
	Grid::Grid( int _resolutionX, int _resolutionZ, float _spacing, const Utils::Color32F& _color, bool _elliptic ) :
		m_renderer( _color, _elliptic ? 1.0f : (4.0f / (_resolutionX + _resolutionZ)) )
	{
		Create2DSlice( _resolutionX, _resolutionZ, 0.0f, _spacing, _elliptic );
		m_renderer.Commit();
	}

	// ********************************************************************* //
	// Create a new 3D grid.
	Grid::Grid( int _resolutionX, int _resolutionY, int _resolutionZ, float _spacing, const Utils::Color32F& _color ) :
		m_renderer( _color, 6.0f / (_resolutionY + _resolutionX + _resolutionZ) )
	{
		// Create lots of 2D grids
		for( int y = 0; y < _resolutionZ; ++y )
		{
			float yOff = y - (_resolutionY - 1) * 0.5f;
			Create2DSlice( _resolutionX, _resolutionZ, yOff * _spacing, _spacing, false );
		}

		// Add grid lines along y axis (translated in x and z)
		for( int z = 0; z < _resolutionZ; ++z )
		for( int x = 0; x < _resolutionX; ++x )
		{
			float length = _spacing * (_resolutionY + 1) * 0.5f;
			float xOff = x - (_resolutionX - 1) * 0.5f;
			float zOff = z - (_resolutionZ - 1) * 0.5f;
			m_renderer.AddLine( Math::Vec3(xOff * _spacing, -length, zOff * _spacing),
				Math::Vec3(xOff * _spacing, length, zOff * _spacing),
				1.0f );
		}
		m_renderer.Commit();
	}

	// ********************************************************************* //
	// Draw all the lines with the glowing wireframe effect.
	void Grid::Draw( const Math::Mat4x4& _worldViewProjection )
	{
		m_renderer.Draw( _worldViewProjection );
	}

	// ********************************************************************* //
	// Create a 2D grid at a specified y coordinate.
	void Grid::Create2DSlice( int _resolutionX, int _resolutionZ, float _coordinateY, float _spacing, bool _elliptic )
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
			m_renderer.AddLine( Math::Vec3(xOff * _spacing, _coordinateY, -length),
				Math::Vec3(xOff * _spacing, _coordinateY, length),
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
			m_renderer.AddLine( Math::Vec3(-length, _coordinateY, zOff * _spacing),
				Math::Vec3(length, _coordinateY, zOff * _spacing),
				transparent );
		}
	}

} // namespace Marker
} // namespace Graphic