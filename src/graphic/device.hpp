#pragma once

#include "effect.hpp"
struct GLFWwindow;

namespace Graphic {

	class VertexBuffer;

	/// \brief A singleton wrapper to controll the render pipeline.
	/// \details The device must be created on program start with the
	///		Initialize method. On shutdown Exit must be called to release the
	///		resources.
	///
	///		The device will create exactly one window on initialisation.
	class Device {
	public:
		/// \brief Create a device and a window.
		static void Initialize( int _iWidth, int _iHeight, bool _bFullScreen );

		/// \brief Close the window and release the device.
		static void Exit();

		static GLFWwindow* GetWindow();

		static void SetRasterizerState( const RasterizerState& _State );
		static void SetSamplerState( unsigned _iLocation, const SamplerState& _State );
		static void SetBlendState( const BlendState& _State );
		static void SetDepthStencilState( const DepthStencilState& _State );

		static void SetEffect( const Effect& _Effect );

		/// \brief Clear backbuffer and z-buffer
		static void Clear( float _r, float _g, float _b );

		/// \brief Clears the z-buffer only
		static void ClearZ();

		/// \brief Draw call to render direct from a bound vertex buffer
		///		without indices.
		/// \details Commits all state changes and uniform buffers automatically.
		static void DrawVertices( const VertexBuffer& _Buffer, int _iFrom, int _iCount );

	private:
		GLFWwindow* m_Window;

		// Current state TODO: zero init
		int m_iRasterizerState;		///< Hash of the current rasterizer state to optimize unnecessary changes.
		int m_iBlendState;			///< Hash of the current blend state used for all render targets.
		int m_iDepthStencilState;	///< Hash of the current depth stencil state to optimize unnecessary changes.
		int m_piSamplerStates[8];	///< One hash per texture stage which sampler is bound
	};
};