#include "game.hpp"
#include "gsmainmenu.hpp"
#include "gsplay.hpp"
#include "gseditor.hpp"
#include "graphic/core/device.hpp"
#include "math/math.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "utilities/assert.hpp"
#include "resources.hpp"
#include "GLFW/glfw3.h"

using namespace ei;

// ************************************************************************* //
GSMainMenu::GSMainMenu(Monolith* _game) : IGameState(_game)
{
	LOG_LVL2("Starting to create game state MainMenu");

	m_hud = new Graphic::Hud(_game);
	//some bsp buttons
	m_hud->CreateBtn("menuBtn", Resources::GetText("continue"), Vec2(-0.25f,0.4f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&] () { m_game->PushState( m_game->GetPlayState() ); });
	m_hud->CreateBtn("menuBtn", Resources::GetText("editor"), Vec2(-0.25f, 0.22f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&]() { m_game->PushState(m_game->GetEditorState()); });
	m_hud->CreateBtn("menuBtn", Resources::GetText("options"), Vec2(-0.25f, 0.04f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&]() { m_finished = true; });
	m_hud->CreateBtn("menuBtn", Resources::GetText("end"), Vec2(-0.25f, -0.14f), Vec2(0.5f, 0.15f), Graphic::RealDimension::width, [&]() { m_finished = true; });

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
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
	
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
