#include "../game.hpp"
#include "gsgameplayopt.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
using namespace Math;
#include <cassert>

// ************************************************************************* //
GSGameplayOpt::GSGameplayOpt(Monolith* _game) : IGameState(_game)
{
	LOG_LVL2("Starting to create game state GameplayOpt");

	m_hud = new Graphic::Hud(_game->m_graficContent, _game);

	LOG_LVL2("Created game state GameplayOpt");
}

// ************************************************************************* //
GSGameplayOpt::~GSGameplayOpt()
{
	delete m_hud;

	LOG_LVL2("Deleted game state GameplayOpt");
}

// ************************************************************************* //
void GSGameplayOpt::OnBegin()
{
	LOG_LVL2("Entered game state GameplayOpt");
}

// ************************************************************************* //
void GSGameplayOpt::OnEnd()
{
	LOG_LVL2("Left game state GameplayOpt");
}

// ************************************************************************* //
void GSGameplayOpt::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSGameplayOpt::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
}

// ************************************************************************* //
void GSGameplayOpt::UpdateInput()
{
}

// ************************************************************************* //
void GSGameplayOpt::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSGameplayOpt::KeyDown( int _key, int _modifiers )
{
	// hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSGameplayOpt::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSGameplayOpt::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSGameplayOpt::KeyDoubleClick( int _key )
{
}