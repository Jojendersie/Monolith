#include "voxel.hpp"
#include "material.hpp"
#include "utilities/logger.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/core/device.hpp"
#include "utilities/scopedpointer.hpp"
#include <jofilelib.hpp>
#include <hybridarray.hpp>

using namespace std;

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
			voxelInfo.isSolid = voxelNode[string("Solid")].Get(true);
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
			voxelInfo.energyOutput = voxelNode[string("EnergyOutput")].Get(0.0f);
			voxelInfo.energyDrain = voxelNode[string("EnergyOutput")].Get(0.0f);
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
	/*typedef int (*BorderIndexFunc)(const Math::IVec3& _position, int _e);
	static BorderIndexFunc GetBorderIndex[6] = {
		[](const Math::IVec3& _position, int _e) { return _position[1] + _e * (_position[2] + _e *       _position[0]); },
		[](const Math::IVec3& _position, int _e) { return _position[1] + _e * (_position[2] + _e * (_e - _position[0] - 1)); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[2] + _e *       _position[1]); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[2] + _e * (_e - _position[1] - 1)); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[1] + _e *       _position[2]); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[1] + _e * (_e - _position[2] - 1)); }
	};*/
	// A function to swap the surface bits in the same order as the access is done
	// THIS IS CURRENTLY WRONG!
	typedef uint8_t (*BorderSurfaceFunc)(uint8_t _surface);
	static BorderSurfaceFunc GetBorderSurface[6] = {
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s << 2) | (_s >> 4)); },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s << 2) | ((_s >> 3) & 2) | (_s >> 5) ); },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s & 3) | ((_s << 2) & 0x30) | ((_s >> 2 ) & 0x0c) ); },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s & 3) | ((_s << 2) & 0x30) | ((_s >> 1 ) & 0x08) | ((_s >> 3 ) & 0x04) ); },
		[](uint8_t _s) -> uint8_t { return _s; },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s & 0xf) | ((_s >> 1) & 0x10) | ((_s << 1) & 0x20)); },
	};

	typedef int (*BorderIndexFunc)(const Math::IVec3& _position, int _e);
	static BorderIndexFunc GetBorderIndex[6] = {
		[](const Math::IVec3& _position, int _e) { return _position[1] + _e * (_position[0]          + _e * _position[2]); }, // Left
		[](const Math::IVec3& _position, int _e) { return _position[1] + _e * (_e - _position[0] - 1 + _e * _position[2]); }, // Right
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[1]          + _e * _position[2]); }, // Bottom
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_e - _position[1] - 1 + _e * _position[2]); }, // Top
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[2]          + _e * _position[1]); }, // Front
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_e - _position[2] - 1 + _e * _position[1]); }  // Back
	};

	// ********************************************************************* //
	int TypeInfo::SamplePos( ComponentType _type, Math::IVec3& _position, int _level, int& _edge, int& _offset )
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
	bool TypeInfo::Sample( ComponentType _type, Math::IVec3 _position, int _level, uint8_t _rootSurface, Material& _materialOut, uint8_t& _surfaceOut )
	{
		int e, off;
		int index = SamplePos(_type, _position, _level, e, off);

		Assert(_position[0] >= 0 && _position[0] < e, "Out of bounds access in voxel texture!");
		Assert(_position[1] >= 0 && _position[1] < e, "Out of bounds access in voxel texture!");
		Assert(_position[2] >= 0 && _position[2] < e, "Out of bounds access in voxel texture!");

		MatSample borderSample;
		ComponentTypeInfo& currentVoxel = g_InfoManager->m_voxels[(int)_type];

		// For each neighbor do the border texture sampling
		// - take the first defined one.
		if( currentVoxel.borderTexture && _rootSurface )
		for( int i = 0; i < 6; ++i )
		{
			if( (_rootSurface & (1<<i)) == 0 )
			{
				MatSample& sample = currentVoxel.borderTexture[off + GetBorderIndex[i](_position, e)];
				if( sample.material != Material::UNDEFINED )
				{
					if( borderSample.material != Material::UNDEFINED )
						// Aggregate the non-surface flags from all samples. The
						// solidity of the final sample depends on all active sampled volumes.
						borderSample.surface &= GetBorderSurface[i](sample.surface);
					else {
						borderSample.material = sample.material;
						borderSample.surface = GetBorderSurface[i](sample.surface);
					}
				}
			}
		}


		// Take main texture if no border texture was found
		MatSample& sample = currentVoxel.texture[index];
		if( sample.material != Material::UNDEFINED )
		{
			// Again combine surface flags to draw as few as possible
			if( borderSample.material != Material::UNDEFINED )
				borderSample.surface &= sample.surface;
			else
				borderSample = sample;
		}

		// If the current voxel is at the border take rootSurface into account
		if( _position[0] == 0   && !(_rootSurface & 0x01) ) borderSample.surface &= 0x3e;	// Left
		if( _position[0] == e-1 && !(_rootSurface & 0x02) ) borderSample.surface &= 0x3d;	// Right
		if( _position[1] == 0   && !(_rootSurface & 0x04) ) borderSample.surface &= 0x3b;	// Bottom
		if( _position[1] == e-1 && !(_rootSurface & 0x08) ) borderSample.surface &= 0x37;	// Top
		if( _position[2] == 0   && !(_rootSurface & 0x10) ) borderSample.surface &= 0x2f;	// Front
		if( _position[2] == e-1 && !(_rootSurface & 0x20) ) borderSample.surface &= 0x1f;	// Back

		// Return the found / combined texture information
		_materialOut = borderSample.material;
		_surfaceOut = borderSample.surface;
		return borderSample.surface != 0;
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
	bool TypeInfo::IsSolid( ComponentType _type )
	{
		if( (int)_type >= g_InfoManager->m_numVoxels ) {
			LOG_LVL1("The searched voxel type is not defined.");
			return false;
		} else
			return g_InfoManager->m_voxels[(int)_type].isSolid;
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
		return false;
		//return GetEnergyOut(_type) > 0.0f && GetEnergyIn(_type) == 0.0f;
	}

	bool TypeInfo::IsBattery( ComponentType _type )
	{
		return false;
	}

	bool TypeInfo::IsDrive( ComponentType _type )
	{
		return false;
	}

	bool TypeInfo::IsWeapon( ComponentType _type )
	{
		return false;
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

		ScopedPtr<uint32_t> buffer(new unsigned int[maxRes * maxRes * maxRes * 2]);

		// Fill with data
		for( int i=0; i<m_numVoxels; ++i )
		{
			int res = maxRes;
			for( int l = 0; l < maxMipMapLevels; ++l )
			{
				Math::IVec3 pos(0);
				for( ; pos[2] < res; ++pos[2] )
					for( pos[1] = 0; pos[1] < res; ++pos[1] )
						for( pos[0] = 0; pos[0] < res; ++pos[0] ) {
							Math::IVec3 modPos = pos;
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

} // namespace Voxel