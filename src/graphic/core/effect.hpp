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
		Effect( const std::string& _VSFile, const std::string& _PSFile,
			RasterizerState::CULL_MODE _cullMode = RasterizerState::CULL_MODE::BACK, RasterizerState::FILL_MODE _fillMode = RasterizerState::FILL_MODE::SOLID,
			BlendState::BLEND_OPERATION _blendOp = BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND _srcOp = BlendState::BLEND::ONE, BlendState::BLEND _dstOp = BlendState::BLEND::ZERO,
			DepthStencilState::COMPARISON_FUNC _depthFunc = DepthStencilState::COMPARISON_FUNC::LESS, bool _zWrite = true );

		/// \brief Construction of a program with pixel, vertex and geometry shader.
		Effect( const std::string& _VSFile, const std::string& _GSFile, const std::string& _PSFile,
			RasterizerState::CULL_MODE _cullMode = RasterizerState::CULL_MODE::BACK, RasterizerState::FILL_MODE _fillMode = RasterizerState::FILL_MODE::SOLID,
			BlendState::BLEND_OPERATION _blendOp = BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND _srcOp = BlendState::BLEND::ONE, BlendState::BLEND _dstOp = BlendState::BLEND::ZERO,
			DepthStencilState::COMPARISON_FUNC _depthFunc = DepthStencilState::COMPARISON_FUNC::LESS, bool _zWrite = true);

		~Effect();

		/// \brief Get the OpenGL id of this shader program.
		unsigned GetProgramID() const	{ return m_programID; }

		
		/// \brief Each buffer must be bound to a program once to be used.
		/// \param [in] _uniformBuffer The buffer which should be bound. A
		///		buffer can be bound to many shaders but should not be bound
		///		more than once to the same shader.
		///
		///		The buffer object must be persistent over time. It should
		///		not be deleted before the effect.
		void BindUniformBuffer( UniformBuffer& _uniformBuffer );

		/// \brief Associates a texture sampler with a texture binding point
		/// \param [in] _name Sampler uniform name used in the glsl code.
		/// \param [in] _location Texture binding point. This location is equal
		///		to the one in Device::SetTexture.
		/// \param [in] _sampler A sampler object which defines how textures
		///		are read.
		void BindTexture( const char* _name, unsigned _location, const SamplerState& _sampler );

	private:
		RasterizerState m_rasterizerState;		///< The rasterizer state
		BlendState m_blendState;				///< Effect blend mode
		DepthStencilState m_depthStencilState;	///< Buffer options

		/// \brief A list of uniform buffers which where bound to the effect.
		std::vector<UniformBuffer*> m_boundUniformBuffers;

		/// \brief This struct is a pair which maps texture locations to
		///		sampler states.
		struct SamplerStateBinding {
			unsigned location;				///< Texture stage to which this sampler is applied
			const SamplerState* sampler;	///< A reference to an existing sampler state object.
		};
		/// \brief A list of all samplers for all used texture stages (forced by setter)
		std::vector<SamplerStateBinding> m_samplerStates;

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