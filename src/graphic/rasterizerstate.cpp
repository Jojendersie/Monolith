#include "rasterizerstate.hpp"
#include "../opengl.hpp"

namespace Graphic {
	
	RasterizerState::RasterizerState( CULL_MODE _CullMode, FILL_MODE _FillMode ) :
		m_CullMode(_CullMode), m_FillMode(_FillMode)
	{
		// Uses standard counterclockwise culling
		glFrontFace( GL_CCW );

		// Determine face value to avoid branching later
		m_iCullFace = _CullMode == CULL_MODE::BACK ? GL_BACK : GL_FRONT;
		m_iFillMode = _FillMode == FILL_MODE::SOLID ? GL_FILL : GL_LINE;
	}


	void RasterizerState::Apply() const
	{
		// Set Culling
		if( m_CullMode == CULL_MODE::NONE )
			glDisable( GL_CULL_FACE );
		else
		{
			glEnable( GL_CULL_FACE );
			glCullFace( m_iCullFace );
		}

		// Set Polygone mode
		glPolygonMode( GL_FRONT_AND_BACK, m_iFillMode );
	}

};