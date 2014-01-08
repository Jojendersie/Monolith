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
		//ComputeVertexBuffer(Math::IVec3((const int*)_nodePostion), _nodePostion[3] );
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

	void Chunk::ComputeVertexBuffer( const Math::IVec3& _nodePostion, int _level )
	{
		struct PerVoxelInfo {
			VoxelType type;
			bool solid;
		};
		m_voxels.Clear();

		// Sample a (2^d+2)^3 volume (neighborhood for visibility). This
		// initial sampling avoids the expensive resampling for many voxels.
		// (Inner voxels would be sampled 7 times!)
		int edgeLength = (1 << m_depth) + 2;
		PerVoxelInfo* volume = new PerVoxelInfo[edgeLength*edgeLength*edgeLength];
		int level = Math::max( 0, _level - m_depth );
		IVec3 pmin = (_nodePostion << (_level - level)) - IVec3(1);
		IVec3 pmax = pmin + IVec3(edgeLength);
		IVec3 pos;
		for( pos[2]=pmin[2]; pos[2]<pmax[2]; ++pos[2] )
			for( pos[1]=pmin[1]; pos[1]<pmax[1]; ++pos[1] )
				for( pos[0]=pmin[0]; pos[0]<pmax[0]; ++pos[0] )
				{
					PerVoxelInfo& voxelInfo = volume[pos[0]-pmin[0] + edgeLength * (pos[1]-pmin[1] + edgeLength * (pos[2]-pmin[2]))];
					voxelInfo.solid = m_model->IsEachChild(pos, level, IsSolid, voxelInfo.type);
				}

		// Iterate over volume and add any surface voxel to vertex buffer
		for( pos[2]=1; pos[2]<edgeLength-1; ++pos[2] )
			for( pos[1]=1; pos[1]<edgeLength-1; ++pos[1] )
				for( pos[0]=1; pos[0]<edgeLength-1; ++pos[0] )
				{
					VoxelType current = volume[pos[0] + edgeLength * (pos[1] + edgeLength * pos[2])].type;
					int left = !volume[pos[0] - 1 + edgeLength * (pos[1] + edgeLength * pos[2])].solid;
					int right = !volume[pos[0] + 1 + edgeLength * (pos[1] + edgeLength * pos[2])].solid;
					int bottom = !volume[pos[0] + edgeLength * (pos[1] - 1 + edgeLength * pos[2])].solid;
					int top = !volume[pos[0] + edgeLength * (pos[1] + 1 + edgeLength * pos[2])].solid;
					int front = !volume[pos[0] + edgeLength * (pos[1] + edgeLength * (pos[2] - 1))].solid;
					int back = !volume[pos[0] + edgeLength * (pos[1] + edgeLength * (pos[2] + 1))].solid;
					// Check if at least one neighbor is NONE and this is not
					// none -> surface
					if( IsSolid(current) &&
						(left || right || bottom || top || front || back) )
					{
						VoxelVertex V;
						V.SetPosition( pos-1 );
						V.SetTexture( int(current) );
						V.SetVisibility(left, right, bottom, top, front, back);
						m_voxels.Add( V );
					}
				}

		// Remove temporary sampling
		delete[] volume;

		m_voxels.Commit();
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
	void ChunkBuilder::RecomputeVertexBuffer( Chunk& _chunk )
	{
		// Sample a (2^d+2)^3 volume (neighborhood for visibility). This
		// initial sampling avoids the expensive resampling for many voxels.
		// (Inner voxels would be sampled 7 times!)
		int edgeLength = (1 << _chunk.m_depth) + 2;
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

		// Iterate over volume and add any surface voxel to vertex buffer
		int numVoxels = 0;
		for( pos[2]=1; pos[2]<edgeLength-1; ++pos[2] )
			for( pos[1]=1; pos[1]<edgeLength-1; ++pos[1] )
				for( pos[0]=1; pos[0]<edgeLength-1; ++pos[0] )
				{
					VoxelType current = m_volumeBuffer[pos[0] + edgeLength * (pos[1] + edgeLength * pos[2])].type;
					int left = !m_volumeBuffer[pos[0] - 1 + edgeLength * (pos[1] + edgeLength * pos[2])].solid;
					int right = !m_volumeBuffer[pos[0] + 1 + edgeLength * (pos[1] + edgeLength * pos[2])].solid;
					int bottom = !m_volumeBuffer[pos[0] + edgeLength * (pos[1] - 1 + edgeLength * pos[2])].solid;
					int top = !m_volumeBuffer[pos[0] + edgeLength * (pos[1] + 1 + edgeLength * pos[2])].solid;
					int front = !m_volumeBuffer[pos[0] + edgeLength * (pos[1] + edgeLength * (pos[2] - 1))].solid;
					int back = !m_volumeBuffer[pos[0] + edgeLength * (pos[1] + edgeLength * (pos[2] + 1))].solid;
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