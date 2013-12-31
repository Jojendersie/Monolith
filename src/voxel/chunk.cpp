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


	Chunk::Chunk(const Model* _model, const Math::IVec4& _nodePostion) :
		m_model( _model ),
		m_scale( float(1<<(_nodePostion[3]-5)) ),
		m_voxels( "u", Graphic::VertexBuffer::PrimitiveType::POINT ),
		m_position( _nodePostion[0]<<5, _nodePostion[1]<<5, _nodePostion[2]<<5 )
	{
		ComputeVertexBuffer(Math::IVec3((const int*)_nodePostion), _nodePostion[3] );
	}

	Chunk::Chunk( Chunk&& _chunk ) :
		m_model( _chunk.m_model ),
		m_scale( _chunk.m_scale ),
		m_voxels( std::move(_chunk.m_voxels) ),
		m_position( _chunk.m_position )
	{
	}


	Chunk::~Chunk()
	{
	}

	void Chunk::ComputeVertexBuffer( const Math::IVec3& _nodePostion, int _level )
	{
		m_voxels.Clear();

		// Sample a 34^3 volume (neighborhood for visibility). This initial
		// sampling avoids the expensive resampling for many voxels.
		// (Inner voxels would be sampled 7 times!)
		VoxelType* volume = new VoxelType[34*34*34];
		int level = Math::max( 0, _level - 5 );
		IVec3 pmin = (_nodePostion << (_level - level)) - IVec3(1);
		IVec3 pmax = pmin + IVec3(34);
		IVec3 pos;
		for( pos[2]=pmin[2]; pos[2]<pmax[2]; ++pos[2] )
			for( pos[1]=pmin[1]; pos[1]<pmax[1]; ++pos[1] )
				for( pos[0]=pmin[0]; pos[0]<pmax[0]; ++pos[0] )
					volume[pos[0]-pmin[0] + 34 * (pos[1]-pmin[1] + 34 * (pos[2]-pmin[2]))] = m_model->Get(pos, level);

		// Iterate over volume and add any surface voxel to vertex buffer
		for( pos[2]=1; pos[2]<33; ++pos[2] )
			for( pos[1]=1; pos[1]<33; ++pos[1] )
				for( pos[0]=1; pos[0]<33; ++pos[0] )
				{
					VoxelType current = volume[pos[0] + 34 * (pos[1] + 34 * pos[2])];
					int left = (volume[pos[0] - 1 + 34 * (pos[1] + 34 * pos[2])] == VoxelType::NONE);
					int right = (volume[pos[0] + 1 + 34 * (pos[1] + 34 * pos[2])] == VoxelType::NONE);
					int bottom = (volume[pos[0] + 34 * (pos[1] - 1 + 34 * pos[2])] == VoxelType::NONE);
					int top = (volume[pos[0] + 34 * (pos[1] + 1 + 34 * pos[2])] == VoxelType::NONE);
					int front = (volume[pos[0] + 34 * (pos[1] + 34 * (pos[2] - 1))] == VoxelType::NONE);
					int back = (volume[pos[0] + 34 * (pos[1] + 34 * (pos[2] + 1))] == VoxelType::NONE);
					// Check if at least one neighbor is NONE and this is not
					// none -> surface
					if( (current != VoxelType::NONE) &&
						(left || right || bottom || top || front || back) )
					{
						VoxelVertex V;
						V.SetPosition( pos-1 );
						V.SetSize( 0 );	// TODO: deprecated
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

		_objectConstants["Corner000"] = Vec4( -m_scale, -m_scale, -m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner001"] = Vec4( -m_scale, -m_scale,  m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner010"] = Vec4( -m_scale,  m_scale, -m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner011"] = Vec4( -m_scale,  m_scale,  m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner100"] = Vec4(  m_scale, -m_scale, -m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner101"] = Vec4(  m_scale, -m_scale,  m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner110"] = Vec4(  m_scale,  m_scale, -m_scale, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner111"] = Vec4(  m_scale,  m_scale,  m_scale, 0.0f ) * _modelViewProjection;

		Graphic::Device::DrawVertices( m_voxels, 0, m_voxels.GetNumVertices() );
	}
};