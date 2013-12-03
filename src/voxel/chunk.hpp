#pragma once

#include <cstdint>
#include <cassert>
#include "../predeclarations.hpp"
#include "../graphic/vertexbuffer.hpp"
#include "voxel.hpp"

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
		uint32_t flags;

		VoxelVertex() : flags(0)	{}

		void SetVisibility( int _iL, int _iR, int _iBo, int _iT, int _iF, int _iBa )	{ flags = (flags & 0xffffffa0) | _iL | _iR<<1 | _iBo<<2 | _iT<<3 | _iF<<4 | _iBa<<5; }
		void SetSize( int _level )							{ assert(0<=_level && _level<=5); flags = (flags & 0xfffffe3f) | (_level<<6); }
		void SetPosition( const Math::IVec3& _position )	{ flags = (flags & 0xff0001ff) | (_position.x << 9) | (_position.y<<14) | (_position.z<<19); }
		void SetTexture( int _iTextureIndex )				{ flags = (flags & 0x00ffffff) | (_iTextureIndex << 24); }
//		void SetHasChildren( bool _bHasChildren )			{ flags = (flags & 0x7fffffff) | (_bHasChildren?0x80000000:0); }

		bool IsVisible() const								{ return (flags & 0x3f) != 0; }
		int GetSize() const									{ return (flags>>6) & 0x7; }
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
		/// \param [out] _objectConstants A reference to the constant buffer
		///		which must be filled.
		/// \param [in] _modelViewProjection The actual view projection matrix.
		///		This matrix should contain the general model transformation too.
		/// \param [in] _camera Camera used for culling and LOD computations.
		void Draw( Graphic::UniformBuffer& _objectConstants,
			const Math::Matrix& _modelViewProjection,
			const Input::Camera& _camera,
			const Math::Vec3& _modelPosition );

		/// \brief Compute the visible voxel set vertex buffer.
		/// \details The current implementation uses CPU later this will be
		///		done on GPU
		void ComputeVertexBuffer();

		/// \brief Set a voxel in the octree.
		/// \details All children of the current node and there children
		///		recursive are set to the same value. If all 8 nodes on the 
		///		target level become one type the parent node is updated too.
		/// \param [in] _position Position in [0,31]^3 or [0,1]^3 depending on
		///		the ground size of the current voxel (_level).
		/// \param [in] _level The octree depth. Level 0 is the root node and
		///		5 is the maximal depth.
		/// \param [out] _overwritten A preallocated buffer with at least
		///		32K * sizeof(VoxelVertex).
		///		It will be filled with a list of all overwritten voxels.
		/// \return The number of overwritten voxels.
		int Set( const Math::IVec3& _position, int _level, VoxelType _type, VoxelVertex* _overwritten );

		struct OctreeNode {
			VoxelType GetType() const { return type; }
			bool IsSolid() const { return (flags & 0x01) ? true : false; }
			bool IsUniform() const { return (flags & 0x02) ? true : false; }
			bool IsEmpty() const { return type == VoxelType::NONE; }
		private: friend class Chunk;
			VoxelType type;
			/// \brief Additional structure information.
			/// \details Each bit has its own meaning:
			///		0: Solid. If 0 this voxel has some children with type NONE.
			///		1: Uniform: All children have the same type.
			uint8_t flags;

			OctreeNode() : flags(0x02), type(VoxelType::NONE)	{}
		};

		/// \brief Get a single octree voxel.
		/// \details If the octree ends earlier it terminates with NONE.
		OctreeNode Get( const Math::IVec3& _position, int _level );

		/// \brief Set position relative to the model.
		void SetPosition( const Math::Vec3& _position )	{ m_position = _position; }

		Math::Vec3 GetPosition()		{ return m_position; }
	private:
		/// \brief One memory block for all levels of the octree. Each level
		///		is saved as x+w*(y+w*z) block. Each element is a typeID.
		OctreeNode* m_octree;

		/// \brief Each element of the array contains the number of voxels in
		///		the associated level of detail.
		/// \details Index 0 is the worst lod and 5 the highest.
		///
		///		Invariant: This number is always valid (updated in set)
		int m_lodVoxelNum[6];

		Graphic::VertexBuffer m_voxels;	///< One VoxelVertex value per visible voxel.

		void FillVBRecursive( const Math::IVec3& _position, int _level );

		Math::Vec3 m_position;			///< Relative position of the chunk respective to the model.
	};

	/// \brief A general loop to make voxel iteration easier. The voxel
	///		position is defined through x,y,z insde the loop.
#	define FOREACH_VOXEL(MaxX, MaxY, MaxZ) \
		for( int z=0; z<(MaxZ); ++z ) \
			for( int y=0; y<(MaxY); ++y ) \
				for( int x=0; x<(MaxX); ++x )

};