#include "model.hpp"

using namespace Math;

namespace Voxel {

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Math::Matrix& _viewProjection )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Matrix mModelViewProjection = _viewProjection;

		// Draw all chunks
		for( int i=0; i<m_numChunks; ++i )
		{
			m_chunks[i]->Draw( _objectConstants, mModelViewProjection );
		}
	}

	void Model::Set( const Math::IVec3& _position, int _level, VoxelType _type )
	{
		// Compute which chunk is searched. The chunk position is in units of
		// the smallest voxels but rounded to 32 voxel alignment.
		IVec3 chunkPos;
		chunkPos.x = _position.x << (5-_level);
		chunkPos.y = _position.y << (5-_level);
		chunkPos.z = _position.z << (5-_level);
		// TODO: align

		// Search the correct chunk
		for( int i=0; i<m_numChunks; ++i )
		{
			if( m_chunks[i]->GetPosition() == chunkPos )
			{
				// Add, update and ready
				return;
			}
		}

		// Nothing found create a new chunk
	}

};