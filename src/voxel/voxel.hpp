#pragma once

#include <cstdint>

namespace Voxel {

	enum struct VoxelType: uint8_t {
		NONE,
		WATER,
		ROCK_1,
		ROCK_2,
		ROCK_3,
		ROCK_4,
	};

	/// \brief General information about a voxel of a specific type.
	/// \details Some voxel can have further attributes saved in other
	///		information structs.
	struct VoxelTypeInfo
	{
		float mass;					///< Mass of a voxel
		float activisionEnergy;		///< Energy required to destroy this voxel in Joule
		float reactionEnergy;		///< Energy released during destruction in Joule
		
		int water;					///< Amount of resource "water" required to build this voxel or dropped by mining.
		int minerals;				///< Amount of resource "mineral" required to build this voxel or dropped by mining.
		int metals;					///< Amount of resource "metal" required to build this voxel or dropped by mining.
		int rareMetals;				///< Amount of resource "rare metals" required to build this voxel or dropped by mining.
		int alloys;					///< Amount of resource "alloy" required to build this voxel or dropped by mining.
		int polymeres;				///< Amount of resource "polymere" required to build this voxel or dropped by mining.
	};
	
	/// \brief The mass if the voxel type in kg
	const VoxelTypeInfo VOXEL_INFO[] = {
		{ 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0},	// NONE
		{ 1.0f, 300000.0f, 0.0f, 10,0, 0, 0, 0, 0},	// WATER
	/*	{ 2.3f, ????, ????, 1, 8, 1, 0, 0, 0},	// ROCK_1
		{ 3.5f, ????, ????, 1, 6, 3, 0, 0, 0},	// ROCK_2
		{ 4.7f, ????, ????, 0, 5, 4, 1, 0, 0},	// ROCK_3
		{ 2.3f, ????, ????, 0, 4, 7, 0, 0, 0},	// ROCK_4*/
	};

	// TODO: move to ideas
	// Tramsformation: 1 Water + 1 Mineral -> 2 Polymere
	//				   4 Metal + 1 Rare Metal -> 5 Alloy
};