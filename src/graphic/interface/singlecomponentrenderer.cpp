#include <ei/vector.hpp>
#include "singlecomponentrenderer.hpp"
#include "voxel/chunk.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"

using namespace ei;

namespace Graphic {

SingleComponentRenderer::SingleComponentRenderer() :
	m_voxels( "uu", nullptr, 0, Graphic::VertexBuffer::PrimitiveType::POINT )
{
	int bufferSize = 64 * Voxel::TypeInfo::GetNumVoxels() * sizeof(Voxel::VoxelVertex);
	Voxel::VoxelVertex* vertexData = (Voxel::VoxelVertex*)malloc(bufferSize);
	Voxel::VoxelVertex* appendBuffer = vertexData;
	// Loop over all components
	for( int i = 0; i < Voxel::TypeInfo::GetNumVoxels(); ++i )
	{
		// Loop over all side flags
		for( int j = 0; j < 64; ++j )
		{
			appendBuffer->SetPosition( IVec3(0) );
			appendBuffer->SetVisibility( j );
			appendBuffer->SetTexture( i );
			appendBuffer->material = 0; // Unused for non-hierarchy voxels
			++appendBuffer;
		}
	}

	m_voxels.Commit((void*&)vertexData, bufferSize);
}

void SingleComponentRenderer::Draw( Voxel::ComponentType _type, int _sideFlags, const ei::Mat4x4& _worldView, const ei::Mat4x4& _projection )
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

	Graphic::Device::DrawVertices( m_voxels, int(_type) * 64 + _sideFlags, 1 );
}

} // namespace Graphic