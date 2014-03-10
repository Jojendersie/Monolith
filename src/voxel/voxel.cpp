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
			Jo::Files::MetaFileWrapper::Node* borderTexNode = nullptr;
			voxelNode.HasChild( string("Border Texture"), &borderTexNode );
			s = s * s * s;
			assert(voxelNode[string("Texture")].Size() == s);	// TODO: Logging
			// Copy textures while interpreting the lookup table
			voxelInfo.texture = new MatSample[(s * 8) / 7];
			if( borderTexNode )
			{
				assert(borderTexNode->Size() == s);
				voxelInfo.borderTexture = new MatSample[(s * 8) / 7];
			}
			auto& colorNode = voxelNode[string("Colors")];
			auto& texNode = voxelNode[string("Texture")];
			for( int v = 0; v < s; ++v )
			{
				// TODO: Logging
				assert((int)texNode[v] < colorNode.Size() && (int)texNode[v] >= 0);
				voxelInfo.texture[v].material = colorNode[(int)texNode[v]];
				if( borderTexNode )
				{
					assert((int)(*borderTexNode)[v] < colorNode.Size() && (int)(*borderTexNode)[v] >= 0);
					voxelInfo.borderTexture[v].material = colorNode[(int)(*borderTexNode)[v]];
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
	}

	// ********************************************************************* //
	void TypeInfo::Unload()
	{
		delete[] g_InfoManager->m_voxels;
		g_InfoManager->m_voxels = nullptr;
		g_InfoManager->m_numVoxels = 0;
	}


	// ********************************************************************* //
	// A set of functions to sample rotated/mirrored in the border field
	typedef int (*BorderIndexFunc)(const Math::IVec3& _position, int _e);
	static BorderIndexFunc GetBorderIndex[6] = {
		[](const Math::IVec3& _position, int _e) { return _position[1] + _e * (_position[2] + _e * _position[0]); },
		[](const Math::IVec3& _position, int _e) { return _position[1] + _e * (_position[2] + _e * (_e - _position[0] - 1)); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[2] + _e * _position[1]); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[2] + _e * (_e - _position[1] - 1)); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[1] + _e * _position[2]); },
		[](const Math::IVec3& _position, int _e) { return _position[0] + _e * (_position[1] + _e * (_e - _position[2] - 1)); }
	};
	// A function to swap the surface bits in the same order as the access is done
	typedef uint8_t (*BorderSurfaceFunc)(uint8_t _surface);
	static BorderSurfaceFunc GetBorderSurface[6] = {
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s << 2) | (_s >> 4)); },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s << 2) | ((_s >> 3) & 2) | (_s >> 5) ); },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s & 3) | ((_s << 2) & 0x30) | ((_s >> 2 ) & 0x0c) ); },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s & 3) | ((_s << 2) & 0x30) | ((_s >> 1 ) & 0x08) | ((_s >> 3 ) & 0x08) ); },
		[](uint8_t _s) -> uint8_t { return _s; },
		[](uint8_t _s) -> uint8_t { return 0x3f & ((_s & 0xf) | ((_s >> 1) & 0x10) | ((_s << 1) & 0x20)); },
	};

	// ********************************************************************* //
	bool TypeInfo::Sample( VoxelType _type, Math::IVec3 _position, int _level, uint8_t _rootSurface, Material& _materialOut, uint8_t& _surfaceOut )
	{
		// Compute level access values
		int maxLevel = GetMaxLevel(_type);
		int e, off;
		// 0   1   2   3 <- level
		// 585 73  9   1 <- l ^ 3 * 8 / 7
		// 584 576 512 0 <- offsets
		if( _level >= maxLevel ) {
			e = (1 << maxLevel);
			off = 0;
			_position /= 1 << (_level-maxLevel);
		} else {
			e = (1 << _level);
			int m = 1 << maxLevel, n = 1 << _level;
			off = (m*m*m * 8) / 7 - (n*n*n * 8) / 7;
		}

		assert(_position[0] >= 0 && _position[0] < e);
		assert(_position[1] >= 0 && _position[1] < e);
		assert(_position[2] >= 0 && _position[2] < e);

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
		MatSample& sample = currentVoxel.texture[off + _position[0] + e * (_position[1] + e * _position[2])];
		if( sample.material != Material::UNDEFINED )
		{
			// Again combine surface flags to draw as few as possible
			if( borderSample.material != Material::UNDEFINED )
				borderSample.surface &= sample.surface;
			else
				borderSample = sample;
		}

		// Return the found / combined texture information
		_materialOut = borderSample.material;
		_surfaceOut = borderSample.surface;
		return borderSample.surface != 0;
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
						if( buffer.Size() > 0 )
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

} // namespace Voxel