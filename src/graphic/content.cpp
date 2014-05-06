#include "utilities/assert.hpp"
#include "content.hpp"
#include "core/texture.hpp"
#include "core/device.hpp"

namespace Graphic {

	Graphic::Effect* Resources::s_effects[];
	Graphic::UniformBuffer* Resources::s_ubos[];
	Graphic::Font* Resources::s_fonts[];
	Graphic::SamplerState* Resources::s_samplers[];

	// ********************************************************************* //
	// Get one of the predefined effects.
	Graphic::Effect* Resources::GetEffect(Effects _effect)
	{
		if( s_effects[(int)_effect] ) return s_effects[(int)_effect];
		
		// Not loaded yet
		switch( _effect )
		{
		case Effects::VOXEL_RENDER:
			s_effects[(int)_effect] = new Effect( "shader/voxel.vs", "shader/voxel.gs", "shader/voxel.ps", Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID );
			s_effects[(int)_effect]->BindUniformBuffer( *GetUBO(UniformBuffers::OBJECT_VOXEL) );
			s_effects[(int)_effect]->BindUniformBuffer( *GetUBO(UniformBuffers::CAMERA) );
			s_effects[(int)_effect]->BindUniformBuffer( *GetUBO(UniformBuffers::GLOBAL) );
			s_effects[(int)_effect]->BindTexture( "u_diffuseTex", 0, *GetSamplerState(SamplerStates::POINT) );
			break;
		case Effects::TEXTURE_2DQUAD:
			s_effects[(int)_effect] = new Effect( "shader/screentex.vs", "shader/screentex.gs", "shader/screentex.ps", Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID, Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::INV_SRC_ALPHA, Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false );
			s_effects[(int)_effect]->BindTexture( "u_screenTex", 7, *GetSamplerState(SamplerStates::LINEAR) );
			break;
		case Effects::WIRE:
			s_effects[(int)_effect] = new Effect( "shader/wire.vs", "shader/wire.ps", Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID, BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE, Graphic::DepthStencilState::COMPARISON_FUNC::LESS, false );
			s_effects[(int)_effect]->BindUniformBuffer( *GetUBO(UniformBuffers::OBJECT_WIRE) );
			break;
		case Effects::BEAM:
			s_effects[(int)_effect] = new Effect( "shader/beam.vs", "shader/beam.gs", "shader/beam.ps", Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID, BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE, Graphic::DepthStencilState::COMPARISON_FUNC::LESS, false );
			s_effects[(int)_effect]->BindUniformBuffer( *GetUBO(UniformBuffers::OBJECT_WIRE) );
			break;
		}
		return s_effects[(int)_effect];
	}

	// ********************************************************************* //
	// Get one of the predefined uniform buffers.
	Graphic::UniformBuffer* Resources::GetUBO(UniformBuffers _ubo)
	{
		if( s_ubos[(int)_ubo] ) return s_ubos[(int)_ubo];

		// Not loaded yet
		switch( _ubo )
		{
		case UniformBuffers::GLOBAL: {
			s_ubos[(int)_ubo] = new UniformBuffer( "Global" );
			s_ubos[(int)_ubo]->AddAttribute( "Aspect", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			s_ubos[(int)_ubo]->AddAttribute( "Width", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			s_ubos[(int)_ubo]->AddAttribute( "Height", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			s_ubos[(int)_ubo]->AddAttribute( "Time", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			Math::IVec2 size = Graphic::Device::GetBackbufferSize();
			(*s_ubos[(int)_ubo])["Aspect"] = Graphic::Device::GetAspectRatio();
			(*s_ubos[(int)_ubo])["Width"] = (float)size[0];
			(*s_ubos[(int)_ubo])["Height"] = (float)size[1];
			break; }
		case UniformBuffers::CAMERA:
			s_ubos[(int)_ubo] = new UniformBuffer( "Camera" );
			s_ubos[(int)_ubo]->AddAttribute( "View", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
			s_ubos[(int)_ubo]->AddAttribute( "Projection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
			s_ubos[(int)_ubo]->AddAttribute( "ViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
			s_ubos[(int)_ubo]->AddAttribute( "ProjectionInverse", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Position", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC3 );
			break;
		case UniformBuffers::OBJECT_VOXEL:
			s_ubos[(int)_ubo] = new UniformBuffer( "Object" );
			s_ubos[(int)_ubo]->AddAttribute( "WorldViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
			s_ubos[(int)_ubo]->AddAttribute( "WorldView", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
			s_ubos[(int)_ubo]->AddAttribute( "Corner000", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner001", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner010", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner011", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner100", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner101", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner110", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "Corner111", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
			s_ubos[(int)_ubo]->AddAttribute( "MaxOffset", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			break;
		case UniformBuffers::OBJECT_WIRE:
			s_ubos[(int)_ubo] = new UniformBuffer( "Object" );
			s_ubos[(int)_ubo]->AddAttribute( "WorldViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
			s_ubos[(int)_ubo]->AddAttribute( "LineWidth", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			s_ubos[(int)_ubo]->AddAttribute( "BlendSlope", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			s_ubos[(int)_ubo]->AddAttribute( "BlendOffset", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );
			break;
		}

		return s_ubos[(int)_ubo];
	}

	// ********************************************************************* //
	// Get one of the predefined fonts.
	Graphic::Font* Resources::GetFont(Fonts _font)
	{
		if( s_fonts[(int)_font] ) return s_fonts[(int)_font];

		// Not loaded yet
		switch( _font )
		{
		case Fonts::DEFAULT:
			s_fonts[(int)_font] = new Graphic::Font("arial");
			break;
		case Fonts::GAME_FONT:
			s_fonts[(int)_font] = new Graphic::Font("MonoLith");
			break;
		}

		return s_fonts[(int)_font];
	}

	// ********************************************************************* //
	// Get one of the predefined sampler states.
	Graphic::SamplerState* Resources::GetSamplerState(SamplerStates _state)
	{
		if( s_samplers[(int)_state] ) return s_samplers[(int)_state];

		// Not loaded yet
		switch( _state )
		{
			case SamplerStates::POINT:
				s_samplers[(int)_state] = new SamplerState(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::POINT,
					Graphic::SamplerState::SAMPLE::POINT, Graphic::SamplerState::SAMPLE::LINEAR );
				break;
			case SamplerStates::LINEAR:
				s_samplers[(int)_state] = new SamplerState(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::LINEAR,
					Graphic::SamplerState::SAMPLE::LINEAR, Graphic::SamplerState::SAMPLE::LINEAR );
				break;
		}

		return s_samplers[(int)_state];
	}

	// ********************************************************************* //
	// Delete all the loaded resources
	void Resources::Unload()
	{
		for( int i = 0; i < (int)Effects::COUNT; ++i )
		{
			delete s_effects[i];
			s_effects[i] = nullptr;
		}

		for( int i = 0; i < (int)UniformBuffers::COUNT; ++i )
		{
			delete s_ubos[i];
			s_ubos[i] = nullptr;
		}

		for( int i = 0; i < (int)Fonts::COUNT; ++i )
		{
			delete s_fonts[i];
			s_fonts[i] = nullptr;
		}

		for( int i = 0; i < (int)SamplerStates::COUNT; ++i )
		{
			delete s_samplers[i];
			s_samplers[i] = nullptr;
		}
	}


} // namespace Graphic