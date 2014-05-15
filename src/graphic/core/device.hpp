#pragma once

#include "predeclarations.hpp"
#include "effect.hpp"
struct GLFWwindow;

namespace Graphic {

	class VertexBuffer;
	class Texture;
	class Framebuffer;

	/// \brief A singleton wrapper to control the render pipeline.
	/// \details The device must be created on program start with the
	///		Initialize method. On shutdown Exit must be called to release the
	///		resources.
	///
	///		The device will create exactly one window on initialization.
	class Device {
	public:
		/// \brief Create a device and a window.
		static void Initialize( int _width, int _height, bool _fullScreen );

		/// \brief Close the window and release the device.
		static void Exit();

		static GLFWwindow* GetWindow();
		static Math::IVec2 GetBackbufferSize();

		/// \brief FramebufferWidth / FramebufferHeight
		static float GetAspectRatio();

		static void SetRasterizerState( const RasterizerState& _state );
		static void SetSamplerState( unsigned _location, const SamplerState& _state );
		static void SetBlendState( const BlendState& _state );
		static void SetDepthStencilState( const DepthStencilState& _state );

		/// \brief Set a shader program and all related states of the pipeline.
		/// \param [in] _effect Compiled shader with associated state objects.
		static void SetEffect( const Effect& _effect );

		/// \brief Bind a texture to a specific texture stage.
		/// \param [in] _texture A texture with any format. The texture tape/
		///		dimension must be consistent with the one from the shader.
		/// \param [in] _location Texture stage. Which texture stages maps to
		///		which uniform variable in the shader is set by Effect::BindTexture.
		static void SetTexture( const Texture& _texture, unsigned _location );


		/// \brief Binds a framebuffer.
		/// \param[in] pFrameBuffer	A framebuffer object, use nullptr to bind the backbuffer.
		/// \param[in] autoViewportSet If true the viewport will be set to 
		///		match the framebuffer's dimensions.
		static void BindFramebuffer(const Framebuffer* _framebuffer, bool _autoViewportSet = true);

		/// \brief Retrieves currently bound framebuffer.
		/// \return nullptr if the hardware backbuffer is active.
		static const Framebuffer* GetCurrentFramebufferBinding();


		/// \brief Clear current framebuffer/backbuffer.
		static void Clear( float _r, float _g, float _b );

		/// \brief Clears the z-buffer only
		static void ClearZ();



		/// \brief Draw call to render direct from a bound vertex buffer
		///		without indices.
		/// \details Commits all state changes and uniform buffers automatically.
		static void DrawVertices( const VertexBuffer& _buffer, int _from, int _count );

	private:
		GLFWwindow* m_window;		///< Reference to the one window created during Initialize()

		/// Currently bound framebuffer object (NULL means backbuffer)
		const Framebuffer* m_BoundFrameBuffer;

		int m_rasterizerState;		///< Hash of the current rasterizer state to optimize unnecessary changes.
		int m_blendState;			///< Hash of the current blend state used for all render targets.
		int m_depthStencilState;	///< Hash of the current depth stencil state to optimize unnecessary changes.
		int m_samplerStates[8];		///< One hash per texture stage which sampler is bound
		const Effect* m_currentEffect;	///< Pointer to the last effect which was set.
	};
};