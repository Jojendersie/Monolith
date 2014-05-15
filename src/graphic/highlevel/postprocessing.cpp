#include "postprocessing.h"
#include "screenalignedtriangle.h"
#include "graphic/core/texture.hpp"
#include "graphic/core/effect.hpp"
#include "graphic/core/rasterizerstate.hpp"
#include "graphic/core/device.hpp"
#include "graphic/content.hpp"


namespace Graphic {

	PostProcessing::PostProcessing()
	{
		m_screenOutput = new Effect("shader/postprocessing/screentri.vs", "shader/postprocessing/output.ps",
			RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID,
			BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::ONE, BlendState::BLEND::ZERO,
			DepthStencilState::COMPARISON_FUNC::ALWAYS, false);

		m_screenOutput->BindTexture("screenTex", 0, *Resources::GetSamplerState(SamplerStates::POINT));
  }

	PostProcessing::~PostProcessing()
	{
		delete m_screenOutput;
		m_screenOutput = nullptr;
	}

	void PostProcessing::PerformPostProcessing(const Texture& sceneTexture, const Texture& depthTexture)
	{
		Device::BindFramebuffer(nullptr);
		Device::SetEffect(*m_screenOutput);
		Device::SetTexture(sceneTexture, 0);
		ScreenAlignedTriangle::Draw();
	}

}