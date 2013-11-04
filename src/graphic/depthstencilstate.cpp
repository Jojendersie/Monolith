#include "../opengl.hpp"
#include "depthstencilstate.hpp"

namespace Graphic {

	DepthStencilState::DepthStencilState( COMPARISON_FUNC _zTest, bool _zWrite ) :
		m_zWrite(_zWrite), m_stencilRef(false), m_zTest(_zTest)
	{
		ComputeHash();
	}

	DepthStencilState::DepthStencilState( COMPARISON_FUNC _zTest, bool _zWrite, 
						   COMPARISON_FUNC _SFTest, STENCIL_OP _SFFail, STENCIL_OP _SFFailZ, STENCIL_OP _SFPass,
						   COMPARISON_FUNC _SBTest, STENCIL_OP _SBFail, STENCIL_OP _SBFailZ, STENCIL_OP _SBPass) :
		m_zWrite(_zWrite), m_stencilRef(true), m_zTest(_zTest),
		m_SFTest(_SFTest), m_SFFail(unsigned(_SFFail)), m_SFFailZ(unsigned(_SFFailZ)), m_SFPass(unsigned(_SFPass)),
		m_SBTest(_SBTest), m_SBFail(unsigned(_SBFail)), m_SBFailZ(unsigned(_SBFailZ)), m_SBPass(unsigned(_SBPass))
	{
		ComputeHash();
	}

	static unsigned STENCIL_OP_MAP[] = {0x1E00, 0, 0x1E01, 0x8507, 0x8508, 0x1E02, 0x1E03, 0x150A};

	void DepthStencilState::ComputeHash()
	{
		m_hash = (int(m_zTest) & 0xf) | (m_zWrite << 3);
		if( m_stencilRef )
		{
			m_hash = (m_hash << 6) | (int(m_SFTest) & 0xf) | ((int(m_SBTest) & 0xf)<<3);
			m_hash = (m_hash << 3) | (m_SFFail & 0xf);
			m_hash = (m_hash << 3) | (m_SFFailZ & 0xf);
			m_hash = (m_hash << 3) | (m_SFPass & 0xf);
			m_hash = (m_hash << 3) | (m_SBFail & 0xf);
			m_hash = (m_hash << 3) | (m_SBFailZ & 0xf);
			m_hash = (m_hash << 3) | (m_SBPass & 0xf);
		}
	}

	void DepthStencilState::Apply() const
	{
		// Z-Test
		if( m_zTest == COMPARISON_FUNC::ALWAYS ) glDisable(GL_DEPTH_TEST);
		else glEnable(GL_DEPTH_TEST);
		glDepthFunc( unsigned(m_zTest) );

		// Z-Write
		glDepthMask( m_zWrite );

		// Stencil Test
		if( m_stencilRef )
		{
			glEnable( GL_STENCIL_TEST );
			glStencilFuncSeparate( unsigned(m_SFTest), unsigned(m_SBTest), m_stencilRef, 0xffffffff );
			glStencilOpSeparate( GL_FRONT, STENCIL_OP_MAP[m_SFFail], STENCIL_OP_MAP[m_SFFailZ], STENCIL_OP_MAP[m_SFPass] );
			glStencilOpSeparate( GL_BACK, STENCIL_OP_MAP[m_SBFail], STENCIL_OP_MAP[m_SBFailZ], STENCIL_OP_MAP[m_SBPass] );
		} else
			glDisable( GL_STENCIL_TEST );
	}

};