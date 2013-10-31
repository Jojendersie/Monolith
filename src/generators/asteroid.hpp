#pragma once

// TODO: Change to model
#include "../voxel/chunk.hpp"

namespace Generators {

	/// \brief Construct an asteroid model.
	class Asteroid: public Voxel::Chunk
	{
	public:
		/// \brief Generate the new asteroid model.
		// TODO: random generator...
		/// \param [in] _iSizeX Maximum number of voxels in x direction.
		/// \param [in] _iSizeY Maximum number of voxels in y direction.
		/// \param [in] _iSizeZ Maximum number of voxels in z direction.
		Asteroid( int _iSizeX, int _iSizeY, int _iSizeZ );
	};
};