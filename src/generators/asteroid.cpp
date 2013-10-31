#include "asteroid.hpp"

using namespace Math;

namespace Generators {

	Asteroid::Asteroid( int _iSizeX, int _iSizeY, int _iSizeZ )
	{
		// Build ellipsoide base form
		FOREACH_VOXEL(_iSizeX, _iSizeY, _iSizeZ)
		{
			// (x-sx/2)/sx
			float d = sqr( float(x)/_iSizeX - 0.5f ) + sqr( float(y)/_iSizeY - 0.5f ) + sqr( float(z)/_iSizeZ - 0.5f ) - 0.25f;
			if( d < 0 )
				Set( IVec3(x,y,z), 5, Voxel::VoxelType::ROCK_1 );
		}
	}
};