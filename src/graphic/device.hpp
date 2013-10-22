#pragma once

#include "effect.hpp"

namespace Graphic {

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

		static void SetRasterizerState( const RasterizerState& _State );
		static void SetSamplerState( const SamplerState& _State );
		static void SetBlendState( const BlendState& _State );
		static void SetDepthStencilState( const DepthStencilState& _State );

		static void SetEffect( const Effect& _Effect );
	private:

		int m_iWindow;
	};
};