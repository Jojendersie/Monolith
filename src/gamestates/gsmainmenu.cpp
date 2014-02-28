#include "../game.hpp"
#include "gsmainmenu.hpp"
#include "gsplay.hpp"
#include "../graphic/device.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/hud.hpp"
using namespace Math;
#include <cassert>

// TODO: remove after test
#include <iostream>

// ************************************************************************* //
GSMainMenu::GSMainMenu(Monolith* _game) : IGameState(_game)
{
	m_hud = new Graphic::Hud(_game->m_graficContent, _game);
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
	
	m_hud->m_dbgLabel->SetText("<s 034>" + std::to_string(_deltaTime * 1000.0) + " ms</s>");
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
	std::cout << Input::KeyToChar(_key, _modifiers);

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;

	if( _key == GLFW_KEY_T )
		m_game->PushState( m_game->GetPlayState() );
}

// ************************************************************************* //
void GSMainMenu::KeyClick( int _key )
{
	std::cout << "c\n";
}

// ************************************************************************* //
void GSMainMenu::KeyDoubleClick( int _key )
{
	std::cout << "x\n";
}