#pragma once

namespace Graphic {

	/// \brief State object for alpha blending.
	class BlendState
	{
	public:
		enum struct BLEND_OPERATION {
		};

		/// \brief Create the blend state object.
		BlendState( BLEND_OPERATION _SrcOp, BLEND_OPERATION _DstOp, float _fBlendFactor );

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return int(m_SrcOp) + int(m_DstOp) * 10 + int(m_fBlendFactor*10000.0f) * 100; }

	private:
		BLEND_OPERATION m_SrcOp;
		BLEND_OPERATION m_DstOp;
		float m_fBlendFactor;
	};
};