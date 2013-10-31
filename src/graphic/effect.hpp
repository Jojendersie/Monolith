#pragma once

#include <string>
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
		Effect( const std::string& _sVSFile, const std::string& _sPSFile );

		/// \brief Construction of a program with pixel, vertex and geometry shader.
		Effect( const std::string& _sVSFile, const std::string& _sGSFile, const std::string& _sPSFile,
			RasterizerState::CULL_MODE _CullMode, RasterizerState::FILL_MODE _FillMode );

		~Effect();

		/// \brief Get the OpenGL id of this shader program.
		unsigned GetProgramID() const	{ return m_iProgramID; }

	private:
		RasterizerState m_RasterizerState;		///< The rasterizer state
	//	SamplerState m_SamplerState[8];			///< One state for each of 8 texture stages
		BlendState m_BlendState;				///< Effect blend mode
		DepthStencilState m_DepthStencilState;	///< Buffer options

		unsigned m_iVertexShader;
		unsigned m_iGeometryShader;
		unsigned m_iPixelShader;
		unsigned m_iProgramID;

		friend class Device;
	};
};