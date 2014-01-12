#include <cassert>
#include <iostream>
#include "../opengl.hpp"
#include "content.hpp"
#include "texture.hpp"

namespace Graphic {

	Content::Content() :
		voxelRenderEffect( "shader/voxel.vs", "shader/voxel.gs", "shader/voxel.ps",
		Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID ),
		objectUBO( "Object" ), cameraUBO( "Camera" ),
		pointSampler(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::POINT,
					Graphic::SamplerState::SAMPLE::POINT, Graphic::SamplerState::SAMPLE::LINEAR ),
		linearSampler(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::LINEAR,
					Graphic::SamplerState::SAMPLE::LINEAR, Graphic::SamplerState::SAMPLE::LINEAR )
	{
		// Init the constant buffers
		objectUBO.AddAttribute( "WorldViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
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
		//cameraUBO.AddAttribute( "Far", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );

		// Bind constant buffers to effects
		voxelRenderEffect.BindUniformBuffer( objectUBO );
		voxelRenderEffect.BindUniformBuffer( cameraUBO );
		voxelRenderEffect.BindTexture("u_diffuseTex", 0, pointSampler);
		assert(glGetError() == GL_NO_ERROR);

		// Load array texture for all voxels
		std::vector<std::string> textureNames;
		textureNames.push_back( "texture/none.png" );
		textureNames.push_back( "texture/rock1.png" );
		textureNames.push_back( "texture/water.png" );
		try {
			voxelTextures = new Graphic::Texture(textureNames);
		} catch( std::string _message ) {
			std::cerr << "Failed to load voxel textures!\n";
		}
	}


	Content::~Content()
	{
		delete voxelTextures;
	}


} // namespace Graphic