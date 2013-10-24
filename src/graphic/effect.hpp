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
		/// \brief Construction with pixel shader and vertex shader only.
		Effect( const char* _sVSFile, const char* _sPSFile );

		/// \brief Construction of a program with pixel, vertex and geometry shader.
		Effect( const char* _sVSFile, const char* _sGSFile, const char* _sPSFile,
			RasterizerState::CULL_MODE _CullMode, RasterizerState::FILL_MODE _FillMode );

		~Effect();

	private:
		RasterizerState m_RasterizerState;		///< The rasterizer state
	//	SamplerState m_SamplerState[8];			///< One state for each of 8 texture stages
		BlendState m_BlendState;				///< Effect blend mode
		DepthStencilState m_DepthStencilState;	///< Buffer options

		unsigned m_iVertexShader;
		unsigned m_iGeometryShader;
		unsigned m_iPixelShader;
		unsigned m_iProgramID;
	};
};