#pragma once

#include <cstdint>
#include <string>
#include "material.hpp"
#include "ei/vector.hpp"

namespace Graphic {
	class Texture;
}

namespace Voxel {

	enum struct ComponentType: uint16 {
		UNDEFINED,
		INVISIBLE,
		WATER,
		COMPUTER,
		STONE,
		SOLAR_CELL,
		ION_THRUSTER_M1,
		LASER,
		STANDARD_STORE,
		STEEL_HULL_SL,
		STEEL_HULL_DL,
		SCANNER,
		ION_BOOSTER_M1,
		ION_BOOSTER_M2,
		ION_BOOSTER_M3,
		BATTERY_M3,
		BATTERY_M2,
	};

	enum struct ComponentClass {
		BATTERY,
		COMPUTER,
		DRIVE,
		REACTOR,
		SENSOR,
		SHIELD,
		STATIC,
		STORAGE,
		WEAPON,
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

		/// \brief Get the material code of the 1x1x1 mip-map.
		static Material GetMaterial( ComponentType _type );

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

		/// \brief Get the energy produced by this voxel in [kJ/s]
		static float GetEnergyOut( ComponentType _type );

		/// \brief Maximum energy usage of the component [kJ/s].
		static float GetEnergyDrain(ComponentType _type);

		/// \brief Capacity of energy reservoir in [kJ].
		static float GetCapacity(ComponentType _type);

		/// \brief Get the storage volume of this voxel
		static float GetStorageVolume(ComponentType _type);

		/// \brief Get the maximum thrust of this voxel in [kN].
		static float GetThrust(ComponentType _type);

		/// \brief Damage per shot in [kJ].
		static float GetDamage(ComponentType _type);

		/// \brief Range of a weapon in [] or range of a sensor.
		static float GetRange(ComponentType _type);

		/// \brief Speed of any kind of shots in [/s].
		static float GetProjectileSpeed(ComponentType _type);

		/// \brief [hit points/s].
		static float GetShieldRegeneration(ComponentType _type);

		/// \brief The component which is spawned and regenerate by the shield.
		static float GetShieldComponentType(ComponentType _type);

		/// \brief Number of supportable storage components [#vox].
		static float GetLiveSupport(ComponentType _type);

		/// \brief Returns the name of the voxel
		static std::string GetName( ComponentType _type );

		/// \brief Get the basic main functional direction. This is 0 for
		///		omni-directional components.
		static const ei::Vec3& GetMainDir( ComponentType _type );

		static int GetNumVoxels();

		/// \brief Set the internal voxel texture array as TEXTURE0
		static void BindVoxelTextureArray();

		/// \brief Get general type
		static bool IsComputer( ComponentType _type );
		static bool IsStorage( ComponentType _type );
		static bool IsReactor( ComponentType _type );
		static bool IsBattery( ComponentType _type );
		static bool IsDrive( ComponentType _type );
		static bool IsWeapon( ComponentType _type );
		static bool IsShield( ComponentType _type );
		static bool IsSensor( ComponentType _type );
	private:
		/// \brief singleton constructor - calls load.
		TypeInfo();
		~TypeInfo();

		struct MatSample
		{
			Material material;
			uint8 surface;

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

			float energyOutput;			///< Maximum energy produced by this voxel in [kJ/s]
			float energyDrain;			///< Maximum energy consumed by this voxel in [kJ/s]
			float storageVolume;		///< Amount of storage a single voxel can hold
			float capacity;				///< Capacity of energy reservoir in [kJ]
			float damage;				///< Damage per shot in [kJ]
			float cooldown;				///< Time in [s] between shots at maximum energy supply. If energy supply is smaller the cooldown gets longer.
			float range;				///< Range of a weapon in [] or range of a sensor. For sensors the range increases relatively if multiple components are working together.
			float projectileSpeed;		///< Speed of any kind of shots in [/s]
			float thrust; 				///< [kN] at maximum energy supply.
			float shieldRegeneration;	///< [hit points/s]
			uint8 shieldComponentType;	///< The component which is spawned and regenerate by the shield.
			float lifeSupport;			///< Number of supportable storage components [#vox]
			ei::Vec3 mainDir;			///< Main functional direction or 0 for omni-directional components

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
		static int SamplePos( ComponentType _type, ei::IVec3& _position, int _level, int& _edge, int& _offset );

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

		uint32 material;
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
		uint16_t health;		///< Hit points until destruction (0 and less). One hit point is approximating 30kJ
		ComponentType type;		///< The type of the voxel
		uint8 dirty: 1;			///< Somebody changed a child or this node
		uint8 solid: 1;			///< This node and all its children are defined
		uint8 surface: 6;		///< One flag for each direction if there is no solid neighborhood
		uint8 sysAssignment;	///< Used from outside to determine an assignment to different ship systems.
		/// \brief A rotation code to transform direction vectors fast.
		/// \details Bits are interpreted as follows:
		///		0-1 swap index for the x component. I.e dir.x = dir[si.x].
		///		2-3 swap index for y (si.y).
		///		z is indirectly given by (3-(si.x+si.y)).
		///		4-6 switch sign for each component.
		///		A full direction can be rotated by:
		///		dir = Vec3(r&0x10?-1:1 * dir[r&0x3], r&0x20?-1:1 * dir[(r&0xc)>>2], r&0x40?-1:1 * dir[3-(r&0x3 + (r&0xc)>>2)])
		uint8 rotation;

		/// \brief Standard constructor creates undefined element
		Voxel() : material(Material::UNDEFINED), type(ComponentType::UNDEFINED), dirty(0), solid(0), surface(0), health(0), sysAssignment(0)	{}

		/// \brief Construct a component with a defined type and undefined material
		Voxel(ComponentType _type) :
			material( TypeInfo::GetMaterial(_type) ),
			type( _type ),
			dirty( 1 ),
			solid( TypeInfo::IsSolid(_type) ? 1 : 0 ),
			surface( 0 ),
			sysAssignment( 0 ),
			rotation( 0x4 ) // no rotation = 0->0, 1->1, no signs
		{
		}

		/// \brief Mark this component as outdated (it is set to undefined)
		void Touch()			{ dirty = 1; }
		bool IsDirty() const	{ return dirty == 1; }

		/// \brief Undefined material and type.
		static const Voxel UNDEFINED;

		/// \brief Checks if type is equal
		bool operator == (const Voxel& _mat) const		{ return type == _mat.type; }
		/// \brief Checks if type is not equal
		bool operator != (const Voxel& _mat) const		{ return type != _mat.type; }

		/// \brief Rotate +90 degree (ccw) or -90 degree (cw) around X axis.
		void RotateX(bool _ccw);
		/// \brief Rotate +90 degree (ccw) or -90 degree (cw) around Y axis.
		void RotateY(bool _ccw);
		/// \brief Rotate +90 degree (ccw) or -90 degree (cw) around Z axis.
		void RotateZ(bool _ccw);

		/// \brief Get the transformed main functional direction. For omni-
		///		directional components this returns Vec3(0)
		ei::Vec3 GetMainDir();
	};
#	pragma pack(pop)
};