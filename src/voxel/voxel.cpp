#include "voxel.hpp"
#include "material.hpp"
#include "utilities/logger.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/core/device.hpp"
#include "utilities/scopedpointer.hpp"
#include <jofilelib.hpp>
#include <hybridarray.hpp>

using namespace std;
using namespace ei;

namespace Voxel {
	static TypeInfo* g_InfoManager;

	// ********************************************************************* //
	TypeInfo::ComponentTypeInfo::ComponentTypeInfo() :
		texture(nullptr),
		borderTexture(nullptr),
		numMipMaps(0)
	{
	}

	TypeInfo::ComponentTypeInfo::~ComponentTypeInfo()
	{
		delete[] texture;
		delete[] borderTexture;
	}

	// ********************************************************************* //
	void TypeInfo::Initialize()
	{
		static TypeInfo infoManager;
		g_InfoManager = &infoManager;
		Load();
	}

	// ********************************************************************* //
	TypeInfo::TypeInfo() :
		m_numVoxels(0),
		m_voxels(nullptr),
		m_voxelTextures(nullptr)
	{
	}

	TypeInfo::~TypeInfo()
	{
		Unload();
	}

	// ********************************************************************* //
	void TypeInfo::Load()
	{
		Unload();

		try {
		Jo::Files::MetaFileWrapper infoFile( Jo::Files::HDDFile( "voxel.json" ), Jo::Files::Format::JSON );
		
		g_InfoManager->m_numVoxels = (int)infoFile.RootNode[string("PerVoxelInfo")].Size();

		if(g_InfoManager->m_numVoxels < 0 )
			LOG_CRITICAL("The file 'voxel.json' is damaged.");
		if(g_InfoManager->m_numVoxels >= 256)
			LOG_LVL2("The file 'voxel.json' contains more definitions than the game supports.");

		// The file contains an array with voxel information
		g_InfoManager->m_voxels = new ComponentTypeInfo[g_InfoManager->m_numVoxels];
		for( int i = 0; i < g_InfoManager->m_numVoxels; ++i )
		{
			auto& voxelNode = infoFile.RootNode[string("PerVoxelInfo")][i];
			ComponentTypeInfo& voxelInfo = g_InfoManager->m_voxels[i];

			// TODO: string pooling to avoid construction and destruction of std strings
			voxelInfo.name = voxelNode[string("Name")];
			voxelInfo.isInner = voxelNode[string("Inner")].Get(true);
			voxelInfo.mass = voxelNode[string("Mass")].Get(1.0f);
			voxelInfo.thresholdEnergy = voxelNode[string("Threshold Energy")].Get(100000.0f);
			voxelInfo.reactionEnergy = voxelNode[string("Reaction Energy")].Get(0.0f);
			voxelInfo.hydrogen = voxelNode[string("Hydrogen")].Get(0);
			voxelInfo.carbon = voxelNode[string("Carbon")].Get(0);
			voxelInfo.metals = voxelNode[string("Metals")].Get(0);
			voxelInfo.rareEarthElements = voxelNode[string("Rare Earth Elements")].Get(0);
			voxelInfo.semiconductors = voxelNode[string("Semiconductors")].Get(0);
			voxelInfo.heisenbergium = voxelNode[string("Heisenbergium")].Get(0);
			// Optional values
			voxelInfo.energyOutput = voxelNode[string("Energy Output")].Get(0.0f);
			voxelInfo.energyDrain = voxelNode[string("Energy Drain")].Get(0.0f);
			voxelInfo.storageVolume = voxelNode[string("StorageVolume")].Get(0.0f);
			voxelInfo.capacity = voxelNode[string("Capacity")].Get(0.0f);
			voxelInfo.damage = voxelNode[string("Damage")].Get(0.0f);
			voxelInfo.cooldown = voxelNode[string("Cooldown")].Get(0.0f);
			voxelInfo.range = voxelNode[string("Range")].Get(0.0f);
			voxelInfo.projectileSpeed = voxelNode[string("ProjectileSpeed")].Get(0.0f);
			voxelInfo.thrust = voxelNode[string("Thrust")].Get(0.0f);
			voxelInfo.shieldRegeneration = voxelNode[string("ShieldRegeneration")].Get(0.0f);
			voxelInfo.shieldComponentType = voxelNode[string("ShieldComponentType")].Get(0);
			voxelInfo.lifeSupport = voxelNode[string("LifeSupport")].Get(0.0f);
			Jo::Files::MetaFileWrapper::Node* mainDirNode;
			if(voxelNode.HasChild(string("Main Direction"), &mainDirNode))
			{
				string dirCode = *mainDirNode;
				voxelInfo.mainDir = (dirCode[0] == '-' ? -1 : 1) *
					Vec3(dirCode[1] == 'x' ? 1.0f : 0.0f, dirCode[1] == 'y' ? 1.0f : 0.0f, dirCode[1] == 'z' ? 1.0f : 0.0f);
			} else voxelInfo.mainDir = Vec3(0.0);
			// Get volumetric size
			int s = voxelNode[string("Texture Resolution")].Get(0);
			voxelInfo.textureResolution = s;
			Jo::Files::MetaFileWrapper::Node* borderTexNode = nullptr;
			voxelNode.HasChild( string("Border Texture"), &borderTexNode );
			s = s * s * s;
			if( voxelNode[string("Texture")].Size() != s )
				LOG_ERROR("The size of the Texture from " + voxelInfo.name + " is " + std::to_string(voxelNode[string("Texture")].Size()) + " but should be " + to_string(s));
			// Copy textures while interpreting the lookup table
			voxelInfo.texture = new MatSample[(s * 8) / 7];
			if( borderTexNode )
			{
				if( voxelNode[string("Border Texture")].Size() != s )
					LOG_ERROR("The size of the Border Texture from " + voxelInfo.name + " is " + std::to_string(voxelNode[string("Border Texture")].Size()) + " but should be " + to_string(s));
				voxelInfo.borderTexture = new MatSample[(s * 8) / 7];
			}
			auto& colorNode = voxelNode[string("Colors")];
			auto& texNode = voxelNode[string("Texture")];
			for( int v = 0; v < s; ++v )
			{
				// Read the textures pixel for pixel and do material lookups in
				// the shared color table.
				int index = (int)texNode[v];
				if( index >= colorNode.Size() || index < 0 )
					LOG_ERROR("The Texture of " + voxelInfo.name + " uses an invalid color index (" + std::to_string(index) + ").");
				voxelInfo.texture[v].material = colorNode[index];
				// Read border texture only if defined
				if( borderTexNode )
				{
					index = (int)(*borderTexNode)[v];
					if( index >= colorNode.Size() || index < 0 )
						LOG_ERROR("The Border Texture of " + voxelInfo.name + " uses an invalid color index (" + std::to_string(index) + ").");
					voxelInfo.borderTexture[v].material = colorNode[index];
				}
			}

			voxelInfo.numMipMaps = GenerateMipMap(voxelInfo.texture, voxelInfo.textureResolution);
			GenerateSurfaceInfo(voxelInfo.texture, voxelInfo.textureResolution, true);
			if( borderTexNode )
			{
				GenerateMipMap(voxelInfo.borderTexture, voxelInfo.textureResolution);
				GenerateSurfaceInfo(voxelInfo.borderTexture, voxelInfo.textureResolution, true);
			}
		}

		} catch( std::string _message ) {
			// In case of a bad file reinstall ;-)
			LOG_CRITICAL( _message );
		} catch(...) {
			LOG_CRITICAL( "Unknown error during loading the voxel definition file." );
		}

		g_InfoManager->GenerateTexture();
	}

	// ********************************************************************* //
	void TypeInfo::Unload()
	{
		delete[] g_InfoManager->m_voxels;
		g_InfoManager->m_voxels = nullptr;
		g_InfoManager->m_numVoxels = 0;
		delete g_InfoManager->m_voxelTextures;
		g_InfoManager->m_voxelTextures = nullptr;
	}


	// ********************************************************************* //
	// A set of functions to sample rotated/mirrored in the border field
	typedef int (*BorderIndexFunc)(const ei::IVec3& _position, int _e);
	static BorderIndexFunc GetBorderIndex[6] = {
		[](const ei::IVec3& _position, int _e) { return _position[1] + _e * (_position[0]          + _e * _position[2]); }, // Left
		[](const ei::IVec3& _position, int _e) { return _position[1] + _e * (_e - _position[0] - 1 + _e * _position[2]); }, // Right
		[](const ei::IVec3& _position, int _e) { return _position[0] + _e * (_position[1]          + _e * _position[2]); }, // Bottom
		[](const ei::IVec3& _position, int _e) { return _position[0] + _e * (_e - _position[1] - 1 + _e * _position[2]); }, // Top
		[](const ei::IVec3& _position, int _e) { return _position[0] + _e * (_position[2]          + _e * _position[1]); }, // Front
		[](const ei::IVec3& _position, int _e) { return _position[0] + _e * (_e - _position[2] - 1 + _e * _position[1]); }  // Back
	};

	// ********************************************************************* //
	int TypeInfo::SamplePos( ComponentType _type, ei::IVec3& _position, int _level, int& _edge, int& _offset )
	{
		// Compute level access values
		int maxLevel = GetMaxLevel(_type);
		// 0   1   2   3 <- level
		// 585 73  9   1 <- l ^ 3 * 8 / 7
		// 584 576 512 0 <- offsets
		if( _level >= maxLevel ) {
			_edge = (1 << maxLevel);
			_offset = 0;
			_position /= 1 << (_level-maxLevel);
		} else {
			_edge = (1 << _level);
			int m = 1 << maxLevel, n = 1 << _level;
			_offset = (m*m*m * 8) / 7 - (n*n*n * 8) / 7;
		}

		return _offset + _position[0] + _edge * (_position[1] + _edge * _position[2]);
	}

	// ********************************************************************* //
	Material TypeInfo::GetMaterial( ComponentType _type )
	{
		int e, off;
		int index = SamplePos(_type, ei::IVec3(0), 0, e, off);
		ComponentTypeInfo& currentVoxel = g_InfoManager->m_voxels[(int)_type];

		return currentVoxel.texture[index].material;
	}

	// ********************************************************************* //
	int TypeInfo::GetMaxLevel( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].numMipMaps;
	}

	// ********************************************************************* //
	bool TypeInfo::IsInner( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return false;
		} else
			return g_InfoManager->m_voxels[(int)_type].isInner;
	}

	// ********************************************************************* //
	float TypeInfo::GetMass( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0.0f;
		} else
			return g_InfoManager->m_voxels[(int)_type].mass;
	}

	// ********************************************************************* //
	float TypeInfo::GetThresholdEnergy( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0.0f;
		} else
			return g_InfoManager->m_voxels[(int)_type].thresholdEnergy;
	}

	// ********************************************************************* //
	float TypeInfo::GetReactionEnergy( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0.0f;
		} else
			return g_InfoManager->m_voxels[(int)_type].reactionEnergy;
	}

	// ********************************************************************* //
	int TypeInfo::GetHydrogen( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].hydrogen;
	}

	// ********************************************************************* //
	int TypeInfo::GetCarbon( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].carbon;
	}

	// ********************************************************************* //
	int TypeInfo::GetMetals( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].metals;
	}

	// ********************************************************************* //
	int TypeInfo::GetRareEarthElements( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].rareEarthElements;
	}

	// ********************************************************************* //
	int TypeInfo::GetSemiconductors( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].semiconductors;
	}

	// ********************************************************************* //
	int TypeInfo::GetHeisenbergium( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].heisenbergium;
	}

	// ********************************************************************* //
	float TypeInfo::GetEnergyOut( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].energyOutput;
	}

	// ********************************************************************* //
	float TypeInfo::GetEnergyDrain( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].energyDrain;
	}

	// ********************************************************************* //
	float TypeInfo::GetCapacity( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].capacity;
	}

	// ********************************************************************* //
	float TypeInfo::GetStorageVolume(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].storageVolume;
	}

	// ********************************************************************* //
	float TypeInfo::GetThrust(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].thrust;
	}

	// ********************************************************************* //
	float TypeInfo::GetDamage(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].damage;
	}

	// ********************************************************************* //
	float TypeInfo::GetRange(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].range;
	}

	// ********************************************************************* //
	float TypeInfo::GetProjectileSpeed(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].projectileSpeed;
	}

	// ********************************************************************* //
	float TypeInfo::GetShieldRegeneration(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].shieldRegeneration;
	}

	// ********************************************************************* //
	float TypeInfo::GetShieldComponentType(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].shieldComponentType;
	}

	// ********************************************************************* //
	float TypeInfo::GetLiveSupport(ComponentType _type)
	{
		if ((int)_type >= g_InfoManager->m_numVoxels) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		}
		else
			return g_InfoManager->m_voxels[(int)_type].lifeSupport;
	}

	// ********************************************************************* //
	std::string TypeInfo::GetName( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return 0;
		} else
			return g_InfoManager->m_voxels[(int)_type].name;
	}

	// ********************************************************************* //
	const ei::Vec3& TypeInfo::GetMainDir( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return g_InfoManager->m_voxels[0].mainDir;
		} else
			return g_InfoManager->m_voxels[(int)_type].mainDir;
	}

	// ********************************************************************* //
	int TypeInfo::GetNumVoxels()
	{
		return g_InfoManager->m_numVoxels;
	}

	// ********************************************************************* //
	int TypeInfo::GenerateMipMap( MatSample* _texture, int _e )
	{
		int numLevels = 0;
		int off = _e*_e*_e;	// Offset of current level
		int prevoff = 0;	// Offset of previous level
		while( (_e /= 2) > 0 )
		{
			++numLevels;
			// Loop over target level
			for(int z = 0; z < _e; ++z ) {
				for(int y = 0; y < _e; ++y ) {
					for(int x = 0; x < _e; ++x )
					{
						int index = x + _e * (y + _e * z);
						// Collect defined nodes from finer levels
						Jo::HybridArray<Material> buffer;
						int parentIndex;
						parentIndex = 2 * x +     2 * _e * (2 * y +     4 * _e * z);
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x + 1 + 2 * _e * (2 * y +     4 * _e * z);
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x +     2 * _e * (2 * y + 1 + 4 * _e * z);
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x + 1 + 2 * _e * (2 * y + 1 + 4 * _e * z);
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x +     2 * _e * (2 * y +     2 * _e * (2 * z + 1));
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x + 1 + 2 * _e * (2 * y +     2 * _e * (2 * z + 1));
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x +     2 * _e * (2 * y + 1 + 2 * _e * (2 * z + 1));
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						parentIndex = 2 * x + 1 + 2 * _e * (2 * y + 1 + 2 * _e * (2 * z + 1));
						if( _texture[prevoff + parentIndex].material != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex].material );
						if( buffer.Size() > 3 )
							_texture[off + index].material = Material(&buffer.First(), buffer.Size());
						else _texture[off + index].material = Material::UNDEFINED;
					}
				}
			} // for target level
			prevoff = off;
			off += _e*_e*_e;
		}
		return numLevels;
	}

	void TypeInfo::GenerateSurfaceInfo( MatSample* _texture, int _e, bool _default )
	{
		int off = 0;	// Offset of current level
		while( _e > 0 )
		{
			// Loop over target level
			for(int z = 0; z < _e; ++z ) {
				for(int y = 0; y < _e; ++y ) {
					for(int x = 0; x < _e; ++x )
					{
						int index = x + _e * (y + _e * z);
						if( _texture[off + index].material == Material::UNDEFINED )
							_texture[off + index].surface = 0;
						else
						{
							bool left   = x == 0      ? _default : _texture[off + x - 1 + _e * (y + _e * z)].material == Material::UNDEFINED;
							bool right  = x == (_e-1) ? _default : _texture[off + x + 1 + _e * (y + _e * z)].material == Material::UNDEFINED;
							bool bottom = y == 0      ? _default : _texture[off + x + _e * (y - 1 + _e * z)].material == Material::UNDEFINED;
							bool top    = y == (_e-1) ? _default : _texture[off + x + _e * (y + 1 + _e * z)].material == Material::UNDEFINED;
							bool front  = z == 0      ? _default : _texture[off + x + _e * (y + _e * (z - 1))].material == Material::UNDEFINED;
							bool back   = z == (_e-1) ? _default : _texture[off + x + _e * (y + _e * (z + 1))].material == Material::UNDEFINED;
							_texture[off + index].surface =
								  (left   ? 0x01 : 0)
								| (right  ? 0x02 : 0)
								| (bottom ? 0x04 : 0)
								| (top    ? 0x08 : 0)
								| (front  ? 0x10 : 0)
								| (back   ? 0x20 : 0);
						}
					}
				}
			} // for target level
			off += _e*_e*_e;
			_e /= 2;
		}
	}

	// ********************************************************************* //
	void TypeInfo::BindVoxelTextureArray()
	{
		Graphic::Device::SetTexture( *g_InfoManager->m_voxelTextures, 0 );
	}

	// ********************************************************************* //
	bool TypeInfo::IsComputer( ComponentType _type )
	{
		return _type == ComponentType::COMPUTER;
	}

	bool TypeInfo::IsStorage( ComponentType _type )
	{
		return GetStorageVolume(_type) > 0.0f;
	}

	bool TypeInfo::IsReactor( ComponentType _type )
	{
		return GetEnergyOut(_type) > 0.0f && GetEnergyDrain(_type) == 0.0f;
	}

	bool TypeInfo::IsBattery( ComponentType _type )
	{
		return GetCapacity(_type) > 0.f;
	}

	bool TypeInfo::IsDrive( ComponentType _type )
	{
		return GetThrust(_type) > 0.0f;
	}

	bool TypeInfo::IsWeapon( ComponentType _type )
	{
		return GetDamage(_type) > 0.0f;
	}

	bool TypeInfo::IsShield( ComponentType _type )
	{
		return false;
	}

	bool TypeInfo::IsSensor( ComponentType _type )
	{
		return false;
	}

	// ********************************************************************* //
	void TypeInfo::GenerateTexture()
	{
		// Get maximum resolution (all textures in an array must have the same
		// size)
		int maxRes = 0, maxMipMapLevels = 0;
		for( int i=0; i<m_numVoxels; ++i )
			if( maxRes < m_voxels[i].textureResolution ) {
				maxRes = m_voxels[i].textureResolution;
				maxMipMapLevels = m_voxels[i].numMipMaps;
			}
		// Reserve texture memory
		Graphic::Texture::Format format(2, 32, Graphic::Texture::Format::ChannelType::UINT);
		m_voxelTextures = new Graphic::Texture(maxRes, maxRes, maxRes, m_numVoxels, format, maxMipMapLevels);

		ScopedPtr<uint32> buffer(new unsigned int[maxRes * maxRes * maxRes * 2]);

		// Fill with data
		for( int i=0; i<m_numVoxels; ++i )
		{
			int res = maxRes;
			for( int l = 0; l < maxMipMapLevels; ++l )
			{
				ei::IVec3 pos(0);
				for( ; pos[2] < res; ++pos[2] )
					for( pos[1] = 0; pos[1] < res; ++pos[1] )
						for( pos[0] = 0; pos[0] < res; ++pos[0] ) {
							ei::IVec3 modPos = pos;
							int e, off;
							int index = SamplePos(ComponentType(i), modPos, maxMipMapLevels - l, e, off);
							// Generate a single material code and a mask for which neighborhood
							// this sample is visible.
							// A bit in borderSample.surface is set if the neighbor creates
							// a material. The 7th bit is set if there is a material in every case.
							MatSample matInfo;
							matInfo.surface = 0;
							matInfo.material = Material::UNDEFINED;
							ComponentTypeInfo& currentVoxel = m_voxels[i];

							// For each neighbor do the border texture sampling
							if( currentVoxel.borderTexture )
							for( int b = 0; b < 6; ++b )
							{
								MatSample& sample = currentVoxel.borderTexture[off + GetBorderIndex[b](modPos, e)];
								if( sample.material != Material::UNDEFINED )
								{
									// Overwrite the material (-> take last)
									matInfo.material = sample.material;
									matInfo.surface |= 1<<b;
								}
							}

							// Take main texture if no border texture was found
							MatSample& sample = currentVoxel.texture[index];
							if( sample.material != Material::UNDEFINED )
							{
								matInfo.material = sample.material;
								matInfo.surface |= 0x40;
							}

							int bufferIndex = (pos[0] + res * (pos[1] + res * pos[2])) * 2;
							buffer[bufferIndex] = matInfo.material.code;
							buffer[bufferIndex + 1] = matInfo.surface; // TODO: Here are a lot of unused bits - can be used for empty space skipping
						}
				m_voxelTextures->UploadData(i, l, buffer);
				res /= 2;
			}
		}
	}


	// ********************************************************************* //
	static uint8 rotate(uint8 _code, const IMat3x3& _rot)
	{
		// Reconstruct matrix from bit code
		int x = _code & 0x3;
		int y = (_code & 0xc) >> 2;
		int z = 3 - (x + y);
		IMat3x3 current(0);
		current[x  ] = _code & 0x10 ? -1 : 1;
		current[y+3] = _code & 0x20 ? -1 : 1;
		current[z+6] = _code & 0x40 ? -1 : 1;
		// Rotate
		current *= _rot;
		// Find position of x and y in the columns
		x = current[0] != 0 ? 0 : (current[1] != 0 ? 1 : 2);
		y = current[3] != 0 ? 0 : (current[4] != 0 ? 1 : 2);
		z = 3 - (x + y);
		return x | (y << 2) | (current[x] > 0 ? 0 : 0x10) | (current[y+3] > 0 ? 0 : 0x20) | (current[z+6] > 0 ? 0 : 0x40);
	}
	void Voxel::RotateX(bool _ccw)
	{
		rotation = rotate(rotation, IMat3x3(1, 0, 0, 0, 0, _ccw?-1:1, 0, _ccw?1:-1,  0));
	}
	void Voxel::RotateY(bool _ccw)
	{
		rotation = rotate(rotation, IMat3x3(0, 0, _ccw?1:-1, 0, 1, 0, _ccw?-1:1, 0, 0));
	}
	void Voxel::RotateZ(bool _ccw)
	{
		rotation = rotate(rotation, IMat3x3(0, _ccw?1:-1, 0, _ccw?-1:1, 0, 0, 0, 0, 1));
	}

	// ********************************************************************* //
	ei::Vec3 Voxel::GetMainDir()
	{
		int x = rotation & 0x3;
		int y = (rotation & 0xc) >> 2;
		int z = 3 - (x + y);
		const Vec3& dir = TypeInfo::GetMainDir( type );
		return Vec3(rotation & 0x10 ? -dir[x] : dir[x], rotation & 0x20 ? -dir[y] : dir[y], rotation & 0x40 ? -dir[z] : dir[z]);
	}

} // namespace Voxel