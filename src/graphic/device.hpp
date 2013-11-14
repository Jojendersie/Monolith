#pragma once

#include "effect.hpp"
struct GLFWwindow;

namespace Graphic {

	class VertexBuffer;
	class Texture;

	/// \brief A singleton wrapper to controll the render pipeline.
	/// \details The device must be created on program start with the
	///		Initialize method. On shutdown Exit must be called to release the
	///		resources.
	///
	///		The device will create exactly one window on initialisation.
	class Device {
	public:
		/// \brief Create a device and a window.
		static void Initialize( int _width, int _height, bool _fullScreen );

		/// \brief Close the window and release the device.
		static void Exit();

		static GLFWwindow* GetWindow();

		static void SetRasterizerState( const RasterizerState& _state );
		static void SetSamplerState( unsigned _location, const SamplerState& _state );
		static void SetBlendState( const BlendState& _state );
		static void SetDepthStencilState( const DepthStencilState& _state );

		static void SetEffect( const Effect& _effect );

		static void SetTexture( const Texture& _texture, unsigned _location );

		/// \brief Clear backbuffer and z-buffer
		static void Clear( float _r, float _g, float _b );

		/// \brief Clears the z-buffer only
		static void ClearZ();

		/// \brief Draw call to render direct from a bound vertex buffer
		///		without indices.
		/// \details Commits all state changes and uniform buffers automatically.
		static void DrawVertices( const VertexBuffer& _buffer, int _from, int _count );

	private:
		GLFWwindow* m_window;

		// Current state TODO: zero init
		int m_rasterizerState;		///< Hash of the current rasterizer state to optimize unnecessary changes.
		int m_blendState;			///< Hash of the current blend state used for all render targets.
		int m_depthStencilState;	///< Hash of the current depth stencil state to optimize unnecessary changes.
		int m_samplerStates[8];		///< One hash per texture stage which sampler is bound
		const Effect* m_currentEffect;	///< Pointer to the last effect which was set.
	};
};