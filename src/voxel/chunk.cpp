#include "chunk.hpp"
#include "model.hpp"
#include "../graphic/device.hpp"
#include "../graphic/uniformbuffer.hpp"
#include "../input/camera.hpp"
#include <cstdlib>
#include <cstring>

using namespace Math;

namespace Voxel {

	/// \brief The level offsets in number of voxels to the start of the octree
	//const int LEVEL_OFFSETS[] = {0,1,9,73,585,4681};
	const int NUM_OCTREE_NODES = 37449;

#	define INDEX(P, L)		(LEVEL_OFFSETS[L] + (P.x) + (1<<(L))*((P.y) + (1<<(L))*(P.z)))
#	define INDEX2(X,Y,Z, L)	(LEVEL_OFFSETS[L] + (X) + (1<<(L))*((Y) + (1<<(L))*(Z)))


	Chunk::Chunk(const Model* _model, const Math::IVec4& _nodePostion, int _depth) :
		m_model( _model ),
		m_scale( float(1<<(_nodePostion[3]-_depth)) ),
		m_depth( _depth ),
		m_root( _nodePostion ),
		m_voxels( "u", nullptr, 0, Graphic::VertexBuffer::PrimitiveType::POINT ),
		m_position( float(_nodePostion[0]<<_depth), float(_nodePostion[1]<<_depth), float(_nodePostion[2]<<_depth) )
	{
	}

	Chunk::Chunk( Chunk&& _chunk ) :
		m_model( _chunk.m_model ),
		m_scale( _chunk.m_scale ),
		m_depth( _chunk.m_depth ),
		m_root( _chunk.m_root ),
		m_voxels( std::move(_chunk.m_voxels) ),
		m_position( _chunk.m_position )
	{
	}


	Chunk::~Chunk()
	{
	}


	void Chunk::Draw( Graphic::UniformBuffer& _objectConstants,
			const Math::Mat4x4& _modelViewProjection )
	{
		// Translation to center the chunks
		_objectConstants["WorldViewProjection"] = Mat4x4::Translation(m_position) * Mat4x4::Scaling(m_scale) * _modelViewProjection;

		float halfScale = m_scale * 0.5f;
		_objectConstants["Corner000"] = Vec4( -halfScale, -halfScale, -halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner001"] = Vec4( -halfScale, -halfScale,  halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner010"] = Vec4( -halfScale,  halfScale, -halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner011"] = Vec4( -halfScale,  halfScale,  halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner100"] = Vec4(  halfScale, -halfScale, -halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner101"] = Vec4(  halfScale, -halfScale,  halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner110"] = Vec4(  halfScale,  halfScale, -halfScale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner111"] = Vec4(  halfScale,  halfScale,  halfScale, 0.0f ) * _modelViewProjection;

		Graphic::Device::DrawVertices( m_voxels, 0, m_voxels.GetNumVertices() );
	}



	// ********************************************************************* //
	struct CSParam {
		ChunkBuilder::PerVoxelInfo* buffer;	///< Array where the selected sector should be stored.
		IVec3 pmin;		///< Minimal boundary
		int level;		///< The level in the octree which should be copied
		int edgeLength;	///< Size of buffer in any direction (is Maximal boundary - Minimal boundary)
	};
	static bool CopySector(const Math::IVec4& _position, VoxelType _type, bool _hasChildren, CSParam* _param)
	{
		if( _position[3] < _param->level )
		{
			assert(_position[3]+1 == _param->level);
			// Child level sets solidity of parent
			if( !IsSolid(_type) )
			{
				// Parent is not solid too -> set
				int x = (_position[0] >> 1) - _param->pmin[0];
				int y = (_position[1] >> 1) - _param->pmin[1];
				int z = (_position[2] >> 1) - _param->pmin[2];
				_param->buffer[x + _param->edgeLength * (y + _param->edgeLength * z)].solid = false;
			}
			return false;	// No further traversal
		} else
		{
			int lvlDiff = _position[3] - _param->level;
			// Position inside target level?
			int span = (1 << lvlDiff) - 1;
			int x = (_position[0] << lvlDiff) - _param->pmin[0];
			int y = (_position[1] << lvlDiff) - _param->pmin[1];
			int z = (_position[2] << lvlDiff) - _param->pmin[2];
			// Inside sector [pmin, pmax]?
			if( (x >= _param->edgeLength) || (x+span < 0) ) return false;
			if( (y >= _param->edgeLength) || (y+span < 0) ) return false;
			if( (z >= _param->edgeLength) || (z+span < 0) ) return false;

			// Is this one of the searched voxels?
			if( lvlDiff == 0 )
			{
				ChunkBuilder::PerVoxelInfo& target = _param->buffer[x + _param->edgeLength * (y + _param->edgeLength * z)];
				target.type = _type;
				target.solid = IsSolid(_type);
				return target.solid;
			}

			// If there are no children traversal would stop - set entire area
			if( !_hasChildren && IsSolid(_type) )
			{
				int zmin = max(0,z); int zmax = min( z+span+1, _param->edgeLength );
				int ymin = max(0,y); int ymax = min( y+span+1, _param->edgeLength );
				int xmin = max(0,x); int xmax = min( x+span+1, _param->edgeLength );
				for( z=zmin; z<zmax; ++z )
					for( y=ymin; y<ymax; ++y )
						for( x=xmin; x<xmax; ++x )
						{
							ChunkBuilder::PerVoxelInfo& target = _param->buffer[x + _param->edgeLength * (y + _param->edgeLength * z)];
							target.type = _type;
							target.solid = true;
						}
			}

			return true;
		}
	}

	// ********************************************************************* //
	void ChunkBuilder::RecomputeVertexBuffer( Chunk& _chunk )
	{
		// Sample a (2^d+2)^3 volume (neighborhood for visibility). This
		// initial sampling avoids the expensive resampling for many voxels.
		// (Inner voxels would be sampled 7 times!)
		
		// Old Method with O(n log(m)) runtime
		/*int edgeLength = (1 << _chunk.m_depth) + 2;
		int level = Math::max( 0, _chunk.m_root[3] - _chunk.m_depth );
		IVec3 pmin = (IVec3(_chunk.m_root) << (_chunk.m_root[3] - level)) - 1;//IVec3(1);
		IVec3 pmax = pmin + IVec3(edgeLength);
		IVec3 pos;
		for( pos[2]=pmin[2]; pos[2]<pmax[2]; ++pos[2] )
			for( pos[1]=pmin[1]; pos[1]<pmax[1]; ++pos[1] )
				for( pos[0]=pmin[0]; pos[0]<pmax[0]; ++pos[0] )
				{
					PerVoxelInfo& voxelInfo = m_volumeBuffer[pos[0]-pmin[0] + edgeLength * (pos[1]-pmin[1] + edgeLength * (pos[2]-pmin[2]))];
					voxelInfo.solid = _chunk.m_model->IsEachChild(pos, level, IsSolid, voxelInfo.type);
				}
		CSParam Sector;
		Sector.edgeLength = edgeLength;//*/

		// New copy method with O(n) runtime
		CSParam Sector;
		Sector.buffer = m_volumeBuffer;
		Sector.edgeLength = (1 << _chunk.m_depth) + 2;
		Sector.level = Math::max( 0, _chunk.m_root[3] - _chunk.m_depth );
		Sector.pmin = (IVec3(_chunk.m_root) << (_chunk.m_root[3] - Sector.level)) - 1;
		memset(m_volumeBuffer, 0, sizeof(m_volumeBuffer));
		_chunk.m_model->Traverse(CopySector, &Sector);

		// Iterate over volume and add any surface voxel to vertex buffer
		int numVoxels = 0;
		IVec3 pos;
		for( pos[2]=1; pos[2]<Sector.edgeLength-1; ++pos[2] )
			for( pos[1]=1; pos[1]<Sector.edgeLength-1; ++pos[1] )
				for( pos[0]=1; pos[0]<Sector.edgeLength-1; ++pos[0] )
				{
					VoxelType current = m_volumeBuffer[pos[0] + Sector.edgeLength * (pos[1] + Sector.edgeLength * pos[2])].type;
					int left =   m_volumeBuffer[pos[0] - 1 + Sector.edgeLength * (pos[1] + Sector.edgeLength * pos[2])].solid ? 0 : 1;
					int right =  m_volumeBuffer[pos[0] + 1 + Sector.edgeLength * (pos[1] + Sector.edgeLength * pos[2])].solid ? 0 : 1;
					int bottom = m_volumeBuffer[pos[0] + Sector.edgeLength * (pos[1] - 1 + Sector.edgeLength * pos[2])].solid ? 0 : 1;
					int top =    m_volumeBuffer[pos[0] + Sector.edgeLength * (pos[1] + 1 + Sector.edgeLength * pos[2])].solid ? 0 : 1;
					int front =  m_volumeBuffer[pos[0] + Sector.edgeLength * (pos[1] + Sector.edgeLength * (pos[2] - 1))].solid ? 0 : 1;
					int back =   m_volumeBuffer[pos[0] + Sector.edgeLength * (pos[1] + Sector.edgeLength * (pos[2] + 1))].solid ? 0 : 1;
					// Check if at least one neighbor is NONE and this is not
					// none -> surface
					if( IsSolid(current) &&
						(left || right || bottom || top || front || back) )
					{
						m_vertexBuffer[numVoxels].SetPosition( pos-1 );
						m_vertexBuffer[numVoxels].SetTexture( int(current) );
						m_vertexBuffer[numVoxels].SetVisibility(left, right, bottom, top, front, back);
						++numVoxels;
					}
				}

		_chunk.m_voxels.Commit(m_vertexBuffer, numVoxels * sizeof(VoxelVertex));
	}

};