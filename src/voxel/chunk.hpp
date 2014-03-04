#pragma once

#include <cstdint>
#include <cassert>
#include "../predeclarations.hpp"
#include "../math/vector.hpp"
#include "../graphic/vertexbuffer.hpp"
#include "voxel.hpp"
#include "sparseoctree.hpp"
#include "model.hpp"

namespace Graphic { class UniformBuffer; }

namespace Voxel {

	struct VoxelVertex
	{
		/// \brief A lot of discrete information.
		///	\details Single bits or groups of bits have different meanings:
		///		0-5: Draw a side (1) or not (0). The order is: Left, Right,
		///			 Bottom, Top, Front, Back
		///		6-8: These three bits form a number s in [0,7] which denotes
		///			 the voxel size 2^s. DEPRECATED
		///		9-13: X coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		14-18: Y coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		19-23: Z coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		24-31: 256 texture indices / voxel types.
		uint32_t flags;
		Material material;

		VoxelVertex() : flags(0)	{}

		void SetVisibility( int _l, int _r, int _bo, int _t, int _f, int _ba )	{ flags = (flags & 0xffffffc0) | _l | _r<<1 | _bo<<2 | _t<<3 | _f<<4 | _ba<<5; }
		void SetVisibility( int _flags )					{ assert(_flags < 64); flags = (flags & 0xffffffc0) | _flags; }
// DEPRECATED		void SetSize( int _level )							{ assert(0<=_level && _level<=5); flags = (flags & 0xfffffe3f) | (_level<<6); }
		void SetPosition( const Math::IVec3& _position )	{ flags = (flags & 0xff0001ff) | (_position[0] << 9) | (_position[1] << 14) | (_position[2] << 19); }
		void SetTexture( int _textureIndex )				{ flags = (flags & 0x00ffffff) | (_textureIndex << 24); }
//		void SetHasChildren( bool _bHasChildren )			{ flags = (flags & 0x7fffffff) | (_bHasChildren?0x80000000:0); }

		bool IsVisible() const								{ return (flags & 0x3f) != 0; }
		int GetSize() const									{ return (flags>>6) & 0x7; }
	};


	/// \brief A block of volume information which is rendered in one call
	///		if visible.
	/// \details One chunk covers exactly 32x32x32 voxels (of any size).
	class Chunk
	{
	public:
		/// \brief Constructs a chunk without any voxel (type NONE).
		/// \param [in] _nodePostion Position of the root node from this chunk
		///		in the model's octree.
		///	\param [in] _depth Detail depth respective to the _nodePosition.
		///		The maximum is 5 which means that _nodePosition is the root
		///		of a 32^3 chunk.
		Chunk(Model::ModelData* _modelData, const Math::IVec4& _nodePostion, int _depth);

		/// \brief Move construction
		Chunk(Chunk&& _chunk);

		virtual ~Chunk();

		/// \brief Fills the constant buffer with the chunk specific data
		///		and draw the voxels.
		/// \details The effect must be set outside.
		/// \param [out] _objectConstants A reference to the constant buffer
		///		which must be filled.
		/// \param [in] _modelViewProjection The actual view projection matrix.
		///		This matrix should contain the general model transformation too.
		void Draw( Graphic::UniformBuffer& _objectConstants,
			const Math::Mat4x4& _modelViewProjection );

		/// \brief Set position relative to the model.
		//void SetPosition( const Math::Vec3& _position )	{ m_position = _position; }

		Math::Vec3 GetPosition()		{ return m_position; }

		/// \brief Get the number of voxels in this chunk
		int NumVoxels() const			{ return m_voxels.GetNumVertices(); }
	private:
		/// \brief Reference to the parent model used to access data.
		Model::ModelData* m_modelData;

		float m_scale;					///< Rendering parameter derived from Octree node size
		int m_depth;					///< The depth in the octree respective to this chunk's root. Maximum is 5.
		Math::IVec4 m_root;				///< Position of the root node from this chunk in the model's octree.

		Graphic::VertexBuffer m_voxels;	///< One VoxelVertex value per surface voxel.

		Math::Vec3 m_position;			///< Relative position of the chunk respective to the model.

		friend class ChunkBuilder;

		/// \brief Compute the initial visible voxel set vertex buffer.
		/// \details TODO: This can be done parallel to the render thread because it
		///		only fills the VB and does not upload it.
		///		CURRENTLY IT COMMITS THE BUFFER
		void ComputeVertexBuffer( const Math::IVec3& _nodePostion, int _level );

		// Prevent copy constructor and operator = being generated.
		Chunk(const Chunk&);
		const Chunk& operator = (const Chunk&);
	};


	/// \brief A class to recompute the vertex buffers of chunks.
	/// \details This class contains buffers which are reused in each chunk
	///		rebuild such that less allocations and memory are required.
	class ChunkBuilder
	{
	public:
		void RecomputeVertexBuffer( Chunk& _chunk );

		/// \brief Information from the target volume out of the octree
		struct PerVoxelInfo {
			VoxelType type;	///< type of the target voxel -> texture.
			bool solid;		///< solidity is required to compute neighbor visibility
		};

	private:
		PerVoxelInfo m_volumeBuffer[34*34*34];
		VoxelVertex m_vertexBuffer[32*32*32];
	};

	/// \brief A general loop to make voxel iteration easier. The voxel
	///		position is defined through x,y,z inside the loop.
#	define FOREACH_VOXEL(MaxX, MaxY, MaxZ) \
	for( int z=0; z<(MaxZ); ++z ) \
		for( int y=0; y<(MaxY); ++y ) \
			for( int x=0; x<(MaxX); ++x )

};