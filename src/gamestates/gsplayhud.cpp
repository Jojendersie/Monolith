#include "gsplayhud.hpp"


using namespace Math;

namespace Graphic
{
	HudGsPlay::HudGsPlay(Monolith* _game) :
		Hud(_game)
	{
		m_velocityLabel = &CreateLabel(Vec2(0.25f, -0.1f), "");
	}
}