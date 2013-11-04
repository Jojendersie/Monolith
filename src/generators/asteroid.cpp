#include "asteroid.hpp"

using namespace Math;

namespace Generators {

	Asteroid::Asteroid( int _sizeX, int _sizeY, int _sizeZ )
	{
		// Build ellipsoide base form
		FOREACH_VOXEL(_sizeX, _sizeY, _sizeZ)
		{
			// (x-sx/2)/sx
			float d = sqr( float(x)/_sizeX - 0.5f ) + sqr( float(y)/_sizeY - 0.5f ) + sqr( float(z)/_sizeZ - 0.5f ) - 0.25f;
			if( d < 0 )
				Set( IVec3(x,y,z), 5, Voxel::VoxelType::ROCK_1 );
		}
	}
};