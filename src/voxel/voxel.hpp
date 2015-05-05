#pragma once

#include <cstdint>
#include <string>
#include "material.hpp"
#include "math/vector.hpp"

namespace Graphic {
	class Texture;
}

namespace Voxel {

	enum struct ComponentType: uint8_t {
		UNDEFINED,
		WATER,
		COMPUTER,
		STONE,
		SOLAR_CELL,
		ION_THRUSTER,
		LASER
	};

	/// \brief A class which provides runtime-constant data about voxel types.
	/// \details The data is loaded from a file named voxel.json.
	class TypeInfo
	{
	public:
		/// \brief Constructs the one instance and calls load
		static void Initialize();

		/// \brief Data must be loaded at startup. This function can be called
		///		to reload the file.
		/// \throws std::string
		static void Load();

		/// \brief Must be called to release the memory allocated by the type information.
		static void Unload();

		/// \brief Sample the voxel material of the type at a target level.
		/// \details This also samples the 'border texture' which provides a
		///		fusion/transition of neighbored voxels. This texture is mostly
		///		defined to allow a smooth texture in directions where no
		///		neighbor is and a solid transition otherwise.
		///		
		///		The border texture will be rotated/mirrored to the correct side
		///		and can therefore be used for alignment too. E.g. A tree which
		///		stands on its border-side. If there are multiple neighbors the
		///		union of all border and normal textures is calculated - so be
		///		careful with trees.
		/// \param [in] _type The type of the component voxel.
		/// \param [in] _position A position inside the target levels 3D array.
		/// \param [in] _level 0 Is the root of the mip map chain (1x1x1).
		///		Positive numbers go to finer maps. If the number is larger than
		///		the highest possible mip map a sample from a coarser map is used.
		///	\param [in] _rootSurface Flags for the neighborhood. A 1 causes a
		///		sampling of the rotated/mirrored border texture in this direction.
		///	\param [out] _materialOut The material at the target position.
		///	\param [out] _surfaceOut The surface properties at the target
		///		position.
		///	\return The voxel is valid (defined and surface voxel).
		static bool Sample( ComponentType _type, Math::IVec3 _position, int _level, uint8_t _rootSurface, Material& _materialOut, uint8_t& _surfaceOut );

		/// \brief Returns the number of mip maps for the given type's materials.
		static int GetMaxLevel( ComponentType _type );

		/// \brief Returns if the volume texture is solid or not.
		static bool IsSolid( ComponentType _type );

		/// \brief Returns the mass of the voxel.
		/// TODO: use integer(64?) mass
		static float GetMass( ComponentType _type );
		
		/// \brief Returns the thresholdEnergy of the voxel.
		static float GetThresholdEnergy( ComponentType _type );
		
		/// \brief Returns the reactionEnergy of the voxel.
		static float GetReactionEnergy( ComponentType _type );
		
		/// \brief Returns the amount of hydrogen the voxel consists of voxel.
		static int GetHydrogen( ComponentType _type );

		/// \brief Returns the amount of carbon the voxel consists of voxel.
		static int GetCarbon( ComponentType _type );

		/// \brief Returns the amount of metal the voxel consists of voxel.
		static int GetMetals( ComponentType _type );

		/// \brief Returns the amount of rareEarthElements the voxel consists of voxel.
		static int GetRareEarthElements( ComponentType _type );

		/// \brief Returns the amount of semiconductors the voxel consists of voxel.
		static int GetSemiconductors( ComponentType _type );

		/// \brief Returns the amount of Heisenbergium the voxel consists of voxel.
		static int GetHeisenbergium( ComponentType _type );

		/// \brief Get the energy produced by this voxel in [J/s]
		static float GetEnergyOut( ComponentType _type );

		/// \brief Get the storage volume of this voxel
		static float GetStorageVolume(ComponentType _type);

		/// \brief Returns the name of the voxel
		static std::string GetName( ComponentType _type );

		static int GetNumVoxels();

		/// \brief Set the internal voxel texture array as TEXTURE0
		static void BindVoxelTextureArray();
	private:
		/// \brief singleton constructor - calls load.
		TypeInfo();
		~TypeInfo();

		struct MatSample
		{
			Material material;
			uint8_t surface;

			MatSample() : material(Material::UNDEFINED), surface(0)	{}
		};

		/// \brief General information about a voxel of a specific type.
		struct ComponentTypeInfo
		{
			float mass;					///< Mass of a voxel
			float thresholdEnergy;		///< Energy required to destroy this voxel in Joule
			float reactionEnergy;		///< Energy released during destruction in Joule

			int hydrogen;				///< Amount of resource "hydrogen" required to build this voxel or dropped by mining.
			int carbon;					///< Amount of resource "carbon" required to build this voxel or dropped by mining.
			int metals;					///< Amount of resource "metal" required to build this voxel or dropped by mining.
			int rareEarthElements;		///< Amount of resource "rare earth element" required to build this voxel or dropped by mining.
			int semiconductors;			///< Amount of resource "semiconductor" required to build this voxel or dropped by mining.
			int heisenbergium;			///< Amount of resource "heisenbergium" required to build this voxel or dropped by mining.

			int textureResolution;		///< Number of pixels in one dimension of the volume "texture"
			int numMipMaps;				///< Number of generated mip maps
			bool isSolid;
			MatSample* texture;			///< Main texture used for sampling of the material-sub-voxels
			MatSample* borderTexture;	///< A texture which is added on each side of this voxel (rotated) which has a neighbor. The coordinate system is on x axis.

			float energyOut;			///< Energy produced by this voxel in [J/s]
			float storageVolume;		///< Amount of storage a single voxel can hold

			std::string name;			///< The name of this voxel type

			ComponentTypeInfo();
			~ComponentTypeInfo();
		};

		ComponentTypeInfo* m_voxels;		///< Array with one entry for each voxel
		int m_numVoxels;					///< Number of known voxels
		Graphic::Texture* m_voxelTextures;	///< Texture array with "volumetric" (N*N)xN texture

		/// \brief Compute the mip maps for a border or standard texture.
		/// \details The texture memory must have a size of ´_edge^3 + (_edge/2)^3 ...´
		///		where the first ´_edge^3´ voxels must be defined. The sum of
		///		required space can be computed by the formula: ´((_edge^3) * 8) / 7´.
		/// \param [in] _texture An array with space for all mip map levels
		/// \param [in] _edge Length of a single edge.
		/// \return Number of generated levels.
		static int GenerateMipMap( MatSample* _texture, int _edge );

		/// \brief Compute the surface flags for each sample (all mip maps)
		/// \param [in] _default Defines if voxels at the border are surface or
		///		not. Using true will create faces at the very outside.
		static void GenerateSurfaceInfo( MatSample* _texture, int _edge, bool _default );

		/// \brief Compute the array access index and offsets
		/// \param [inout] _position The position inside the mip map level.
		///		The position can be changed if upsampling occurs.
		/// \param [in] _level The mip map level to sample
		/// \param [out] _edge Length of a single edge.
		/// \param [out] _offset Offset for the mip map level.
		/// \return The full index of the voxel or an upsampled voxel position.
		static int SamplePos( ComponentType _type, Math::IVec3& _position, int _level, int& _edge, int& _offset );

		/// \brief Bake all the voxels inclusive mip-maps in the texture array.
		void GenerateTexture();
	};

	/// \brief General information about a voxel of a specific type.
	/// \details This struct is going to be deleted after new rendering is introduced.
	/*struct VoxelTypeInfo
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
		{ 1.0f, 334000.0f, 0.0f, 10,0, 0, 0, 0, 0, 0x86DCCB29},	// WATER
		{ 2.3f, 460000.0f, 80000.0f, 1, 8, 1, 0, 0, 0, 0x07BC0632},	// ROCK_1
		{ 3.5f, 700000.0f, 60000.0f, 1, 6, 3, 0, 0, 0, 0x0},	// ROCK_2
		{ 4.7f, 940000.0f, 50000.0f, 0, 5, 4, 1, 0, 0, 0x0},	// ROCK_3
		{ 2.6f, 780000.0f, 45000.0f, 0, 4, 7, 0, 0, 0, 0x0},	// ROCK_4
	};//*/


	/// \brief Models are represented by a material and voxel-type tree in one.
	/// \details A voxel is just a volumetric representation in contrast to components
	///		which are functional units.
#	pragma pack(push, 1)
	struct Voxel
	{
		Material material;		///< Graphical representation
		ComponentType type;			///< The type of the voxel
		uint8_t dirty: 1;		///< Somebody changed a child or this node
		uint8_t solid: 1;		///< This node and all its children are defined
		uint8_t surface: 6;	///< One flag for each direction if there is no solid neighborhood

		/// \brief Standard constructor creates undefined element
		Voxel() : material(Material::UNDEFINED), type(ComponentType::UNDEFINED), dirty(0), solid(0), surface(0)	{}

		/// \brief Construct a component with a defined type and undefined material
		Voxel(ComponentType _type) : type(_type), dirty(1), solid(TypeInfo::IsSolid(_type)?1:0), surface(0) { uint8_t dummy; TypeInfo::Sample(_type, Math::IVec3(0), 0, 0, material, dummy); }

		/// \brief Mark this component as outdated (it is set to undefined)
		void Touch()			{ dirty = 1; }
		bool IsDirty() const	{ return dirty == 1; }

		/// \brief Undefined material and type.
		static const Voxel UNDEFINED;

		/// \brief Checks if type is equal
		bool operator == (const Voxel& _mat) const		{ return type == _mat.type; }
		/// \brief Checks if type is not equal
		bool operator != (const Voxel& _mat) const		{ return type != _mat.type; }
	};
#	pragma pack(pop)
};