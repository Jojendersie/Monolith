#include "gsplayhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "utilities/scriptengineinst.hpp"

using namespace ei;

namespace Graphic
{
	HudGsPlay::HudGsPlay(Monolith* _game) :
		Hud(_game, nullptr)
	{
		m_velocityLabel = &CreateScreenElement<TextRender>(PixelOffset(80, -20), Anchor(MidMid, this));
		m_targetVelocityLabel = &CreateScreenElement<TextRender>(PixelOffset(80, 20), Anchor(MidMid, this));
	//	m_velocityLabel = &CreateLabel(Vec2(0.25f, -0.1f), "");
	//	m_targetVelocityLabel = &CreateLabel(Vec2(0.25f, 0.f), "");

		m_mainMessageBox = &CreateMessageBox(Vec2(-0.2f, 0.8f), Vec2(0.4f, 0.2f));
	}

	void HudGsPlay::BuildScriptVars()
	{
		for (auto var : m_scriptVars)
		{
			m_scriptDisplays.push_back(&CreateScreenElement<FillBar>(PixelOffset(50, 100), PixelOffset(300, 30), DefinitionPoint::BotLeft, Anchor(BotLeft, this)));
		}
	}

	void HudGsPlay::UpdateScriptVars()
	{
		for (int i = 0; i < (int)m_scriptVars.size(); ++i)
		{
			m_scriptDisplays[i]->SetFillLevel(m_scriptVars[i]->value);
		}
	}
}