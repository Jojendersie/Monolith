#pragma once

#include <cstdint>
#include "utilities/assert.hpp"
#include "predeclarations.hpp"
#include "ei/vector.hpp"
#include "graphic/core/vertexbuffer.hpp"
#include "voxel.hpp"
#include "sparseoctree.hpp"
#include "model.hpp"

namespace Graphic { class UniformBuffer; }

namespace Voxel {

	// Values in 2 to 6 are possible chunk sizes
	const int LOG_CHUNK_SIZE = 6;
	const int CHUNK_SIZE = 1<<LOG_CHUNK_SIZE;

	class VoxelVertex
	{
		/// \brief A lot of discrete information.
		///	\details Single bits or groups of bits have different meanings:
		///		0-5: Draw a side (1) or not (0). The order is: Left, Right,
		///			 Bottom, Top, Front, Back
		///		6-11: X coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		12-17: Y coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		18-23: Z coordinate of voxel relative to the grid corner of the
		///			 respective size-dimension (2^s).
		///		24-30: rotation code (see voxel.hpp)
		///		31: is the second code a material (1) or a texture/component type (0).
		uint32 flags;
		uint32 materialOrTexture;	///< The material code is used for hierarchical rendering (only component-groups are encoded by voxels of a single material).

	public:
		VoxelVertex() : flags(0)	{}

		void SetVisibility( int _l, int _r, int _bo, int _t, int _f, int _ba )	{ flags = (flags & 0xffffffc0) | _l | _r<<1 | _bo<<2 | _t<<3 | _f<<4 | _ba<<5; }
		void SetVisibility( int _flags )					{ Assert(_flags < 64, "Invalid visiblity flag."); flags = (flags & 0xffffffc0) | _flags; }
		void SetPosition( const ei::IVec3& _position )	    { flags = (flags & 0xff00003f) | (_position[0] << 6) | (_position[1] << 12) | (_position[2] << 18); }
 		void SetTexture( int _textureIndex )				{ flags &= 0x7fffffff; materialOrTexture = _textureIndex; }
		void SetMaterial( uint32 _materialCode )			{ flags |= 0x80000000; materialOrTexture = _materialCode; }
		// WARNING: only call SetRotation() once and only after flags are set!
		void SetRotation( int _rotationCode );

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
		Chunk(Model::ModelData* _modelData, const ei::IVec4& _nodePostion, int _depth);

		/// \brief Move construction
		Chunk(Chunk&& _chunk);

		virtual ~Chunk();

		/// \brief Fills the constant buffer with the chunk specific data
		///		and draw the voxels.
		/// \details The effect must be set outside.
		/// \param [in] _modelView The actual view matrix.
		///		This matrix should contain the general model transformation too.
		///	\param [in] _projection The projection matrix to precompute the
		///		corner vectors.
		/// \param [in] _time Current game time.
		void Draw( const ei::Mat4x4& _modelView, const ei::Mat4x4& _projection );

		/// \brief Set position relative to the model.
		//void SetPosition( const Math::Vec3& _position )	{ m_position = _position; }

		ei::Vec3 GetPosition()			{ return m_position; }

		/// \brief Get the number of voxels in this chunk
		int NumVoxels() const			{ return m_voxels.GetNumVertices(); }

		/// \brief Test if this chunk was used in the last x seconds.
		/// \param [in] _time Current game time.
		bool IsNotUsedLately() const;
	private:
		/// \brief Reference to the parent model used to access data.
		Model::ModelData* m_modelData;

		float m_scale;					///< Rendering parameter derived from Octree node size
		int m_depth;					///< The depth in the octree respective to this chunk's root. Maximum is 5.
		ei::IVec4 m_root;				///< Position of the root node from this chunk in the model's octree.

		Graphic::VertexArrayBuffer m_voxels;	///< One VoxelVertex value per surface voxel.

		ei::Vec3 m_position;			///< Relative position of the chunk respective to the model.

		double m_lastRendered;			///< Point in time where this chunk was rendered the last time.

		friend class ChunkBuilder;
		friend void Model::ClearChunkCache();

		/// \brief Compute the initial visible voxel set vertex buffer.
		/// \details TODO: This can be done parallel to the render thread because it
		///		only fills the VB and does not upload it.
		///		CURRENTLY IT COMMITS THE BUFFER
		void ComputeVertexBuffer( const ei::IVec3& _nodePostion, int _level );

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
			ComponentType type;	///< type of the target voxel -> texture.
			bool solid;		///< solidity is required to compute neighbor visibility
		};

	private:
		PerVoxelInfo m_volumeBuffer[34*34*34];
	};

	/// \brief A general loop to make voxel iteration easier. The voxel
	///		position is defined through x,y,z inside the loop.
#	define FOREACH_VOXEL(MaxX, MaxY, MaxZ) \
	for( int z=0; z<(MaxZ); ++z ) \
		for( int y=0; y<(MaxY); ++y ) \
			for( int x=0; x<(MaxX); ++x )

};