#include "game.hpp"
#include "gsmainmenu.hpp"
#include "gsplay.hpp"
#include "gseditor.hpp"
#include "gameplay/galaxy.hpp"
#include "graphic/core/device.hpp"
#include "math/math.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "utilities/assert.hpp"
#include "resources.hpp"
#include "GLFW/glfw3.h"

using namespace ei;
using namespace Math;

// ************************************************************************* //
GSMainMenu::GSMainMenu(Monolith* _game) : IGameState(_game)
{
	LOG_LVL2("Starting to create game state MainMenu");

	m_hud = new Graphic::Hud(_game, nullptr);

	namespace G = Graphic;

	auto& btn0 = m_hud->CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f, -0.4f), Vec2(0.f),
		G::TopMid, G::ScreenOverlay::Anchor(G::TopMid, m_hud), Resources::GetText("continue"),
		[&]() { m_game->PushState(m_game->GetPlayState()); });
	btn0.Scale(Vec2(0.65f));


	auto& btn1 = m_hud->CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f, -0.02f), Vec2(0.f),
		G::TopLeft, G::ScreenOverlay::Anchor(G::BotLeft, &btn0), Resources::GetText("editor"),
		[&]() { m_game->PushState(m_game->GetEditorState());});
	btn1.Scale(Vec2(0.65f));

	auto& btn2 = m_hud->CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f, -0.02f), Vec2(0.f),
		G::TopLeft, G::ScreenOverlay::Anchor(G::BotLeft, &btn1), Resources::GetText("options"),
		[&]() { m_finished = true; });
	btn2.Scale(Vec2(0.65f));

	auto& btn3 = m_hud->CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f, -0.02f), Vec2(0.f),
		G::TopLeft, G::ScreenOverlay::Anchor(G::BotLeft, &btn2), Resources::GetText("end"),
		[&]() { m_finished = true; });
	btn3.Scale(Vec2(0.65f));
//	m_hud->CreateBtn("menuBtn", Resources::GetText("continue"), Vec2(-0.25f,0.4f), Vec2(0.5f, 0.15f), [&] () { m_game->PushState( m_game->GetPlayState() ); });
//	m_hud->CreateBtn("menuBtn", Resources::GetText("editor"), Vec2(-0.25f, 0.22f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&]() { m_game->PushState(m_game->GetEditorState()); });
//	m_hud->CreateBtn("menuBtn", Resources::GetText("options"), Vec2(-0.25f, 0.04f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&]() { m_finished = true; });
//	m_hud->CreateBtn("menuBtn", Resources::GetText("end"), Vec2(-0.25f, -0.14f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&]() { m_finished = true; });

	m_galaxy = new Galaxy(10000, 2000.f, 10000);

	LOG_LVL2("Created game state MainMenu");
}

// ************************************************************************* //
GSMainMenu::~GSMainMenu()
{
	delete m_hud;

	LOG_LVL2("Deleted game state MainMenu");
}

// ************************************************************************* //
void GSMainMenu::OnBegin()
{
	LOG_LVL2("Entered game state MainMenu");
}

// ************************************************************************* //
void GSMainMenu::OnEnd()
{
	LOG_LVL2("Left game state MainMenu");
}


// ************************************************************************* //
void GSMainMenu::Simulate( double _deltaTime )
{
}

// ************************************************************************* //
void GSMainMenu::Render( double _deltaTime )
{
	Graphic::Device::Clear( 0.05f, 0.05f, 0.06f );

	const float galaxyZoom = -1500.0f;
	float galaxyRotation = Monolith::Time() * 0.01f;
	Input::Camera galaxyCam( FixVec3(Fix(galaxyZoom*sin(galaxyRotation)), Fix(1000.0), Fix(galaxyZoom*cos(galaxyRotation))),
		Quaternion(-0.4f, 0.0f, 0.0f) * Quaternion(0.0f, -galaxyRotation, 0.0f),
		0.55f, Graphic::Device::GetAspectRatio() );
	galaxyCam.UpdateMatrices();
	galaxyCam.Set( Graphic::Resources::GetUBO(Graphic::UniformBuffers::CAMERA) );
	m_galaxy->Draw(galaxyCam);
	
	m_hud->m_dbgLabel->SetText("<t 026> <s 024>" + std::to_string(_deltaTime * 1000.0) + " ms</s></t>");
	m_hud->Draw( _deltaTime );
}


// ************************************************************************* //
void GSMainMenu::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSMainMenu::KeyDown( int _key, int _modifiers )
{
//	std::cout << Input::KeyToChar(_key, _modifiers);

	//hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;

	if( _key == GLFW_KEY_T )
		m_game->PushState( m_game->GetPlayState() );
}

// ************************************************************************* //
void GSMainMenu::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSMainMenu::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSMainMenu::KeyDoubleClick( int _key )
{
}
