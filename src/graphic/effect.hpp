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
		Effect( const std::string& _VSFile, const std::string& _PSFile );

		/// \brief Construction of a program with pixel, vertex and geometry shader.
		Effect( const std::string& _VSFile, const std::string& _GSFile, const std::string& _PSFile,
			RasterizerState::CULL_MODE _cullMode, RasterizerState::FILL_MODE _fillMode );

		~Effect();

		/// \brief Get the OpenGL id of this shader program.
		unsigned GetProgramID() const	{ return m_programID; }

	private:
		RasterizerState m_rasterizerState;		///< The rasterizer state
	//	SamplerState m_SamplerState[8];			///< One state for each of 8 texture stages
		BlendState m_blendState;				///< Effect blend mode
		DepthStencilState m_depthStencilState;	///< Buffer options

		unsigned m_vertexShader;
		unsigned m_geometryShader;
		unsigned m_pixelShader;
		unsigned m_programID;

		friend class Device;
	};
};