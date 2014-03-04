#pragma once

#include <cstdint>

namespace Voxel {

	enum struct VoxelType: uint8_t {
		UNDEFINED,
		WATER,
		ROCK_1,
		ROCK_2,
		ROCK_3,
		ROCK_4
	};

	/// \brief A class which provides runtime-constant data about voxel types.
	/// \details The data is loaded from a file named voxel.json.
	/*class TypeInfo
	{
	public:
		/// \brief Data must be loaded at startup. This function can be called
		///		to reload the file.
		/// \throws std::string
		static void Load();
	private:

		/// \brief General information about a voxel of a specific type.
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

			int textureResolution;		///< Number of pixels in one dimension of the volume "texture"
			Material* texture;			///< Main texture used for sampling of the material-sub-voxels
			Material* textureBorder;	///< A texture which is added on each side of this voxel (rotated) which has a neighbor. The coordinate system is on x axis.
		};
	};*/

	/// \brief General information about a voxel of a specific type.
	/// \details This struct is going to be deleted after new rendering is introduced.
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

		uint32_t material;
	};
	
	/// \brief The mass if the voxel type in kg
	const VoxelTypeInfo VOXEL_INFO[] = {
		{ 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0, 0x0},	// NONE
		{ 1.0f, 334000.0f, 0.0f, 10,0, 0, 0, 0, 0, 0x05E80057},	// WATER
		{ 2.3f, 460000.0f, 80000.0f, 1, 8, 1, 0, 0, 0, 0x04240052},	// ROCK_1
		{ 3.5f, 700000.0f, 60000.0f, 1, 6, 3, 0, 0, 0, 0x0},	// ROCK_2
		{ 4.7f, 940000.0f, 50000.0f, 0, 5, 4, 1, 0, 0, 0x0},	// ROCK_3
		{ 2.6f, 780000.0f, 45000.0f, 0, 4, 7, 0, 0, 0, 0x0},	// ROCK_4
	};

	// TODO: move to ideas
	// Tramsformation: 1 Water + 1 Mineral -> 2 Polymere
	//				   4 Metal + 1 Rare Metal -> 5 Alloy


	inline bool IsSolid( VoxelType _v )	{ return _v != VoxelType::UNDEFINED; }
};