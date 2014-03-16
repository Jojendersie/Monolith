#include "rasterizerstate.hpp"
#include "opengl.hpp"

namespace Graphic {
	
	RasterizerState::RasterizerState( CULL_MODE _cullMode, FILL_MODE _fillMode ) :
		m_cullMode(_cullMode), m_fillMode(_fillMode)
	{
		// Uses standard counterclockwise culling
		glFrontFace( GL_CCW );

		// Determine face value to avoid branching later
		m_cullFace = _cullMode == CULL_MODE::BACK ? GL_BACK : GL_FRONT;
		m_fillModeGL = _fillMode == FILL_MODE::SOLID ? GL_FILL : GL_LINE;
	}


	void RasterizerState::Apply() const
	{
		// Set Culling
		if( m_cullMode == CULL_MODE::NONE )
			glDisable( GL_CULL_FACE );
		else
		{
			glEnable( GL_CULL_FACE );
			glCullFace( m_cullFace );
		}

		// Set Polygone mode
		glPolygonMode( GL_FRONT_AND_BACK, m_fillModeGL );
	}

};