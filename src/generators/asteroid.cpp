#include "asteroid.hpp"
#include "../voxel/chunk.hpp"
#include "random.hpp"

using namespace Math;

namespace Generators {

	Asteroid::Asteroid( int _sizeX, int _sizeY, int _sizeZ, int _seed )
	{
		Random Noise(_seed);
		Random Rnd(_seed*1435461);
		int h = (int)(0.5*log(_sizeX*_sizeX + _sizeY*_sizeY + _sizeZ*_sizeZ));
		FOREACH_VOXEL(_sizeX, _sizeY, _sizeZ)
		{
			// Build ellipsoid base form
			Vec3 position(float(x)/_sizeX, float(y)/_sizeY, float(z)/_sizeZ);
			// (x-sx/2)/sx
			float d = lengthSq(position - 0.5f) - 0.25f;
			// Add noise
			for( int i=0; i<h; ++i )
				d += 0.15f * Noise.At( position * (4.0f * (1<<i)) ) / (1<<i);
			if( d < 0 )
			{
				Voxel::VoxelType type = Voxel::VoxelType(1);//Rnd.Uniform(1,2));
				Set( IVec3(x,y,z), 0, type );
				assert( type == Get( IVec3(x,y,z), 0 ) );
			}
		}

		//size_t test = m_voxelTree.MemoryConsumption();

//		for( int i=0; i<m_numChunks; ++i )
	//		m_chunks[i]->ComputeVertexBuffer();
	}
};