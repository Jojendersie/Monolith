#include "gsplayhud.hpp"


using namespace ei;

namespace Graphic
{
	HudGsPlay::HudGsPlay(Monolith* _game) :
		Hud(_game, nullptr)
	{
		m_velocityLabel = &CreateLabel(Vec2(0.25f, -0.1f), "");

		m_mainMessageBox = &CreateMessageBox(Vec2(-0.2f, 0.8f), Vec2(0.4f, 0.2f));
	}
}