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
		RasterizerState( CULL_MODE _CullMode, FILL_MODE _FillMode );

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return int(m_CullMode) + int(m_FillMode) * 10; }

	private:
		CULL_MODE m_CullMode;
		FILL_MODE m_FillMode;
	};
};