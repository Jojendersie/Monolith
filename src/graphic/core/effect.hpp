#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "rasterizerstate.hpp"
#include "samplerstate.hpp"
#include "blendstate.hpp"
#include "depthstencilstate.hpp"

/// Remove this define to disable FileWatcher on all shader files.
#define AUTO_SHADER_RELOAD

namespace Graphic {

	class UniformBuffer;

	/// \brief An effect is a combination of shaders and state objects.
	class Effect
	{
	public:

		/// \brief Construction of a program with pixel, vertex and optional geometry shader.
		/// \param _VSFile		Path of a vertex shader glsl file.
		/// \param _PSFile		Path of a pixel shader glsl file.
		/// \param _GSFile		Path of a geometry shader glsl file. If this string is empty, no geometry shader will be loaded.
		/// \param _prefixCode	Code that is specified here will be inserted in each (root-)shader file right after the #version tag.
		///						If no #version tag is present, nothing will happen. Additional #include commands are allowed since
		///						_prefixCode will only inserted once in the first parsed code-file.
		///						Currently _prefixCode is not allowed to have include files on its own!
		Effect(const std::string& _VSFile, const std::string& _PSFile, const std::string& _GSFile = "", const std::string& _prefixCode = "");

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
		void BindTexture( const std::string& _name, unsigned _location, const SamplerState& _sampler );


		/// \brief Sets a new rasterizer state.
		/// Default is RasterizerState(RasterizerState::CULL_MODE::BACK, RasterizerState::FILL_MODE::SOLID)
		void SetRasterizerState(const RasterizerState& rasterizerState);

		/// \brief Sets a new rasterizer state.
		/// Default is BlendState(BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::ONE, BlendState::BLEND::ZERO)
		void SetBlendState(const BlendState& blendState);

		/// \brief Sets a new rasterizer state.
		/// Default is DepthStencilState(DepthStencilState::COMPARISON_FUNC::LESS, true)
		void SetDepthStencilState(const DepthStencilState& depthStencilState);

#ifdef AUTO_SHADER_RELOAD
		/// Triggers shader reloads for changed shaders.
		static void UpdateShaderFileWatcher();
#endif

	private:
		/// Returns false if there is something wrong with the shader program.
		bool CheckShaderProgramError();

		RasterizerState m_rasterizerState;		///< The rasterizer state
		BlendState m_blendState;				///< Effect blend mode
		DepthStencilState m_depthStencilState;	///< Buffer options

		/// \brief A list of uniform buffers which where bound to the effect.
		std::vector<UniformBuffer*> m_boundUniformBuffers;

		struct SamplerStateBinding
		{
			SamplerStateBinding(unsigned int location, const SamplerState* sampler) : location(location), sampler(sampler) {}

			unsigned location;				///< Texture stage to which this sampler is applied
			const SamplerState* sampler;	///< A reference to an existing sampler state object.
		};

		/// \brief A list of texture slots and samplers which where bound to the effect.
		std::unordered_map<std::string, SamplerStateBinding> m_boundTextures;

		unsigned m_vertexShader;
		unsigned m_geometryShader;
		unsigned m_pixelShader;
		unsigned m_programID;


		// Debug & auto reload infos:

		std::string m_prefixCode; ///< Used prefix code for loading.
		std::unordered_set<std::string> m_VSFiles;	///< All shader files that were involved in the vertex shader source code. 
		std::unordered_set<std::string> m_GSFiles;	///< All shader files that were involved in the geometry shader source code. 
		std::unordered_set<std::string> m_PSFiles;	///< All shader files that were involved in the pixel shader source code. 

		std::string m_VSMainFile; ///< Main file for the vertex shader.
		std::string m_PSMainFile; ///< Main file for the pixel shader.
		std::string m_GSMainFile; ///< Main file for the geometry shader.

#ifdef AUTO_SHADER_RELOAD

		friend class ShaderFileWatch;

		void AddToFileWatcher();
		void RemoveFromFileWatcher();
		void HandleChangedShaderFile(const std::string& shaderFilename);

#endif

		/// \brief Commit all changes in the bound constant buffers before a
		///		draw call.
		/// \details This is done automatically by the device.
		void CommitUniformBuffers() const;

		friend class Device;
	};
};