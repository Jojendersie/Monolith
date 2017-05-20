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
			switch (var->mode)
			{
			case Script::DisplayMode::Bar:
				m_scriptDisplays.push_back(&CreateScreenElement<FillBar>(PixelOffset(50, 100), PixelOffset(300, 30), DefinitionPoint::BotLeft, Anchor(BotLeft, this)));
				break;
			case Script::DisplayMode::PlainText:
				m_scriptTexts.push_back(&CreateScreenElement<TextRender>(PixelOffset(80, 20), Anchor(MidMid, this)));
				break;
			}
		}
	}

	void HudGsPlay::UpdateScriptVars()
	{
		if (!m_scriptDisplays.size() && !m_scriptTexts.size()) return;
		// the screen elements should still be in the same order they where added
		int barCount = 0;
		int textCount = 0;
		for (auto var : m_scriptVars)
		{
			switch (var->mode)
			{
			case Script::DisplayMode::Bar:
				m_scriptDisplays[barCount]->SetFillLevel(var->value);
				++barCount;
				break;
			case Script::DisplayMode::PlainText:
				m_scriptTexts[textCount]->SetText(var->string);
				++textCount;
				break;
			}
		}
	}
}