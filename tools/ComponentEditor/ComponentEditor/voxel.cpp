#include "voxel.h"

Voxel::Voxel(void):
	texture(nullptr),
	borderTexture(nullptr)
{
}


Voxel::~Voxel(void)
{
	if(texture) delete[] texture;
	if(borderTexture )delete[] borderTexture;
}

