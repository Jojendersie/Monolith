#include <ei/vector.hpp>
#include "singlecomponentrenderer.hpp"
#include "voxel/chunk.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"

using namespace ei;

namespace Graphic {

SingleComponentRenderer::SingleComponentRenderer() :
m_voxels( Graphic::VertexArrayBuffer::PrimitiveType::POINT, {{VertexAttribute::UINT, 7}, {VertexAttribute::UINT, 8}} )
{
	int bufferSize = 128 * 64 * Voxel::TypeInfo::GetNumVoxels() * sizeof(Voxel::VoxelVertex);
	Voxel::VoxelVertex* vertexData = (Voxel::VoxelVertex*)malloc(bufferSize);
	Voxel::VoxelVertex* appendBuffer = vertexData;
	// Loop over all components
	for( int i = 0; i < Voxel::TypeInfo::GetNumVoxels(); ++i )
	{
		// Loop over all side flags
		for( int j = 0; j < 64; ++j )
		{
			// Loop over all rotations (valid and invalid ones)
			for( int k = 0; k < 128; ++k )
			{
				appendBuffer->SetPosition( IVec3(0) );
				appendBuffer->SetVisibility( j );
				appendBuffer->SetTexture( i );
				appendBuffer->SetRotation( k );
				++appendBuffer;
			}
		}
	}

	m_voxels.GetBuffer(0)->SetData((void*&)vertexData, bufferSize);
}

void SingleComponentRenderer::Draw( const Voxel::Voxel& _component, int _sideFlags, const ei::Mat4x4& _worldView, const ei::Mat4x4& _projection )
{
	Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::OBJECT_VOXEL);
	objectConstants["WorldView"] = _worldView;
	objectConstants["InverseWorldView"] = invert(_worldView);
	Mat4x4 modelViewProjection = _projection * _worldView;

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

	// Rotate side-flags
	/*int rx = _component.rotation & 0x03;
	int ry = (_component.rotation & 0x0c) >> 2;
	int rz = 3 - (rx + ry);
	int flags[6] = { _sideFlags & 0x01, (_sideFlags & 0x02) >> 1, (_sideFlags & 0x04) >> 2,
					(_sideFlags & 0x08) >> 3, (_sideFlags & 0x10) >> 4, (_sideFlags & 0x20) >> 5};
	int sideFlags = ((_component.rotation & 0x10) ? (flags[rx*2]<<1) | (flags[rx*2+1]   ) : (flags[rx*2]   ) | (flags[rx*2+1]<<1))
				  | ((_component.rotation & 0x20) ? (flags[ry*2]<<3) | (flags[ry*2+1]<<2) : (flags[ry*2]<<2) | (flags[ry*2+1]<<3))
				  | ((_component.rotation & 0x40) ? (flags[rz*2]<<5) | (flags[rz*2+1]<<4) : (flags[rz*2]<<4) | (flags[rz*2+1]<<5));*/

	Graphic::Device::DrawVertices( m_voxels, (int(_component.type) * 64 + _sideFlags) * 128 + _component.rotation, 1 );
}

} // namespace Graphic