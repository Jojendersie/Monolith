#include "postprocessing.h"
#include "screenalignedtriangle.h"
#include "graphic/core/texture.hpp"
#include "graphic/core/framebuffer.hpp"
#include "graphic/core/effect.hpp"
#include "graphic/core/rasterizerstate.hpp"
#include "graphic/core/device.hpp"
#include "graphic/content.hpp"

namespace Graphic {

	PostProcessing::PostProcessing(AmbientOcclusionConfig _aoConfig) :
		m_screenOutputEffect(nullptr),
		m_ambientOcclusionEffect(nullptr),
		m_ambientOcclusionBlurXEffect(nullptr),
		m_ambientOcclusionBlurYEffect(nullptr)
	{
		for (int i = 0; i < 2; ++i)
		{
			m_ambientOcclusionFB[i] = nullptr;
			m_ambientOcclusion[i] = nullptr;
		}

		Reconfigure(_aoConfig);
	}

	PostProcessing::~PostProcessing()
	{
		delete m_screenOutputEffect;
		delete m_ambientOcclusionEffect;
		delete m_ambientOcclusionBlurXEffect;
		delete m_ambientOcclusionBlurYEffect;

		for (int i = 0; i < 2; ++i)
		{
			delete m_ambientOcclusionFB[i];
			delete m_ambientOcclusion[i];
		}
	}

	void PostProcessing::Reconfigure(AmbientOcclusionConfig _aoConfig)
	{
		m_ambientOcclusionConfig = _aoConfig;

		// This will recreate all screen size dependent textures according to the current config.
		OnScreenResized();
		// Similar goes for this one!
		ReLoadShader();
	}

	void PostProcessing::ReLoadShader()
	{

		RasterizerState noCull(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID);
		DepthStencilState noDepthTest(DepthStencilState::COMPARISON_FUNC::ALWAYS, false);

		// Shader setup
		delete m_screenOutputEffect;
		m_screenOutputEffect = new Effect("shader/postprocessing/screentri.vs", "shader/postprocessing/output.ps", "", 
											m_ambientOcclusionConfig != AmbientOcclusionConfig::OFF ? "#define AMBIENT_OCCULSION" : "");
		m_screenOutputEffect->SetRasterizerState(noCull);
		m_screenOutputEffect->SetDepthStencilState(noDepthTest);
		m_screenOutputEffect->BindUniformBuffer(Resources::GetUBO(UniformBuffers::GLOBAL));
		m_screenOutputEffect->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
		m_screenOutputEffect->BindTexture("ambientOcclusionMap", 1, Resources::GetSamplerState(SamplerStates::LINEAR_NOMIPMAP));
		
		delete m_ambientOcclusionEffect;
		m_ambientOcclusionEffect = nullptr;
		delete m_ambientOcclusionBlurXEffect;
		m_ambientOcclusionBlurXEffect = nullptr;
		delete m_ambientOcclusionBlurYEffect;
		m_ambientOcclusionBlurYEffect = nullptr;

		if (m_ambientOcclusionConfig != AmbientOcclusionConfig::OFF)
		{
			static const char* qualityDefines[] = { "#define LOWQ", "#define MEDIUMQ", "#define HIGHQ" };
			const std::string curQualityDefine = qualityDefines[static_cast<unsigned int>(m_ambientOcclusionConfig)-1];

			m_ambientOcclusionEffect = new Effect("shader/postprocessing/screentri.vs", "shader/postprocessing/ambientocclusion.ps", "", curQualityDefine);
			m_ambientOcclusionEffect->SetRasterizerState(noCull);
			m_ambientOcclusionEffect->SetDepthStencilState(noDepthTest);
			m_ambientOcclusionEffect->BindTexture("depthTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			m_ambientOcclusionEffect->BindUniformBuffer(Resources::GetUBO(UniformBuffers::CAMERA));

			m_ambientOcclusionBlurXEffect = new Effect("shader/postprocessing/screentri.vs", "shader/postprocessing/ambientocclusionblur.ps", "", curQualityDefine);
			m_ambientOcclusionBlurXEffect->SetRasterizerState(noCull);
			m_ambientOcclusionBlurXEffect->SetDepthStencilState(noDepthTest);
			m_ambientOcclusionBlurXEffect->BindUniformBuffer(Resources::GetUBO(UniformBuffers::GLOBAL));
			m_ambientOcclusionBlurXEffect->BindUniformBuffer(Resources::GetUBO(UniformBuffers::CAMERA));
			m_ambientOcclusionBlurXEffect->BindTexture("depthTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			m_ambientOcclusionBlurXEffect->BindTexture("ambientOcclusionMap", 1, Resources::GetSamplerState(SamplerStates::POINT));

			m_ambientOcclusionBlurYEffect = new Effect("shader/postprocessing/screentri.vs", "shader/postprocessing/ambientocclusionblur.ps", "", "#define BLUR_Y_FIN\n" + curQualityDefine);
			m_ambientOcclusionBlurYEffect->SetRasterizerState(noCull);
			m_ambientOcclusionBlurYEffect->SetDepthStencilState(noDepthTest);
			m_ambientOcclusionBlurYEffect->BindUniformBuffer(Resources::GetUBO(UniformBuffers::GLOBAL));
			m_ambientOcclusionBlurYEffect->BindUniformBuffer(Resources::GetUBO(UniformBuffers::CAMERA));
			m_ambientOcclusionBlurYEffect->BindTexture("depthTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			m_ambientOcclusionBlurYEffect->BindTexture("ambientOcclusionMap", 1, Resources::GetSamplerState(SamplerStates::POINT));
		}
	}

	void PostProcessing::OnScreenResized()
	{
		for (int i = 0; i < 2; i++)
		{
			delete m_ambientOcclusion[i];
			m_ambientOcclusion[i] = nullptr;
			delete m_ambientOcclusionFB[i];
			m_ambientOcclusionFB[i] = nullptr;
		}

		// Ambient occlusion textures
		if (m_ambientOcclusionConfig != AmbientOcclusionConfig::OFF)
		{
			for (int i = 0; i < 2; i++)
			{
				m_ambientOcclusion[i] = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
					Texture::Format(1, 16, Texture::Format::ChannelType::FLOAT), 1);
				m_ambientOcclusionFB[i] = new Framebuffer(m_ambientOcclusion[i]);
			}
		}
	}

	void PostProcessing::PerformPostProcessing(const Texture& sceneTexture, const Texture& depthTexture)
	{
		if (m_ambientOcclusionConfig != AmbientOcclusionConfig::OFF)
		{
			Device::SetTexture(depthTexture, 0);

			// Compute ambient occlusion
			Device::BindFramebuffer(m_ambientOcclusionFB[0]);
			Device::SetEffect(*m_ambientOcclusionEffect);
			ScreenAlignedTriangle::Draw();
			// Blur X
			Device::BindFramebuffer(m_ambientOcclusionFB[1]);
			Device::SetEffect(*m_ambientOcclusionBlurXEffect);
			Device::SetTexture(*m_ambientOcclusion[0], 1);
			ScreenAlignedTriangle::Draw();
			// Blur Y
			Device::BindFramebuffer(m_ambientOcclusionFB[0]);
			Device::SetEffect(*m_ambientOcclusionBlurYEffect);
			Device::SetTexture(*m_ambientOcclusion[1], 1);
			ScreenAlignedTriangle::Draw();
		}


		// Combine to hardware backbuffer.
		Device::BindFramebuffer(nullptr);
		Device::SetEffect(*m_screenOutputEffect);
		Device::SetTexture(sceneTexture, 0);
		if (m_ambientOcclusionConfig != AmbientOcclusionConfig::OFF)
			Device::SetTexture(*m_ambientOcclusion[0], 1);
		ScreenAlignedTriangle::Draw();
	}

}