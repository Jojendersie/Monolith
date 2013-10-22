#pragma once

namespace Graphic {

	/// \brief State object for texture sampling modes.
	class SamplerState
	{
	public:
		enum struct EDGE_TREATMENT {
			WRAP,
			CLAMP,
		};

		enum struct SAMPLE {
			POINT,
			LINEAR,
			ANISOTROPH
		};

		/// \brief Create the sampler state object.
		SamplerState( EDGE_TREATMENT _EdgeMode, SAMPLE _MinMagFilter, SAMPLE _MipFilter );

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return int(m_EdgeMode) + int(m_MinMagFilter) * 10 + int(m_MipFilter) * 100; }

	private:
		EDGE_TREATMENT m_EdgeMode;
		SAMPLE m_MinMagFilter;
		SAMPLE m_MipFilter;
	};
};