#pragma once

#include <string>
#include <vector>
#include "rasterizerstate.hpp"
#include "samplerstate.hpp"
#include "blendstate.hpp"
#include "depthstencilstate.hpp"

namespace Graphic {

	class UniformBuffer;

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

		
		/// \brief Each buffer must be bound to a program once to be used.
		/// \param [in] _uniformBuffer The buffer which should be bound. A
		///		buffer can be bound to many shaders but should not be bound
		///		more than oneth to the same shader.
		///
		///		The buffer object must be persistent over time. It should
		///		not be deleted before the effect.
		void BindUniformBuffer( UniformBuffer& _uniformBuffer );

	private:
		RasterizerState m_rasterizerState;		///< The rasterizer state
	//	SamplerState m_SamplerState[8];			///< One state for each of 8 texture stages
		BlendState m_blendState;				///< Effect blend mode
		DepthStencilState m_depthStencilState;	///< Buffer options

		/// \brief A list of uniform buffers which where bound to the effect.
		std::vector<UniformBuffer*> m_boundUniformBuffers;

		unsigned m_vertexShader;
		unsigned m_geometryShader;
		unsigned m_pixelShader;
		unsigned m_programID;

		/// \brief Commit all changes in the bound constant buffers before a
		///		draw call.
		/// \details This is done automatically by the device.
		void CommitUniformBuffers() const;

		friend class Device;
	};
};