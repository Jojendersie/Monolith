#include "asteroid.hpp"
#include "random.hpp"

using namespace Math;

namespace Generators {

	Asteroid::Asteroid( int _sizeX, int _sizeY, int _sizeZ, int _seed )
	{
		Random Noise(_seed);
		Random Rnd(_seed*1435461);
		// Build ellipsoide base form
		FOREACH_VOXEL(_sizeX, _sizeY, _sizeZ)
		{
			Vec3 position(float(x)/_sizeX, float(y)/_sizeY, float(z)/_sizeZ);
			// (x-sx/2)/sx
			float d = sqr( position.x - 0.5f ) + sqr( position.y - 0.5f ) + sqr( position.z - 0.5f ) - 0.25f;
			for( int i=0; i<3; ++i )
				d += 0.15f * Noise.At( position * (4.0f * (1<<i)) ) / (1<<i);
			if( d < 0 )
				Set( IVec3(x,y,z), 5, Voxel::VoxelType(Rnd.Uniform(1,2)) );
		}

		for( int i=0; i<m_numChunks; ++i )
			m_chunks[i]->ComputeVertexBuffer();
	}
};