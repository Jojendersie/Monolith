#include "chunk.hpp"
#include "model.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"
#include "input/camera.hpp"
#include "game.hpp"
#include <cstdlib>
#include <cstring>

using namespace ei;

namespace Voxel {

	void VoxelVertex::SetRotation( int _rotationCode )
	{
		flags = (flags & 0x80ffffff) | (_rotationCode << 24);

		// Rotate side-flags
		/*int rx = _rotationCode & 0x03;
		int ry = (_rotationCode & 0x0c) >> 2;
		int rz = 3 - (rx + ry);
		int flagArray[6] = { flags & 0x01, (flags & 0x02) >> 1, (flags & 0x04) >> 2,
							(flags & 0x08) >> 3, (flags & 0x10) >> 4, (flags & 0x20) >> 5};
		int sideFlags = ((_rotationCode & 0x10) ? (flagArray[rx*2]<<1) | (flagArray[rx*2+1]   ) : (flagArray[rx*2]   ) | (flagArray[rx*2+1]<<1))
					  | ((_rotationCode & 0x20) ? (flagArray[ry*2]<<3) | (flagArray[ry*2+1]<<2) : (flagArray[ry*2]<<2) | (flagArray[ry*2+1]<<3))
					  | ((_rotationCode & 0x40) ? (flagArray[rz*2]<<5) | (flagArray[rz*2+1]<<4) : (flagArray[rz*2]<<4) | (flagArray[rz*2+1]<<5));
		flags = (flags & 0xffffffc0) | sideFlags;*/
		/*if(rx < 3 && ry < 3 && rz < 3 && rx >= 0 && ry >= 0 && rz >= 0)
		{
			int flagArray[6];
			flagArray[rx*2]   = _rotationCode & 0x10 ? (flags & 0x02) >> 1 : flags & 0x01;
			flagArray[rx*2+1] = _rotationCode & 0x10 ? flags & 0x01 : (flags & 0x02) >> 1;
			flagArray[ry*2]   = _rotationCode & 0x20 ? (flags & 0x08) >> 3 : (flags & 0x04) >> 2;
			flagArray[ry*2+1] = _rotationCode & 0x20 ? (flags & 0x04) >> 2 : (flags & 0x08) >> 3;
			flagArray[rz*2]   = _rotationCode & 0x40 ? (flags & 0x20) >> 5 : (flags & 0x10) >> 4;
			flagArray[rz*2+1] = _rotationCode & 0x40 ? (flags & 0x10) >> 4 : (flags & 0x20) >> 5;
			flags = (flags & 0xffffffc0) | flagArray[0] | flagArray[1]<<1 | flagArray[2]<<2 | flagArray[3]<<3 | flagArray[4]<<4 | flagArray[5]<<5;
		}*/
	}

	// ********************************************************************* //
#	define INDEX(P, L)		(LEVEL_OFFSETS[L] + (P.x) + (1<<(L))*((P.y) + (1<<(L))*(P.z)))
#	define INDEX2(X,Y,Z, L)	(LEVEL_OFFSETS[L] + (X) + (1<<(L))*((Y) + (1<<(L))*(Z)))


	Chunk::Chunk(Model::ModelData* _modelData, const IVec4& _nodePostion, int _depth) :
		m_modelData( _modelData ),
		m_scale( pow(2.0f, _nodePostion[3]-_depth) ),//float(1<<(_nodePostion[3]-_depth)) ),
		m_depth( _depth ),
		m_root( _nodePostion ),
		m_voxels( "uu", nullptr, 0, Graphic::VertexBuffer::PrimitiveType::POINT ),
		m_position( float(_nodePostion[0]<<_depth), float(_nodePostion[1]<<_depth), float(_nodePostion[2]<<_depth) )
	{
		// Use an initialization point in the future such that it does not get deleted too fast.
		m_lastRendered = Monolith::Time() + 1.0f;
	}

	Chunk::Chunk( Chunk&& _chunk ) :
		m_modelData( _chunk.m_modelData ),
		m_scale( _chunk.m_scale ),
		m_depth( _chunk.m_depth ),
		m_root( _chunk.m_root ),
		m_voxels( std::move(_chunk.m_voxels) ),
		m_position( _chunk.m_position ),
		m_lastRendered( _chunk.m_lastRendered )
	{
	}


	Chunk::~Chunk()
	{
	}


	void Chunk::Draw( const Mat4x4& _modelView, const Mat4x4& _projection )
	{
		Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::OBJECT_VOXEL);
		// Translation to center the chunks
		Mat4x4 modelView = _modelView * scalingH(m_scale) * translation(m_position);
		objectConstants["WorldView"] = modelView;
		objectConstants["InverseWorldView"] = invert(modelView);
		Mat4x4 modelViewProjection = _projection * modelView;

		Vec4 c000 = modelViewProjection * Vec4( -0.5f, -0.5f, -0.5f, 0.0f );
		Vec4 c001 = modelViewProjection * Vec4( -0.5f, -0.5f,  0.5f, 0.0f );
		Vec4 c010 = modelViewProjection * Vec4( -0.5f,  0.5f, -0.5f, 0.0f );
		Vec4 c011 = modelViewProjection * Vec4( -0.5f,  0.5f,  0.5f, 0.0f );
		objectConstants["Corner000"] = c000;
		objectConstants["Corner001"] = c001;
		objectConstants["Corner010"] = c010;
		objectConstants["Corner011"] = c011;
		objectConstants["Corner100"] = modelViewProjection * Vec4(  0.5f, -0.5f, -0.5f, 0.0f );
		objectConstants["Corner101"] = modelViewProjection * Vec4(  0.5f, -0.5f,  0.5f, 0.0f );
		objectConstants["Corner110"] = modelViewProjection * Vec4(  0.5f,  0.5f, -0.5f, 0.0f );
		objectConstants["Corner111"] = modelViewProjection * Vec4(  0.5f,  0.5f,  0.5f, 0.0f );
		objectConstants["MaxOffset"] = max(len(c000), len(c001), len(c010), len(c011));

		Graphic::Device::DrawVertices( m_voxels, 0, m_voxels.GetNumVertices() );

		// Set the time stamp for the garbage collection
		m_lastRendered = Monolith::Time();
	}


	// ********************************************************************* //
	bool Chunk::IsNotUsedLately() const
	{
		return Monolith::Time() - m_lastRendered > 15.0;
	}

	// ********************************************************************* //
	/// \brief Current dirty region update - just reuse a child voxel.
	/// \details This is the first part of the update which recreates materials
	///		and solidity flags.
	struct UpdateInner: public Model::ModelData::SVOProcessor
	{
		/// \brief If the current voxel is not dirty its whole subtree is
		///		clean too. Then stop.
		bool PreTraversal(const ei::IVec4& _position, Model::ModelData::SVON* _node)
		{
			return _node->Data().IsDirty();
		}

		/// \brief Do an update: mix the materials of all children and choose
		///		type randomly
		void PostTraversal(const ei::IVec4& _position, Model::ModelData::SVON* _node)
		{
			if( !_node->Data().IsDirty() ) return;

			// Remains undefined
			if( !_node->Children() ) return;

			_node->Data().type = ComponentType::UNDEFINED;

			// Take the last defined children and check solidity.
			bool inner = true;
			for( int i = 0; i < 8; ++i )
			{
				inner &= _node->Children()[i].Data().inner;
			}
			_node->Data().inner = inner ? 1 : 0;
		}
	};

	/// \brief Update neighborhood visibility and material.
	/// \details This is the second pass which uses the solidity from first
	///		pass and resets the dirty flag.
	struct UpdateMaterial: public Model::ModelData::SVONeighborProcessor
	{
		/// \brief If the current voxel is not dirty its whole subtree is
		///		clean too. Then stop.
		bool PreTraversal(const ei::IVec4& _position, Model::ModelData::SVON* _node,
			const Model::ModelData::SVON* _left, const Model::ModelData::SVON* _right, const Model::ModelData::SVON* _bottom,
			const Model::ModelData::SVON* _top, const Model::ModelData::SVON* _front, const Model::ModelData::SVON* _back)
		{
			return _node->Data().IsDirty();
		}

		/// \brief Do an update: mix the materials of all children and choose
		///		type randomly
		void PostTraversal(const ei::IVec4& _position, Model::ModelData::SVON* _node,
			const Model::ModelData::SVON* _left, const Model::ModelData::SVON* _right, const Model::ModelData::SVON* _bottom,
			const Model::ModelData::SVON* _top, const Model::ModelData::SVON* _front, const Model::ModelData::SVON* _back)
		{
			if( !_node->Data().IsDirty() ) return;

			int selfInner = _node->Data().inner;
			_node->Data().surface =
				   ((_left == nullptr)   || !(_left->Data().inner || selfInner))
				| (((_right == nullptr)  || !(_right->Data().inner || selfInner))  << 1)
				| (((_bottom == nullptr) || !(_bottom->Data().inner || selfInner)) << 2)
				| (((_top == nullptr)    || !(_top->Data().inner || selfInner))    << 3)
				| (((_front == nullptr)  || !(_front->Data().inner || selfInner))  << 4)
				| (((_back == nullptr)   || !(_back->Data().inner || selfInner))   << 5);

			// Recompute the material from surface voxels only
			if( _node->Children() && _node->Data().surface )
			{
				Material materials[8];	int num = 0;
				for( int i = 0; i < 8; ++i )
				{
					if( _node->Children()[i].Data().surface ) {
					//	Assert( _node->Children()[i].Data().material != Material::UNDEFINED );
						materials[num++] = _node->Children()[i].Data().material;
					}
				}
				if( num > 0 )
					_node->Data().material = Material(materials, num);
				else
					// If all children are non-surface this one is non-surface too.
					// The estimation from solidity was wrong.
					_node->Data().surface = 0;
			}

			_node->Data().dirty = 0;
		}
	};
	
	struct FillBuffer: public Model::ModelData::SVOProcessor
	{
		VoxelVertex* appendBuffer;	///< Pointer to a buffer which is filled as (*appendBuffer++) = ...
		int level;					///< The level in the octree which should be copied
		IVec3 pmin;					///< Minimal boundary

		/// \brief Traverse over the surface only and copy all the vertices
		///		which are in the correct depth.
		bool PreTraversal(const ei::IVec4& _position, Model::ModelData::SVON* _node)
		{
			// Take a LOD of the material
			if( _position[3] == level && _node->Data().surface )
			{
				// Generate a vertex here
				appendBuffer->SetPosition( IVec3(_position)-pmin );
				appendBuffer->SetVisibility( _node->Data().surface );
				appendBuffer->SetRotation( _node->Data().rotation );
				if( _node->Data().type == ComponentType::UNDEFINED )
					appendBuffer->SetMaterial( _node->Data().material.code );
				else
					appendBuffer->SetTexture( (int)_node->Data().type );
				++appendBuffer;
			}

			// Go into recursion if not on target level and current voxel not inside
			return (_position[3] > level) && _node->Data().surface;
		}
	};

	// ********************************************************************* //
	void ChunkBuilder::RecomputeVertexBuffer( Chunk& _chunk )
	{
		VoxelVertex* vertexBuffer = (VoxelVertex*)malloc(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*sizeof(VoxelVertex));

		// If it is dirty update the subtree
		Model::ModelData::SVON* node = _chunk.m_modelData->Get( IVec3(_chunk.m_root), _chunk.m_root[3] );
		if( node->Data().IsDirty() )
		{
			node->Traverse( _chunk.m_root, UpdateInner() );
			node->TraverseEx( _chunk.m_root, UpdateMaterial(),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]-1, _chunk.m_root[1]  , _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]+1, _chunk.m_root[1]  , _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]-1, _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]+1, _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]  , _chunk.m_root[2]-1), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]  , _chunk.m_root[2]+1), _chunk.m_root[3] ) );
		}

		// Newest method O(k): run over surface only
		FillBuffer FillP;
		FillP.appendBuffer = vertexBuffer;
		FillP.level = _chunk.m_root[3] - _chunk.m_depth;
		FillP.pmin = (IVec3(_chunk.m_root) << (_chunk.m_root[3] - FillP.level));
		// Using all neighbors == none creates at least the voxels at the chunk boundary.
		// These extra voxels solve a problem when deleting things in a neighbor chunk.
		// Without there would be noticeable holes due to not updating this chunk.
		node->Traverse( _chunk.m_root, FillP );
		int numVoxels = int(FillP.appendBuffer - vertexBuffer);

		if( numVoxels )
			_chunk.m_voxels.Commit((void*&)vertexBuffer, numVoxels * sizeof(VoxelVertex));
		else free(vertexBuffer);
	}

};