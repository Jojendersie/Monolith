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


	Chunk::Chunk(Model::ModelData* _modelData, const Math::IVec4& _nodePostion, int _depth) :
		m_modelData( _modelData ),
		m_scale( pow(2.0f, _nodePostion[3]-_depth) ),//float(1<<(_nodePostion[3]-_depth)) ),
		m_depth( _depth ),
		m_root( _nodePostion ),
		m_voxels( "uu", nullptr, 0, Graphic::VertexBuffer::PrimitiveType::POINT ),
		m_position( float(_nodePostion[0]<<_depth), float(_nodePostion[1]<<_depth), float(_nodePostion[2]<<_depth) )
	{
	}

	Chunk::Chunk( Chunk&& _chunk ) :
		m_modelData( _chunk.m_modelData ),
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
			const Math::Mat4x4& _modelView, const Math::Mat4x4& _projection )
	{
		// Translation to center the chunks
		Math::Mat4x4 modelViewProjection = Mat4x4::Translation(m_position) * Mat4x4::Scaling(m_scale) * _modelView;
		_objectConstants["WorldView"] = modelViewProjection;
		modelViewProjection *= _projection;
		_objectConstants["WorldViewProjection"] = modelViewProjection;

		float halfScale = m_scale * 0.5f;
		_objectConstants["Corner000"] = Vec4( -0.5f, -0.5f, -0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner001"] = Vec4( -0.5f, -0.5f,  0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner010"] = Vec4( -0.5f,  0.5f, -0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner011"] = Vec4( -0.5f,  0.5f,  0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner100"] = Vec4(  0.5f, -0.5f, -0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner101"] = Vec4(  0.5f, -0.5f,  0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner110"] = Vec4(  0.5f,  0.5f, -0.5f, 0.0f ) * modelViewProjection;
		_objectConstants["Corner111"] = Vec4(  0.5f,  0.5f,  0.5f, 0.0f ) * modelViewProjection;

		Graphic::Device::DrawVertices( m_voxels, 0, m_voxels.GetNumVertices() );
	}



	// ********************************************************************* //
	/// \brief Current dirty region update - just reuse a child voxel.
	/// \details This is the first part of the update which recreates materials
	///		and solidity flags.
	struct UpdateMaterial: public Model::ModelData::SVOProcessor
	{
		/// \brief If the current voxel is not dirty its whole subtree is
		///		clean too. Then stop.
		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
			return _node->Data().IsDirty();
		}

		/// \brief Do an update: mix the materials of all children and choose
		///		type randomly
		void PostTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
			if( !_node->Data().IsDirty() ) return;

			// Remains undefined
			if( !_node->Children() ) return;

			// Take the last defined children and check solidity.
			bool solid = true;
			for( int i = 0; i < 8; ++i )
			{
				if( _node->Children()[i].Data().type != VoxelType::UNDEFINED ) {
					_node->Data().type = _node->Children()[i].Data().type;
				}
				solid &= _node->Children()[i].Data().solid;
			}
			_node->Data().solid = solid ? 1 : 0;
		}
	};

	/// \brief Update neighborhood visibility.
	/// \details This is the second pass which uses the solidity from first
	///		pass and resets the dirty flag.
	struct UpdateDirty: public Model::ModelData::SVONeighborProcessor
	{
		/// \brief If the current voxel is not dirty its whole subtree is
		///		clean too. Then stop.
		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node,
			const Model::ModelData::SVON* _left, const Model::ModelData::SVON* _right, const Model::ModelData::SVON* _bottom,
			const Model::ModelData::SVON* _top, const Model::ModelData::SVON* _front, const Model::ModelData::SVON* _back)
		{
			return _node->Data().IsDirty();
		}

		/// \brief Do an update: mix the materials of all children and choose
		///		type randomly
		void PostTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node,
			const Model::ModelData::SVON* _left, const Model::ModelData::SVON* _right, const Model::ModelData::SVON* _bottom,
			const Model::ModelData::SVON* _top, const Model::ModelData::SVON* _front, const Model::ModelData::SVON* _back)
		{
			if( !_node->Data().IsDirty() ) return;

			_node->Data().surface = ((_left == nullptr)   || !_left->Data().solid)
				| (((_right == nullptr)  || !_right->Data().solid)  << 1)
				| (((_bottom == nullptr) || !_bottom->Data().solid) << 2)
				| (((_top == nullptr)    || !_top->Data().solid)    << 3)
				| (((_front == nullptr)  || !_front->Data().solid)  << 4)
				| (((_back == nullptr)   || !_back->Data().solid)   << 5);

			// Recompute the material from surface voxels only
			if( _node->Children() )
			{
				Material materials[8];	int num = 0;
				for( int i = 0; i < 8; ++i )
				{
					if( _node->Children()[i].Data().surface ) {
						assert( _node->Children()[i].Data().type != VoxelType::UNDEFINED );
						materials[num++] = _node->Children()[i].Data().material;
					}
				}
				if( num > 0 )
					_node->Data().material = Material(materials, num);
			}

			_node->Data().dirty = 0;
		}
	};

	struct CopySector: public Model::ModelData::SVOProcessor
	{
		ChunkBuilder::PerVoxelInfo* buffer;	///< Array where the selected sector should be stored.
		IVec3 pmin;		///< Minimal boundary
		int level;		///< The level in the octree which should be copied
		int edgeLength;	///< Size of buffer in any direction (is Maximal boundary - Minimal boundary)

		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
			int lvlDiff = _position[3] - level;
			// Position inside target level?
			int span = (1 << lvlDiff) - 1;
			int x = (_position[0] << lvlDiff) - pmin[0];
			int y = (_position[1] << lvlDiff) - pmin[1];
			int z = (_position[2] << lvlDiff) - pmin[2];
			// Inside sector [pmin, pmax]?
			if( (x >= edgeLength) || (x+span < 0) ) return false;
			if( (y >= edgeLength) || (y+span < 0) ) return false;
			if( (z >= edgeLength) || (z+span < 0) ) return false;

			// Is this one of the searched voxels?
			if( lvlDiff == 0 )
			{
				ChunkBuilder::PerVoxelInfo& target = buffer[x + edgeLength * (y + edgeLength * z)];
				target.type = _node->Data().type;
				target.solid = _node->Data().solid;
				return false;
			}

			// If there are no children traversal would stop - set entire area
			//TODO: deprecated Uniform Block optimization is removed
			/*if( !_node->Children() && IsSolid(_node->Data()) )
			{
				int zmin = max(0,z); int zmax = min( z+span+1, edgeLength );
				int ymin = max(0,y); int ymax = min( y+span+1, edgeLength );
				int xmin = max(0,x); int xmax = min( x+span+1, edgeLength );
				for( z=zmin; z<zmax; ++z )
					for( y=ymin; y<ymax; ++y )
						for( x=xmin; x<xmax; ++x )
						{
							ChunkBuilder::PerVoxelInfo& target = buffer[x + edgeLength * (y + edgeLength * z)];
							target.type = _node->Data();
							target.solid = true;
						}
			}*/

			return true;
		}
	};
	
	struct FillBuffer: public Model::ModelData::SVOProcessor
	{
		VoxelVertex* appendBuffer;	///< Pointer to a buffer which is filled as (*appendBuffer++) = ...
		int level;					///< The level in the octree which should be copied
		IVec3 pmin;					///< Minimal boundary

		/// \brief Traverse over the surface only and copy all the vertices
		///		which are in the correct depth.
		bool PreTraversal(const Math::IVec4& _position, Model::ModelData::SVON* _node)
		{
			// Sub-sampling of material
			if( _position[3] > level && !_node->Children() && _node->Data().surface )
			{
				// Compute area where the voxels are created
				int e = 1 << (_position[3] - level);
				IVec3 position(0,0,0);
				IVec3 offset(IVec3(_position) * e - pmin);
				// Collect all vertices from the material definition
				for(; position[2] < e; ++position[2] ) {
					for(position[1] = 0; position[1] < e; ++position[1] ) {
						for(position[0] = 0; position[0] < e; ++position[0] ) {
							appendBuffer->material = TypeInfo::Sample( _node->Data().type, position, _position[3] - level );
							if( appendBuffer->material != Material::UNDEFINED )
							{
								appendBuffer->SetPosition( offset+position );
								appendBuffer->SetVisibility( 0x3f );//_node->Data().surface );
								++appendBuffer;
							}
						}
					}
				}
				return false;
			}

			// Take a LOD of the material
			if( _position[3] == level && _node->Data().surface )
			{
				// Generate a vertex here
				appendBuffer->SetPosition( IVec3(_position)-pmin );
				appendBuffer->SetVisibility( _node->Data().surface );
				appendBuffer->material = _node->Data().material;
				++appendBuffer;
			}

			// Go into recursion if not on target level and current voxel not inside
			return (_position[3] > level) && _node->Data().surface;
		}
	};

	// ********************************************************************* //
	void ChunkBuilder::RecomputeVertexBuffer( Chunk& _chunk )
	{
		// If it is dirty update the subtree
		Model::ModelData::SVON* node = _chunk.m_modelData->Get( IVec3(_chunk.m_root), _chunk.m_root[3] );
		if( node->Data().IsDirty() )
		{
			node->Traverse( _chunk.m_root, UpdateMaterial() );
			node->TraverseEx( _chunk.m_root, UpdateDirty(),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]-1, _chunk.m_root[1]  , _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]+1, _chunk.m_root[1]  , _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]-1, _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]+1, _chunk.m_root[2]  ), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]  , _chunk.m_root[2]-1), _chunk.m_root[3] ),
				_chunk.m_modelData->Get( IVec3(_chunk.m_root[0]  , _chunk.m_root[1]  , _chunk.m_root[2]+1), _chunk.m_root[3] ) );
		}

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
		/*CopySector Sector;
		Sector.buffer = m_volumeBuffer;
		Sector.edgeLength = (1 << _chunk.m_depth) + 2;
		Sector.level = Math::max( 0, _chunk.m_root[3] - _chunk.m_depth );
		Sector.pmin = (IVec3(_chunk.m_root) << (_chunk.m_root[3] - Sector.level)) - 1;
		memset(m_volumeBuffer, 0, sizeof(m_volumeBuffer));
		_chunk.m_modelData->Traverse(Sector);

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
		//*/

		// Newest method O(k): run over surface only
		FillBuffer FillP;
		FillP.appendBuffer = m_vertexBuffer;
		FillP.level = _chunk.m_root[3] - _chunk.m_depth;
		FillP.pmin = (IVec3(_chunk.m_root) << (_chunk.m_root[3] - FillP.level));
		// Using all neighbors == none creates at least the voxels at the chunk boundary.
		// These extra voxels solve a problem when deleting things in a neighbor chunk.
		// Without there would be noticeable holes due to not updating this chunk.
		node->Traverse( _chunk.m_root, FillP );
		int numVoxels = FillP.appendBuffer - m_vertexBuffer;
		//*/

		_chunk.m_voxels.Commit(m_vertexBuffer, numVoxels * sizeof(VoxelVertex));
	}

};