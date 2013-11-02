#include "model.hpp"

using namespace Math;

namespace Voxel {

	void Model::Draw( Graphic::UniformBuffer& _ObjectConstants, const Math::Matrix& _mViewProjection )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Matrix mModelViewProjection = _mViewProjection;

		// Draw all chunks
		for( int i=0; i<m_iNumChunks; ++i )
		{
			m_aChunks[i]->Draw( _ObjectConstants, mModelViewProjection );
		}
	}

	void Model::Set( const Math::IVec3& _vPosition, int _iLevel, VoxelType _Type )
	{
		// Compute which chunk is searched. The chunk position is in units of
		// the smallest voxels but rounded to 32 voxel alignment.
		Vec3 vChunkPos;
		vChunkPos.x = _vPosition.x << (5-_iLevel);
		vChunkPos.y = _vPosition.y << (5-_iLevel);
		vChunkPos.z = _vPosition.z << (5-_iLevel);
		// TODO: align

		// Search the correct chunk
		for( int i=0; i<m_iNumChunks; ++i )
		{
			if( m_aChunks[i]->GetPosition() == vChunkPos )
			{
				// Add, update and ready
				return;
			}
		}

		// Nothing found create a new chunk
	}

};