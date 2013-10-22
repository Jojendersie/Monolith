#pragma once

#include "rasterizerstate.hpp"
#include "samplerstate.hpp"
#include "blendstate.hpp"
#include "depthstencilstate.hpp"

namespace Graphic {

	/// \brief An effect is a combination of shaders and state objects.
	class Effect
	{
	public:
	private:
		RasterizerState m_RasterizerState;		///< The rasterizer state
		SamplerState m_SamplerState[8];			///< One state for each of 8 texture stages
		BlendState m_BlendState;				///< Effect blend mode
		DepthStencilState m_DepthStencilState;	///< Buffer options
	};
};