#include "chunk.hpp"
#include "../graphic/device.hpp"
#include "../graphic/uniformbuffer.hpp"
#include <cstdlib>
#include <cstring>

using namespace Math;

namespace Voxel {

	/// \brief The level offsets in number of voxels to the start of the octree
	const int LEVEL_OFFSETS[] = {0,1,9,73,585,4681};
	const int NUM_OCTREE_NODES = 37449;

#	define INDEX(P, L)		(LEVEL_OFFSETS[L] + (P.x) + (1<<(L))*((P.y) + (1<<(L))*(P.z)))
#	define INDEX2(X,Y,Z, L)	(LEVEL_OFFSETS[L] + (X) + (1<<(L))*((Y) + (1<<(L))*(Z)))


	Chunk::Chunk() :
		m_Voxels( NUM_OCTREE_NODES, "u", Graphic::VertexBuffer::PrimitiveType::POINT )
	{
		m_Octree = (OctreeNode*)malloc( sizeof(OctreeNode) * NUM_OCTREE_NODES );
		memset( m_Octree, 0, sizeof(OctreeNode) * NUM_OCTREE_NODES );
	}


	Chunk::~Chunk()
	{
		free(m_Octree);
		m_Octree = nullptr;
	}


	void Chunk::Set( const Math::IVec3& _vPosition, int _iLevel, VoxelType _Type )
	{
		int iLevelSize = 1<<_iLevel;
		assert(_vPosition.x >= 0 && _vPosition.x < iLevelSize);
		assert(_vPosition.y >= 0 && _vPosition.y < iLevelSize);
		assert(_vPosition.z >= 0 && _vPosition.z < iLevelSize);

		// Trivialy set
		int iIndex = INDEX( _vPosition, _iLevel);
		m_Octree[iIndex].Type = _Type;
		// A set always forces all children to the same type -> uniform
		m_Octree[iIndex].iFlags = 0x02 | (_Type==VoxelType::NONE ? 0 : 0x01);

		// Fill children with the same type
		if( _iLevel < 5 )
		{
			int iBaseX = _vPosition.x * 2;
			int iBaseY = _vPosition.y * 2;
			int iBaseZ = _vPosition.z * 2;
			Set( IVec3( iBaseX,   iBaseY,   iBaseZ   ), _iLevel+1, _Type );
			Set( IVec3( iBaseX+1, iBaseY,   iBaseZ   ), _iLevel+1, _Type );
			Set( IVec3( iBaseX,   iBaseY+1, iBaseZ   ), _iLevel+1, _Type );
			Set( IVec3( iBaseX+1, iBaseY+1, iBaseZ   ), _iLevel+1, _Type );
			Set( IVec3( iBaseX,   iBaseY,   iBaseZ+1 ), _iLevel+1, _Type );
			Set( IVec3( iBaseX+1, iBaseY,   iBaseZ+1 ), _iLevel+1, _Type );
			Set( IVec3( iBaseX,   iBaseY+1, iBaseZ+1 ), _iLevel+1, _Type );
			Set( IVec3( iBaseX+1, iBaseY+1, iBaseZ+1 ), _iLevel+1, _Type );
		}

		// Update parents
		int iBaseX = _vPosition.x & 0xfffffffe;
		int iBaseY = _vPosition.y & 0xfffffffe;
		int iBaseZ = _vPosition.z & 0xfffffffe;
		while( _iLevel > 0 )
		{
			// Count different types in the neighbourhood and take majority.
			bool bSolid = true;		// False if at least one node is of type NONE
			bool bUniformChildren = true;
			OctreeNode pTypes[] = {
				m_Octree[INDEX2( iBaseX,   iBaseY,   iBaseZ,   _iLevel)],
				m_Octree[INDEX2( iBaseX+1, iBaseY,   iBaseZ,   _iLevel)],
				m_Octree[INDEX2( iBaseX,   iBaseY+1, iBaseZ,   _iLevel)],
				m_Octree[INDEX2( iBaseX+1, iBaseY+1, iBaseZ,   _iLevel)],
				m_Octree[INDEX2( iBaseX,   iBaseY,   iBaseZ+1, _iLevel)],
				m_Octree[INDEX2( iBaseX+1, iBaseY,   iBaseZ+1, _iLevel)],
				m_Octree[INDEX2( iBaseX,   iBaseY+1, iBaseZ+1, _iLevel)],
				m_Octree[INDEX2( iBaseX+1, iBaseY+1, iBaseZ+1, _iLevel)] };
			// Find majority element
			int iMajIdx=0, iMajCount=1;
			bSolid &= pTypes[0].Type != VoxelType::NONE;
			bUniformChildren &= pTypes[0].IsUniform();
			for( int i=1; i<8; ++i )
			{
				bSolid &= pTypes[i].Type != VoxelType::NONE;
				bUniformChildren &= pTypes[i].IsUniform();
				// Moore's Voting algorithm part 1
				if( pTypes[i].Type == pTypes[iMajIdx].Type ) ++iMajCount;
				else --iMajCount;
				if( !iMajCount ) {iMajCount=1; iMajIdx = i;}
			}
			// Passed all tests set parent and go upward
			iBaseX >>= 1;
			iBaseY >>= 1;
			iBaseZ >>= 1;
			--_iLevel;
			iIndex = INDEX2(iBaseX, iBaseY, iBaseZ, _iLevel);
			m_Octree[iIndex].Type = pTypes[iMajIdx].Type;
			m_Octree[iIndex].iFlags = (bSolid ? 1 : 0) | ((bUniformChildren && (iMajCount==8)) ? 2 : 0);
			iBaseX &= 0xfffffffe;
			iBaseY &= 0xfffffffe;
			iBaseZ &= 0xfffffffe;
		}
	}

	Chunk::OctreeNode Chunk::Get( const Math::IVec3& _vPosition, int _iLevel )
	{
		int iLevelSize = 1<<_iLevel;
		if(_vPosition.x < 0 || _vPosition.x >= iLevelSize) return OctreeNode();
		if(_vPosition.y < 0 || _vPosition.y >= iLevelSize) return OctreeNode();
		if(_vPosition.z < 0 || _vPosition.z >= iLevelSize) return OctreeNode();

		return m_Octree[LEVEL_OFFSETS[_iLevel] + _vPosition.x + iLevelSize*(_vPosition.y + iLevelSize*_vPosition.z)];
	}


	void Chunk::ComputeVertexBuffer()
	{
		// Currently CPU method: recursively traverse the octree and add
		// largest voxel if visible.
		m_Voxels.Clear();
		FillVBRecursive( Math::IVec3(0,0,0), 0 );
		m_Voxels.Commit();
	}


	void Chunk::FillVBRecursive( const Math::IVec3& _vPosition, int _iLevel )
	{
		OctreeNode T = Get( _vPosition, _iLevel );
		// Stop at empty voxels
		if( T.GetType()==VoxelType::NONE && T.IsUniform() ) return;

		// Recursion required?
		// If all children have the same type stop recursion.
		bool bHasChildren = !T.IsUniform() && (_iLevel < 5);

		if( T.GetType()!=VoxelType::NONE && !bHasChildren )
		{
			// Add the current node (at least it is added for the LOD)
			VoxelVertex V;
			V.SetVisibility((Get( Math::IVec3(_vPosition.x-1, _vPosition.y, _vPosition.z), _iLevel )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_vPosition.x+1, _vPosition.y, _vPosition.z), _iLevel )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_vPosition.x, _vPosition.y-1, _vPosition.z), _iLevel )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_vPosition.x, _vPosition.y+1, _vPosition.z), _iLevel )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_vPosition.x, _vPosition.y, _vPosition.z-1), _iLevel )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_vPosition.x, _vPosition.y, _vPosition.z+1), _iLevel )).IsSolid() ? 0 : 1 );
			if( V.IsVisible() )
			{
				V.SetPosition( _vPosition );
				V.SetSize( 5-_iLevel );
				V.SetTexture( int(T.GetType()) );
				m_Voxels.Add( &V );
			}
		}

		if( bHasChildren )
		{
			FillVBRecursive( _vPosition * 2 + Math::IVec3(0,0,0), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(1,0,0), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(0,1,0), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(1,1,0), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(0,0,1), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(1,0,1), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(0,1,1), _iLevel+1 );
			FillVBRecursive( _vPosition * 2 + Math::IVec3(1,1,1), _iLevel+1 );
		}
	}

	void Chunk::Draw( Graphic::UniformBuffer& _ObjectConstants, const Math::Matrix& _mViewProjection )
	{
		// TODO: culling & LOD

		// -16 is the translation to center the chunks
		// TODO: mass mid point (Schwerpunkt)
		_ObjectConstants["WorldViewProjection"] = MatrixTranslation(m_vPosition) * _mViewProjection;

		_ObjectConstants["Corner000"] = Vec4( -0.5f, -0.5f, -0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner001"] = Vec4( -0.5f, -0.5f,  0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner010"] = Vec4( -0.5f,  0.5f, -0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner011"] = Vec4( -0.5f,  0.5f,  0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner100"] = Vec4(  0.5f, -0.5f, -0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner101"] = Vec4(  0.5f, -0.5f,  0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner110"] = Vec4(  0.5f,  0.5f, -0.5f, 0.0f ) * _mViewProjection;
		_ObjectConstants["Corner111"] = Vec4(  0.5f,  0.5f,  0.5f, 0.0f ) * _mViewProjection;

		// TODO: auto commit
		_ObjectConstants.Commit();
		Graphic::Device::DrawVertices( m_Voxels, 0, m_Voxels.GetNumVertices() );
	}
};