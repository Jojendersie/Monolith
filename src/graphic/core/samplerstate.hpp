#pragma once

namespace Graphic {

	/// \brief State object for texture sampling modes.
	class SamplerState
	{
	public:
		enum struct EDGE_TREATMENT {
			WRAP = 0x2901,
			CLAMP = 0x812F,
			MIRROR = 0x8370
		};

		enum struct SAMPLE {
			POINT = 0x2600,
			LINEAR = 0x2601
		};

		/// \brief Create the sampler state object.
		SamplerState( EDGE_TREATMENT _edgeMode, SAMPLE _minFilter, SAMPLE _magFilter, SAMPLE _mipFilter );

		~SamplerState();

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return m_hash; }

	private:
		int m_hash;

		unsigned m_sampler;

		/// \brief Set all related states.
		/// \details This can only be called by the device. Use
		///		Device::SetDepthStencilState() to set a state object.
		/// \param [in] _location Texture stage which should use this sampler
		void Apply( unsigned _location ) const;
		friend class Device;
	};
};