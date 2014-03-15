#pragma once

namespace Graphic {

	/// \brief State object for culling and fill mode.
	class RasterizerState
	{
	public:
		enum struct CULL_MODE {
			FRONT,
			BACK,
			NONE
		};

		enum struct FILL_MODE {
			SOLID,
			WIREFRAME
		};

		/// \brief Create the rasterizer state object.
		RasterizerState( CULL_MODE _cullMode, FILL_MODE _fillMode );

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return int(m_cullMode) + int(m_fillMode) * 10; }

	private:
		CULL_MODE m_cullMode;
		FILL_MODE m_fillMode;

		/// \brief Set all states.
		/// \details This can only be called by the device. Use
		///		Device::SetRasterizerState() to set a state object.
		void Apply() const;

		// OpenGL specific values to set the state fast
		unsigned int m_cullFace;
		unsigned int m_fillModeGL;

		friend class Device;
	};
};