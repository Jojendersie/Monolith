#pragma once

namespace Graphic {

	/// \brief State object for z and stencil buffer options.
	class DepthStencilState
	{
	public:

		enum struct COMPARISON_FUNC {
			LESS = 0x0201,			///< Comparison function less '<'
			GREATER = 0x0204,		///< Comparison function greater '>'
			EQUAL = 0x0202,			///< Comparison function equal '=='
			NEVER = 0x0200,			///< Fail always
			ALWAYS = 0x0207			///< Disables z-Testing / stencil test
		};

		enum struct STENCIL_OP {
			KEEP = 0,
			ZERO = 1,
			REPLACE = 2,
			INCREMENT = 3,
			DECREMENT = 4,
			INC_SAT = 5,
			DEC_SAT = 6,
			INVERT = 7,
		};

		/// \brief Create the depth-stencil state object with disabled stencil buffer.
		DepthStencilState( COMPARISON_FUNC _zTest, bool _zWrite );

		/// \brief Create the depth-stencil state object with enabled stencil buffer.
		/// \param [in] _SFTest Stencil test comparison function for front faces.
		DepthStencilState( COMPARISON_FUNC _zTest, bool _zWrite, 
						   COMPARISON_FUNC _SFTest, STENCIL_OP _SFFail, STENCIL_OP _SFFailZ, STENCIL_OP _SFPass,
						   COMPARISON_FUNC _SBTest, STENCIL_OP _SBFail, STENCIL_OP _SBFailZ, STENCIL_OP _SBPass);

		/// \brief Unique number for each different state combination.
		/// \details Two different state objects with the same setup have the
		///		same hash.
		int GetHash() const		{ return m_hash; }

		/// \brief Set stencil reference value for front and back faces
		void SetStencilReference( int _iValue )		{ m_stencilRef = _iValue;}
		int GetStencilReference()					{ return m_stencilRef; }

	private:
		bool m_zWrite, m_useStencil;
		COMPARISON_FUNC m_zTest;
		COMPARISON_FUNC m_SFTest, m_SBTest;
		unsigned m_SFFail, m_SFFailZ, m_SFPass;
		unsigned m_SBFail, m_SBFailZ, m_SBPass;
		int m_stencilRef;
		int m_hash;

		/// \brief Set all related states.
		/// \details This can only be called by the device. Use
		///		Device::SetDepthStencilState() to set a state object.
		void Apply() const;
		friend class Device;

		void ComputeHash();
	};
};