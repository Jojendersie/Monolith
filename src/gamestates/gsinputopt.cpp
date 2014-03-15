#include "../game.hpp"
#include "gsinputopt.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
using namespace Math;
#include <cassert>

// ************************************************************************* //
GSInputOpt::GSInputOpt(Monolith* _game) : IGameState(_game)
{
	LOG_LVL2("Starting to create game state InputOpt");

	m_hud = new Graphic::Hud(_game->m_graficContent, _game);

	LOG_LVL2("Created game state InputOpt");
}

// ************************************************************************* //
GSInputOpt::~GSInputOpt()
{
	delete m_hud;

	LOG_LVL2("Deleted game state InputOpt");
}

// ************************************************************************* //
void GSInputOpt::OnBegin()
{
	LOG_LVL2("Entered game state InputOpt");
}

// ************************************************************************* //
void GSInputOpt::OnEnd()
{
	LOG_LVL2("Left game state InputOpt");
}

// ************************************************************************* //
void GSInputOpt::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSInputOpt::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
}

// ************************************************************************* //
void GSInputOpt::UpdateInput()
{
}

// ************************************************************************* //
void GSInputOpt::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSInputOpt::KeyDown( int _key, int _modifiers )
{
	// hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSInputOpt::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSInputOpt::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSInputOpt::KeyDoubleClick( int _key )
{
}
