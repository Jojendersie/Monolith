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


	Chunk::Chunk() :
		m_voxels( "u", Graphic::VertexBuffer::PrimitiveType::POINT )
	{
	}


	Chunk::~Chunk()
	{
	}

	void Chunk::ComputeVertexBuffer()
	{
		m_voxels.Clear();

		// Sample a 34^3 volume (neighborhood for visibility). This initial
		// sampling avoids the expensive resampling for many voxels.
		// (Inner voxels would be sampled 7 times!)
		VoxelType* volume = new VoxelType[34*34*34];
		int level = Math::max( 0, m_level - 4 );
		IVec3 pmin = (m_nodePostion << (m_level - level)) - IVec3(1);
		IVec3 pmax = pmin + IVec3(34);
		IVec3 pos;
		for( pos[2]=pmin[2]; pos[2]<pmax[2]; ++pos[2] )
			for( pos[1]=pmin[1]; pos[1]<pmax[1]; ++pos[1] )
				for( pos[0]=pmin[0]; pos[0]<pmax[0]; ++pos[0] )
					m_model->Get(pos, level);

		// Iterate over volume and add any surface voxel to vertex buffer
		pmin += 1;
		pmax -= 1;
		for( pos[2]=pmin[2]; pos[2]<pmax[2]; ++pos[2] )
			for( pos[1]=pmin[1]; pos[1]<pmax[1]; ++pos[1] )
				for( pos[0]=pmin[0]; pos[0]<pmax[0]; ++pos[0] )
				{
					// Check if at least one neighbor is NONE and this is not
					// none -> surface
					if( (volume[pos[0] + 34 * (pos[1] + 34 * pos[2])] != VoxelType::NONE) &&
						((volume[pos[0] - 1 + 34 * (pos[1] + 34 * pos[2])] == VoxelType::NONE) ||
						 (volume[pos[0] + 1 + 34 * (pos[1] + 34 * pos[2])] != VoxelType::NONE) ||
						 (volume[pos[0] + 34 * (pos[1] - 1 + 34 * pos[2])] != VoxelType::NONE) ||
						 (volume[pos[0] + 34 * (pos[1] + 1 + 34 * pos[2])] != VoxelType::NONE) ||
						 (volume[pos[0] + 34 * (pos[1] + 34 * (pos[2] - 1))] != VoxelType::NONE) ||
						 (volume[pos[0] + 34 * (pos[1] + 34 * (pos[2] + 1))] != VoxelType::NONE)) )
					{
						VoxelVertex V;
						V.SetPosition( pos );
						V.SetSize( 0 );	// TODO: deprecated
						V.SetTexture( int(volume[pos[0] + 34 * (pos[1] + 34 * pos[2])]) );
						m_voxels.Add( V );
					}
				}

		// Remove temporary sampling
		delete[] volume;

		m_voxels.Commit();
	}


	/*void Chunk::FillVBRecursive( const Model::SVON* _current, int _level,
		const Model::SVON* _left, const Model::SVON* _right,
		const Model::SVON* _bottom, const Model::SVON* _top,
		const Model::SVON* _front, const Model::SVON* _back )
	{
		// TODO: undefined nodes and blue prints
		
		// Stop at empty voxels
		if( _current->type==VoxelType::NONE ) return;

		// Several stop-options: Target level reached or tree not subdevided more
		if( _level == 0 || !_current->children )
		{
			// Is this voxel visible?
			if( _left->IsSolid() && _right->IsSolid() &&
				_bottom->IsSolid() && _top->IsSolid() &&
				_front->IsSolid() && _back->IsSolid() )
			{
				// Add a vertex
			}
		} else if(_current->children)
		{
			--_level;
			// Recursion
			FillVBRecursive(&_current->children[0], _level, &_left->children[4], &_current->children[4], &_bottom->children[2], &_current->children[2], &_front->children[1], &_current->children[1]);
			FillVBRecursive(&_current->children[1], _level, &_left->children[5], &_current->children[5], &_bottom->children[3], &_current->children[3], &_front->children[1], &_current->children[1]);
		}

		// Recursion required?
		// If all children have the same type stop recursion.
		bool hasChildren = !T.IsUniform() && (_level < 5);

		if( T.GetType()!=VoxelType::NONE && !hasChildren )
		{
			// Add the current node (at least it is added for the LOD)
			VoxelVertex V;
			V.SetVisibility((Get( Math::IVec3(_position.x-1, _position.y, _position.z), _level )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_position.x+1, _position.y, _position.z), _level )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_position.x, _position.y-1, _position.z), _level )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_position.x, _position.y+1, _position.z), _level )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_position.x, _position.y, _position.z-1), _level )).IsSolid() ? 0 : 1 ,
							(Get( Math::IVec3(_position.x, _position.y, _position.z+1), _level )).IsSolid() ? 0 : 1 );
			if( V.IsVisible() )
			{
				V.SetPosition( _position );
				V.SetSize( 5-_level );
				V.SetTexture( int(T.GetType()) );
				m_voxels.Add( &V );
			}
		}

		if( hasChildren )
		{
			FillVBRecursive( _position * 2 + Math::IVec3(0,0,0), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(1,0,0), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(0,1,0), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(1,1,0), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(0,0,1), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(1,0,1), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(0,1,1), _level+1 );
			FillVBRecursive( _position * 2 + Math::IVec3(1,1,1), _level+1 );
		}
	}*/

	void Chunk::Draw( Graphic::UniformBuffer& _objectConstants,
			const Math::Mat4x4& _modelViewProjection,
			const Input::Camera& _camera,
			const Math::Vec3& _modelPosition )
	{
		// TODO: culling & LOD
		Vec3 chunkWorldPos = m_position + _modelPosition;
		int lod = (int)Math::min(5.0f, 5.0f/(_camera.GetPosition() - chunkWorldPos).LengthSq() );
		int lodOffset = 0;
		//for( int i=0; i<lod; ++i) lodOffset += m_lodVoxelNum[i];

		// Translation to center the chunks
		_objectConstants["WorldViewProjection"] = Mat4x4::Translation(m_position) * _modelViewProjection;

		_objectConstants["Corner000"] = Vec4( -0.5f, -0.5f, -0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner001"] = Vec4( -0.5f, -0.5f,  0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner010"] = Vec4( -0.5f,  0.5f, -0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner011"] = Vec4( -0.5f,  0.5f,  0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner100"] = Vec4(  0.5f, -0.5f, -0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner101"] = Vec4(  0.5f, -0.5f,  0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner110"] = Vec4(  0.5f,  0.5f, -0.5f, 0.0f ) * _modelViewProjection;
		_objectConstants["Corner111"] = Vec4(  0.5f,  0.5f,  0.5f, 0.0f ) * _modelViewProjection;

		Graphic::Device::DrawVertices( m_voxels, 0, m_voxels.GetNumVertices() );
	}
};