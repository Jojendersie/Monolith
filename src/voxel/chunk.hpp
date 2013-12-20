#pragma once

#include <cstdint>
#include <cassert>
#include "../predeclarations.hpp"
#include "../math/vector3.hpp"
#include "../graphic/vertexbuffer.hpp"
#include "voxel.hpp"

namespace Graphic { class UniformBuffer; }

namespace Voxel {

	struct VoxelVertex
	{
		/// \brief A lot of discrete information.
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
	/// \details One chunk covers exactly 32x32x32 voxels (of any size).
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
			const Math::Mat4x4& _modelViewProjection,
			const Input::Camera& _camera,
			const Math::Vec3& _modelPosition );

		/// \brief Compute the visible voxel set vertex buffer.
		/// \details TODO: This can be done parallel to the render thread because it
		///		only fills the VB and does not upload it.
		///		CURRENTLY IT COMMITS THE BUFFER
		void ComputeVertexBuffer();

		/// \brief Set position relative to the model.
		void SetPosition( const Math::Vec3& _position )	{ m_position = _position; }

		Math::Vec3 GetPosition()		{ return m_position; }
	private:
		/// \brief Reference to the parent model used to access data.
		const Model* m_model;

		/// \brief Position of the root node from this chunk in the model's
		///		octree.
		Math::IVec3 m_nodePostion;

		/// \brief Level of the chunk root node.
		/// \details This should most times be at least 4 (lvl0 is highest
		///		resolution lvl4 covers 32^3 of that resolution).
		int m_level;

		Graphic::VertexBuffer m_voxels;	///< One VoxelVertex value per surface voxel.

		Math::Vec3 m_position;			///< Relative position of the chunk respective to the model.
	};

	/// \brief A general loop to make voxel iteration easier. The voxel
	///		position is defined through x,y,z insde the loop.
#	define FOREACH_VOXEL(MaxX, MaxY, MaxZ) \
	for( int z=0; z<(MaxZ); ++z ) \
		for( int y=0; y<(MaxY); ++y ) \
			for( int x=0; x<(MaxX); ++x )

};