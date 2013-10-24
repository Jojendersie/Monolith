#include "blendstate.hpp"
#include "../opengl.hpp"

namespace Graphic {
	BlendState::BlendState( BLEND_OPERATION _BlendOp, BLEND _SrcOp, BLEND _DstOp ) :
		m_BlendOperation(_BlendOp), m_SrcOp(_SrcOp), m_DstOp(_DstOp)
	{
	}

	void BlendState::Apply() const
	{
		// TODO: glBlendFunci... for MRT

		if( m_BlendOperation == BLEND_OPERATION::DISABLE )
			glDisable( GL_BLEND );
		else {
			glEnable( GL_BLEND );
			glBlendEquation( unsigned(m_BlendOperation) );
			glBlendFunc( unsigned(m_SrcOp), unsigned(m_DstOp) );
		}
	}
};