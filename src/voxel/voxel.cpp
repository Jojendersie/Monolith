#include "voxel.hpp"
#include "material.hpp"
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
		m_voxels(nullptr)
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

		// TODO: handle bad-file cases
		Jo::Files::MetaFileWrapper infoFile( Jo::Files::HDDFile( "voxel.json" ), Jo::Files::Format::JSON );
		g_InfoManager->m_numVoxels = (int)infoFile.RootNode[string("PerVoxelInfo")].Size();

		// TODO: logging and error message
		assert(g_InfoManager->m_numVoxels > 0 && g_InfoManager->m_numVoxels < 256);

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
			voxelInfo.water = voxelNode[string("Water")].Get(0);
			voxelInfo.minerals = voxelNode[string("Minerals")].Get(0);
			voxelInfo.metals = voxelNode[string("Metals")].Get(0);
			voxelInfo.rareMetals = voxelNode[string("Rare Metals")].Get(0);
			voxelInfo.alloys = voxelNode[string("Alloys")].Get(0);
			voxelInfo.polymers = voxelNode[string("Polymers")].Get(0);
			// Get volumetric size
			int s = voxelNode[string("Texture Resolution")].Get(0);
			voxelInfo.textureResolution = s;
			s = s * s * s;
			assert(voxelNode[string("Texture")].Size() == s);	// TODO: Logging
			assert(voxelNode[string("Border Texture")].Size() == s);
			// Copy textures while interpreting the lookup table
			voxelInfo.texture = new Material[(s * 8) / 7];
			voxelInfo.borderTexture = new Material[(s * 8) / 7];
			auto& colorNode = voxelNode[string("Colors")];
			auto& texNode = voxelNode[string("Texture")];
			auto& borderTexNode = voxelNode[string("Border Texture")];
			for( int v = 0; v < s; ++v )
			{
				// TODO: Logging
				assert((int)texNode[v] < colorNode.Size() && (int)texNode[v] >= 0);
				assert((int)borderTexNode[v] < colorNode.Size() && (int)borderTexNode[v] >= 0);
				voxelInfo.texture[v] = colorNode[(int)texNode[v]];
				voxelInfo.borderTexture[v] = colorNode[(int)borderTexNode[v]];
			}

			voxelInfo.numMipMaps = GenerateMipMap(voxelInfo.texture, voxelInfo.textureResolution);
			GenerateMipMap(voxelInfo.borderTexture, voxelInfo.textureResolution);
		}
	}

	// ********************************************************************* //
	void TypeInfo::Unload()
	{
		delete[] g_InfoManager->m_voxels;
		g_InfoManager->m_voxels = nullptr;
		g_InfoManager->m_numVoxels = 0;
	}


	// ********************************************************************* //
	Material TypeInfo::Sample( VoxelType _type, const Math::IVec3& _position, int _level )
	{
		assert(GetMaxLevel(_type) >= _level);

		// Compute level access values
		int e = 1 << (GetMaxLevel(_type) - _level);
		int off = (e*e*e * 8) / 7 - 1;

		assert(_position[0] >= 0 && _position[0] < e);
		assert(_position[1] >= 0 && _position[1] < e);
		assert(_position[2] >= 0 && _position[2] < e);

		return g_InfoManager->m_voxels[(int)_type].texture[off + _position[0] + e * (_position[1] + e * _position[2])];
	}

	// ********************************************************************* //
	int TypeInfo::GetMaxLevel( VoxelType _type )
	{
		// TODO: Log
		assert((int)_type < g_InfoManager->m_numVoxels);
		return g_InfoManager->m_voxels[(int)_type].numMipMaps;
	}

	// ********************************************************************* //
	bool TypeInfo::IsSolid( VoxelType _type )
	{
		// TODO: Log
		assert((int)_type < g_InfoManager->m_numVoxels);
		return g_InfoManager->m_voxels[(int)_type].isSolid;
	}

	// ********************************************************************* //
	float TypeInfo::GetMass( VoxelType _type )
	{
		// TODO: Log
		assert((int)_type < g_InfoManager->m_numVoxels);
		return g_InfoManager->m_voxels[(int)_type].mass;
	}


	// ********************************************************************* //
	int TypeInfo::GenerateMipMap( Material* _texture, int _edge )
	{
		int numLevels = 0;
		int e = _edge;		// current level edge length
		int off = e*e*e;	// Offset of current level
		int prevoff = 0;	// Offset of previous level
		while( (e /= 2) > 0 )
		{
			++numLevels;
			// Loop over target level
			for(int z = 0; z < e; ++z ) {
				for(int y = 0; y < e; ++y ) {
					for(int x = 0; x < e; ++x )
					{
						int index = x + e * (y + e * z);
						// Collect defined nodes from finer levels
						Jo::HybridArray<Material> buffer;
						int parentIndex;
						parentIndex = 2 * x +     2 * e * (2 * y +     4 * e * z);
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x + 1 + 2 * e * (2 * y +     4 * e * z);
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x +     2 * e * (2 * y + 1 + 4 * e * z);
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x + 1 + 2 * e * (2 * y + 1 + 4 * e * z);
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x +     2 * e * (2 * y +     2 * e * (2 * z + 1));
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x + 1 + 2 * e * (2 * y +     2 * e * (2 * z + 1));
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x +     2 * e * (2 * y + 1 + 2 * e * (2 * z + 1));
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						parentIndex = 2 * x + 1 + 2 * e * (2 * y + 1 + 2 * e * (2 * z + 1));
						if( _texture[prevoff + parentIndex] != Material::UNDEFINED ) buffer.PushBack( _texture[prevoff + parentIndex] );
						_texture[off + index] = Material(&buffer.First(), buffer.Size());
					}
				}
			} // for target level
			prevoff = off;
			off += e*e*e;
		}
		return numLevels;
	}

} // namespace Voxel