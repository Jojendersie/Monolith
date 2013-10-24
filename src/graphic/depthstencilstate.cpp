#include "../opengl.hpp"
#include "depthstencilstate.hpp"

namespace Graphic {

	DepthStencilState::DepthStencilState( COMPARISON_FUNC _ZTest, bool _bZWrite ) :
		m_bWriteZ(_bZWrite), m_bUseStencil(false), m_ZTest(_ZTest)
	{
		ComputeHash();
	}

	DepthStencilState::DepthStencilState( COMPARISON_FUNC _ZTest, bool _bZWrite, 
						   COMPARISON_FUNC _SFTest, STENCIL_OP _SFFail, STENCIL_OP _SFFailZ, STENCIL_OP _SFPass,
						   COMPARISON_FUNC _SBTest, STENCIL_OP _SBFail, STENCIL_OP _SBFailZ, STENCIL_OP _SBPass) :
		m_bWriteZ(_bZWrite), m_bUseStencil(true), m_ZTest(_ZTest),
		m_SFTest(_SFTest), m_SFFail(unsigned(_SFFail)), m_SFFailZ(unsigned(_SFFailZ)), m_SFPass(unsigned(_SFPass)),
		m_SBTest(_SBTest), m_SBFail(unsigned(_SBFail)), m_SBFailZ(unsigned(_SBFailZ)), m_SBPass(unsigned(_SBPass))
	{
		ComputeHash();
	}

	static unsigned STENCIL_OP_MAP[] = {0x1E00, 0, 0x1E01, 0x8507, 0x8508, 0x1E02, 0x1E03, 0x150A};

	void DepthStencilState::ComputeHash()
	{
		m_iHash = (int(m_ZTest) & 0xf) | (m_bWriteZ << 3);
		if( m_bUseStencil )
		{
			m_iHash = (m_iHash << 6) | (int(m_SFTest) & 0xf) | ((int(m_SBTest) & 0xf)<<3);
			m_iHash = (m_iHash << 3) | (m_SFFail & 0xf);
			m_iHash = (m_iHash << 3) | (m_SFFailZ & 0xf);
			m_iHash = (m_iHash << 3) | (m_SFPass & 0xf);
			m_iHash = (m_iHash << 3) | (m_SBFail & 0xf);
			m_iHash = (m_iHash << 3) | (m_SBFailZ & 0xf);
			m_iHash = (m_iHash << 3) | (m_SBPass & 0xf);
		}
	}

	void DepthStencilState::Apply() const
	{
		// Z-Test
		if( m_ZTest == COMPARISON_FUNC::ALWAYS ) glDisable(GL_DEPTH_TEST);
		else glEnable(GL_DEPTH_TEST);
		glDepthFunc( unsigned(m_ZTest) );

		// Z-Write
		glDepthMask( m_bWriteZ );

		// Stencil Test
		if( m_bUseStencil )
		{
			glEnable( GL_STENCIL_TEST );
			glStencilFuncSeparate( unsigned(m_SFTest), unsigned(m_SBTest), m_iStencilRef, 0xffffffff );
			glStencilOpSeparate( GL_FRONT, STENCIL_OP_MAP[m_SFFail], STENCIL_OP_MAP[m_SFFailZ], STENCIL_OP_MAP[m_SFPass] );
			glStencilOpSeparate( GL_BACK, STENCIL_OP_MAP[m_SBFail], STENCIL_OP_MAP[m_SBFailZ], STENCIL_OP_MAP[m_SBPass] );
		} else
			glDisable( GL_STENCIL_TEST );
	}

};