#include "model.hpp"
#include "voxel.hpp"
#include <cstdlib>

using namespace Math;

namespace Voxel {

	Model::Model() :
		m_chunks(nullptr),
		m_numChunks(0),
		m_position(0.0f),
		m_mass(0.0f),
		m_center(0.0f),
		m_boundingSphereRadius(0.0f)
	{
	}

	Model::~Model()
	{
		for( int i=0; i<m_numChunks; ++i )
			delete m_chunks[i];
		free(m_chunks);
	}

	void Model::Draw( Graphic::UniformBuffer& _objectConstants, const Math::Matrix& _viewProjection )
	{
		// TODO: culling

		// Create a new model space transformation
		Math::Matrix mModelViewProjection = MatrixTranslation(-m_center) * MatrixRotation(m_rotation) * MatrixTranslation( m_position+m_center ) * _viewProjection;

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
		IVec3 chunkPos = _position / (1<<_level);
		IVec3 posInsideChunk = _position - chunkPos * (1<<_level);
		chunkPos *= 32;

		Chunk* targetChunk = nullptr;

		// Search the correct chunk
		for( int i=0; i<m_numChunks; ++i )
		{
			if( m_chunks[i]->GetPosition() == chunkPos )
			{
				// Add, update and ready
				targetChunk = m_chunks[i];
				break;
			}
		}

		// Nothing found create a new chunk
		if( !targetChunk )
		{
			m_chunks = (Chunk**)realloc(m_chunks, sizeof(Chunk*) * (m_numChunks+1));
			targetChunk = m_chunks[m_numChunks] = new Chunk();
			targetChunk->SetPosition( chunkPos );
			++m_numChunks;
		}

		targetChunk->Set(posInsideChunk, _level, _type);

		// Update mass center
		if( m_mass == 0.0f )
			m_center = Vec3(_position);
		else
			m_center = (m_center * m_mass + _position * VOXEL_INFO[int(_type)].mass) / (m_mass + VOXEL_INFO[int(_type)].mass);
		m_mass += VOXEL_INFO[int(_type)].mass;

		// TEMP: approximate a sphere; TODO Grow and shrink a real bounding volume
		m_boundingSphereRadius = Math::max(m_boundingSphereRadius, (m_center - _position).Length() );
	}


	// ********************************************************************* //
};