#pragma once

#include "../voxel/model.hpp"

namespace Generators {

	/// \brief Construct an asteroid model.
	class Asteroid: public Voxel::Model
	{
	public:
		/// \brief Generate the new asteroid model.
		// TODO: random generator...
		/// \param [in] _sizeX Maximum number of voxels in x direction.
		/// \param [in] _sizeY Maximum number of voxels in y direction.
		/// \param [in] _sizeZ Maximum number of voxels in z direction.
		Asteroid( int _sizeX, int _sizeY, int _sizeZ, int _seed );
	};
};