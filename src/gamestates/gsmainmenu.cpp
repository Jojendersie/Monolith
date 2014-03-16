#include "../game.hpp"
#include "gsmainmenu.hpp"
#include "gsplay.hpp"
#include "../graphic/core/device.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
using namespace Math;
#include <cassert>

// ************************************************************************* //
GSMainMenu::GSMainMenu(Monolith* _game) : IGameState(_game)
{
	m_hud = new Graphic::Hud(_game->m_graficContent, _game);
	//some bsp buttons
	m_hud->CreateBtn("menuBtn", "continue", Math::Vec2(-0.25f,0.4f), Math::Vec2(0.6f, 0.15f), [&] () { m_game->PushState( m_game->GetPlayState() ); });
	m_hud->CreateBtn("menuBtn", "options", Math::Vec2(-0.25f,0.22f), Math::Vec2(0.6f, 0.15f));
	m_hud->CreateBtn("menuBtn", "end", Math::Vec2(-0.25f,0.04f), Math::Vec2(0.6f, 0.15f), [&] () { m_finished = true; });
}

// ************************************************************************* //
GSMainMenu::~GSMainMenu()
{
	delete m_hud;
}

// ************************************************************************* //
void GSMainMenu::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSMainMenu::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
	
	m_hud->m_dbgLabel->SetText("<t 026> <s 024>" + std::to_string(_deltaTime * 1000.0) + " ms</s></t>");
	m_hud->Draw(  _time, _deltaTime );
}

// ************************************************************************* //
void GSMainMenu::UpdateInput()
{
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
