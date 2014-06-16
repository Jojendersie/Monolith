#include "cube.h"


Cube::Cube(void):
	m_color(255,255,255),
	m_state(false)
{
}


Cube::~Cube(void)
{
}

void Cube::setState(bool _state)
{
	m_statePre = m_state;
	m_state = _state;
	//
	m_node->setOption(QGLSceneNode::Option::HideNode, !_state);
}

QColor Cube::yCbCrToRGB(unsigned int _code)
{
	int y = _code & 255;
	int pb = (_code >> 18) & 31;
	int pr = (_code >> 23) & 31;
/*	em = (_code >> 17) & 1;
	m_transparent = (_code >> 16) & 1;
	yvar = (_code >> 28) & 15;
	sp = (_code >> 8) & 15;
	sh = (_code >> 12) & 15;*/
	// De_code color to rgb
	int r = std::min(255,std::max(0, (int)std::ceil(y + 0.22627 * (pb-15) + 11.472 * (pr-15))));
	int g = std::min(255,std::max(0, (int)std::ceil(y - 3.0268 * (pb-15) - 5.8708 * (pr-15))));
	int b = std::min(255,std::max(0, (int)std::ceil(y + 14.753 * (pb-15) + 0.0082212 * (pr-15))));
	return QColor(r,g,b);
}
