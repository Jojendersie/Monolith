#include "rasterizerstate.hpp"
#include "opengl.hpp"

namespace Graphic {
	
	RasterizerState::RasterizerState( CULL_MODE _cullMode, FILL_MODE _fillMode ) :
		m_cullMode(_cullMode), m_fillMode(_fillMode)
	{
		// Determine face value to avoid branching later
		m_cullFace = _cullMode == CULL_MODE::BACK ? GL_BACK : GL_FRONT;
		m_fillModeGL = _fillMode == FILL_MODE::SOLID ? GL_FILL : GL_LINE;
	}


	void RasterizerState::Apply() const
	{
		// Set Culling
		if( m_cullMode == CULL_MODE::NONE )
			GL_CALL(glDisable, GL_CULL_FACE);
		else
		{
			GL_CALL(glEnable, GL_CULL_FACE);
			GL_CALL(glCullFace, m_cullFace);
		}

		// Set Polygone mode
		GL_CALL(glPolygonMode, GL_FRONT_AND_BACK, m_fillModeGL);
	}

};