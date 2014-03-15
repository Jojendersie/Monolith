#include "../game.hpp"
#include "gseditorchoice.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
using namespace Math;
#include <cassert>

// ************************************************************************* //
GSEditorChoice::GSEditorChoice(Monolith* _game) : IGameState(_game)
{
	m_hud = new Graphic::Hud(_game->m_graficContent, _game);
}

// ************************************************************************* //
GSEditorChoice::~GSEditorChoice()
{
	delete m_hud;
}

// ************************************************************************* //
void GSEditorChoice::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSEditorChoice::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
}

// ************************************************************************* //
void GSEditorChoice::UpdateInput()
{
}

// ************************************************************************* //
void GSEditorChoice::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSEditorChoice::KeyDown( int _key, int _modifiers )
{
	// hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSEditorChoice::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSEditorChoice::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSEditorChoice::KeyDoubleClick( int _key )
{
}
