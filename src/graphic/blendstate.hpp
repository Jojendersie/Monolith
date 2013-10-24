#pragma once

namespace Graphic {

	/// \brief State object for alpha blending.
	class BlendState
	{
	public:
		enum struct BLEND_OPERATION {
			ADD = 0x8006,
			SUBTRACT = 0x800A,
			REVERSE_SUBTRACT = 0x800B,
			MIN = 0x8007,
			MAX = 0x8008,
			DISABLE
		};

		enum struct BLEND {
			ZERO = 0,
			ONE = 1,
			SRC_ALPHA = 0x0302,
			INV_SRC_ALPHA = 0x0303,
		};

		/// \brief Create the blend state object.
		BlendState( BLEND_OPERATION _BlendOp, BLEND _SrcOp, BLEND _DstOp );

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return int(m_SrcOp) + int(m_DstOp) * 10 + int(m_BlendOperation) * 100; }

	private:
		BLEND_OPERATION m_BlendOperation;
		BLEND m_SrcOp;
		BLEND m_DstOp;

		/// \brief Set all related states.
		/// \details This can only be called by the device. Use
		///		Device::SetDepthStencilState() to set a state object.
		void Apply() const;
		friend class Device;
	};
};