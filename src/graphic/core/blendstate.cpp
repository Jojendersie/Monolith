#include "blendstate.hpp"
#include "../../opengl.hpp"

namespace Graphic {
	BlendState::BlendState( BLEND_OPERATION _blendOp, BLEND _srcOp, BLEND _dstOp ) :
		m_blendOperation(_blendOp), m_srcOp(_srcOp), m_dstOp(_dstOp)
	{
	}

	void BlendState::Apply() const
	{
		// TODO: glBlendFunci... for MRT

		if( m_blendOperation == BLEND_OPERATION::DISABLE )
			glDisable( GL_BLEND );
		else {
			glEnable( GL_BLEND );
			glBlendEquation( unsigned(m_blendOperation) );
			glBlendFunc( unsigned(m_srcOp), unsigned(m_dstOp) );
		}
	}
};