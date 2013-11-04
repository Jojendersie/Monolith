#include "font.hpp"

namespace Graphic
{
	Font::Font() :
		m_effect( "shader/font.vs", "shader/font.gs", "shader/font.ps",
				Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID )
	{
	}

	TextRender::TextRender(Font* _font) :
		m_font(_font),
		m_characters( 1000, "222c11", VertexBuffer::PrimitiveType::POINT )
	{
	}
};