#include "gseditorhud.hpp"
#include "../graphic/interface/pixelcoords.hpp"

using namespace ei;

namespace Graphic
{
	HudGsEditor::HudGsEditor(Monolith* _game, Graphic::SingleComponentRenderer* _componentRenderer) :
		Hud(_game, _componentRenderer)
	{
		m_nameEdit = &CreateScreenElement<EditField>(Vec2(0.55f, -0.2f), Vec2(0.4f, 0.085f));

		ScreenOverlay* el = &CreateScreenElement<ScreenTexture>("frame", Vec2(-0.995f, 0.825f));
		el->Scale(Vec2(0.67f));
		el = &CreateScreenElement<ScreenTexture>("frame180", Vec2(-0.995f, -0.725f));
		el->Scale(Vec2(0.67f));

		m_voxelContainer = CreateContainer(Vec2(-0.98f, -0.9f), Vec2(0.5f, 1.7f));//Math::Vec2(0.6f,1.75f));
		m_voxelContainer->SetScrollable(true);

		
		//box holding informations about the current model
		m_modelInfoContainer = CreateContainer(Vec2(0.55f, -0.9f), Vec2(0.4f, 0.6f));

		m_modelInfoContainer->CreateScreenElement<ScreenTexture>("simpleWindow", Vec2(0.f), Vec2(0.4f, 0.6f), TopLeft, Anchor(TopLeft, m_modelInfoContainer));


		m_modelInfoContainer->CreateScreenElement<TextRender>(Vec2(-0.86f, 0.4f), ScreenPosition::Anchor(), &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT), "mass(kg):", 0.7f);
		m_mass = &m_modelInfoContainer->CreateScreenElement<TextRender>(Vec2(0.32f, 0.4f), ScreenPosition::Anchor(), &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT), "", 0.7f);

		m_modelInfoContainer->CreateScreenElement<TextRender>(Vec2(-0.86f, 0.24f), ScreenPosition::Anchor(), &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT), "acceleration(m/s):", 0.7f);
		for (int i = 0; i < 6; ++i)
		{
			m_modelInfoContainer->CreateScreenElement<TextRender>(Vec2(-0.86f, 0.08f - i * 0.16f), ScreenPosition::Anchor(), &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT), char(i + 0xCC) + std::string(":"), 0.7f);
			m_resourceCosts[i] = &m_modelInfoContainer->CreateScreenElement<TextRender>(Vec2(0.32f, 0.08f - i * 0.16f), ScreenPosition::Anchor(), &Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT), "00000000", 0.7f);
		}
	}
}