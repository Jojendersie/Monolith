#include "gseditorhud.hpp"


using namespace ei;

namespace Graphic
{
	HudGsEditor::HudGsEditor(Monolith* _game) :
		Hud(_game)
	{
		m_nameEdit = &CreateEditField(Vec2(-0.98f, 0.94f), Vec2(0.66f, 0.1f), 1, 0.f);

		CreateScreenTexture(Vec2(-0.995f, 0.825f), Vec2(0.48f, 0.2f), "frame");
		CreateScreenTexture(Vec2(-0.995f, -0.725f), Vec2(0.48f, 0.2f), "frame180");

		m_voxelContainer = CreateContainer(Vec2(-0.98f, -0.9f), Vec2(0.5f, 1.7f));//Math::Vec2(0.6f,1.75f));
		m_voxelContainer->SetScrollable(true);

		
		//box holding informations about the current model
		m_modelInfoContainer = CreateContainer(Vec2(0.55f, -0.9f), Vec2(0.4f, 0.6f));

		m_modelInfoContainer->CreateScreenTexture(Vec2(-1.f, 1.f), Vec2(2.f, 2.f), "simpleWindow");


		m_modelInfoContainer->CreateLabel(Vec2(-0.86f, 0.4f), "mass(kg):", 0.7f, Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));
		m_mass = &m_modelInfoContainer->CreateLabel(Vec2(0.32f, 0.4f), "", 0.7f, Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));

		m_modelInfoContainer->CreateLabel(Vec2(-0.86f, 0.24f), "acceleration(m/s):", 0.7f, Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));
		for (int i = 0; i < 6; ++i)
		{
			m_modelInfoContainer->CreateLabel(Vec2(-0.86f, 0.08f - i * 0.16f), char(i + 0xCC) + std::string(":"), 0.7f, Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));
			m_resourceCosts[i] = &m_modelInfoContainer->CreateLabel(Vec2(0.32f, 0.08f - i * 0.16f), "00000000", 0.7f, Graphic::Resources::GetFont(Graphic::Fonts::GAME_FONT));
		}
	//	m_modelInfoContainer->CreateEditField(Vec2(-0.98f, 0.64f), Vec2(0.66f, 0.1f), 1, 0.f);
	}
}