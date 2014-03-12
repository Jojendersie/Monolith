#include <cassert>
#include "../opengl.hpp"
#include "content.hpp"
#include "texture.hpp"
#include "device.hpp"

namespace Graphic {

	Content::Content() :
		voxelRenderEffect( "shader/voxel.vs", "shader/voxel.gs", "shader/voxel.ps", Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID ),
		texture2DEffect( "shader/screentex.vs", "shader/screentex.gs", "shader/screentex.ps", Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID, Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::INV_SRC_ALPHA, Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false),
		objectUBO( "Object" ), cameraUBO( "Camera" ), globalUBO( "Global" ),
		pointSampler(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::POINT,
					Graphic::SamplerState::SAMPLE::POINT, Graphic::SamplerState::SAMPLE::LINEAR ),
		linearSampler(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::LINEAR,
					Graphic::SamplerState::SAMPLE::LINEAR, Graphic::SamplerState::SAMPLE::LINEAR )
	{
		// Init the constant buffers
		objectUBO.AddAttribute( "WorldViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
		objectUBO.AddAttribute( "WorldView", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
		objectUBO.AddAttribute( "Corner000", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner001", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner010", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner011", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner100", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner101", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner110", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		objectUBO.AddAttribute( "Corner111", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );

		cameraUBO.AddAttribute( "View", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
		cameraUBO.AddAttribute( "Projection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
		cameraUBO.AddAttribute( "ViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
		cameraUBO.AddAttribute( "ProjectionInverse", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
		cameraUBO.AddAttribute( "Position", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC3 );
		//cameraUBO.AddAttribute( "Far", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );

		globalUBO.AddAttribute( "Aspect", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
		globalUBO.AddAttribute( "Width", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
		globalUBO.AddAttribute( "Height", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
		globalUBO.AddAttribute( "Time", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
		Math::IVec2 size = Graphic::Device::GetFramebufferSize();
		globalUBO["Aspect"] = Graphic::Device::GetAspectRatio();
		globalUBO["Width"] = (float)size[0];
		globalUBO["Height"] = (float)size[1];

		// Bind constant buffers to effects
		voxelRenderEffect.BindUniformBuffer( objectUBO );
		voxelRenderEffect.BindUniformBuffer( cameraUBO );
		voxelRenderEffect.BindUniformBuffer( globalUBO );
		voxelRenderEffect.BindTexture("u_diffuseTex", 0, pointSampler);
		assert(glGetError() == GL_NO_ERROR);

		//texture2DEffect.BindUniformBuffer( globalUBO );
		assert(glGetError() == GL_NO_ERROR);

		defaultFont = new Font("arial", this);
	}


	Content::~Content()
	{
		delete defaultFont;
	}


} // namespace Graphic