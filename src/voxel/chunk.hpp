#pragma once

#include <cstdint>
#include "../graphic/vertexbuffer.hpp"
#include "voxel.hpp"
#include <cassert>

namespace Graphic { class UniformBuffer; }

namespace Voxel {

	struct VoxelVertex
	{
		/// \brief A lot of discrete infromation.
		///	\details Single bits or groups of bits have different meanings:
		///		0-5: Draw a side (1) or not (0). The order is: Left, Right,
		///			 Bottom, Top, Front, Back
		///		6-8: These three bits form a number s in [0,7] which denotes
		///			 the voxel size 2^s.
		///		9-13: X coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		14-18: Y coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		19-23: Z coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		24-31: 256 texture indices / voxel types.
		uint32_t iFlags;

		VoxelVertex() : iFlags(0)	{}

		void SetVisibility( int _iL, int _iR, int _iBo, int _iT, int _iF, int _iBa )	{ iFlags = (iFlags & 0xffffffa0) | _iL | _iR<<1 | _iBo<<2 | _iT<<3 | _iF<<4 | _iBa<<5; }
		void SetSize( int _iLevel )							{ assert(0<=_iLevel && _iLevel<=5); iFlags = (iFlags & 0xfffffe3f) | (_iLevel<<6); }
		void SetPosition( const Math::IVec3& _vPosition )	{ iFlags = (iFlags & 0xff0001ff) | (_vPosition.x << 9) | (_vPosition.y<<14) | (_vPosition.z<<19); }
		void SetTexture( int _iTextureIndex )				{ iFlags = (iFlags & 0x00ffffff) | (_iTextureIndex << 24); }
//		void SetHasChildren( bool _bHasChildren )			{ iFlags = (iFlags & 0x7fffffff) | (_bHasChildren?0x80000000:0); }

		bool IsVisible() const								{ return (iFlags & 0x3f) != 0; }
		int GetSize() const									{ return (iFlags>>6) & 0x7; }
	};


	/// \brief A block of volume information which is rendered in one call
	///		if visible.
	/// \details One chunk consists of exactly 32x32x32 voxels.
	class Chunk
	{
	public:
		/// \brief Constructs a chunk without any voxel (type NONE).
		Chunk();

		virtual ~Chunk();

		/// \brief Fills the constant buffer with the chunk specific data
		///		and draw the voxels.
		/// \details The effect must be set outside.
		/// \param [out] _ObjectConstants A reference to the constant buffer
		///		which must be filled.
		/// \param [in] _mViewProjection The actual view projection matrix. TODO: camera mit culling
		void Draw( Graphic::UniformBuffer& _ObjectConstants, const Math::Matrix& _mViewProjection );

		/// \brief Compute the visible voxel set vertex buffer.
		/// \details The current implementation uses CPU later this will be
		///		done on GPU
		void ComputeVertexBuffer();

		/// \brief Set a voxel in the octree.
		/// \details All children of the current node and there children
		///		recursive are set to the same value. If all 8 nodes on the 
		///		target level become one type the parent node is updated too.
		/// \param [in] _vPosition Position in [0,31]^3 or [0,1]^3 depending of
		///		the ground size of the current voxel.
		/// \param [in] _iLevel The octree depth. Level 0 is the root node and
		///		5 is the maximal depth.
		void Set( const Math::IVec3& _vPosition, int _iLevel, VoxelType _Type );

		struct OctreeNode {
			VoxelType GetType() const { return Type; }
			bool IsSolid() const { return (iFlags & 0x01) ? true : false; }
			bool IsUniform() const { return (iFlags & 0x02) ? true : false; }
			bool IsEmpty() const { return Type == VoxelType::NONE; }
		private: friend class Chunk;
			VoxelType Type;
			/// \brief Additional structure information.
			/// \details Each bit has its own meaning:
			///		0: Solid. If 0 this voxel has some children with type NONE.
			///		1: Uniform: All children have the same type.
			uint8_t iFlags;

			OctreeNode() : iFlags(0x02), Type(VoxelType::NONE)	{}
		};

		/// \brief Get a single octree voxel.
		/// \details If the octree ends earlier it terminates with NONE.
		OctreeNode Get( const Math::IVec3& _vPosition, int _iLevel );
	private:
		/// \brief One memory block for all levels of the octree. Each level
		///		is saved as x+w*(y+w*z) block. Each element is a typeID.
		OctreeNode* m_Octree;		

		Graphic::VertexBuffer m_Voxels;	///< One VoxelVertex value per visible voxel.

		void FillVBRecursive( const Math::IVec3& _vPosition, int _iLevel );
	};

	/// \brief A generall loop to make voxel iteration easier. The voxel
	///		position is defined through x,y,z insde the loop.
#	define FOREACH_VOXEL(MaxX, MaxY, MaxZ) \
		for( int z=0; z<(MaxZ); ++z ) \
			for( int y=0; y<(MaxY); ++y ) \
				for( int x=0; x<(MaxX); ++x )

};