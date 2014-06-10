#include "game.hpp"
#include "gsgraphicopt.hpp"
#include "math/math.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "../dependencies/glfw-3.0.3/include/GLFW/glfw3.h"
using namespace Math;
#include "utilities/assert.hpp"

// ************************************************************************* //
GSGraphicOpt::GSGraphicOpt(Monolith* _game) : IGameState(_game)
{
	LOG_LVL2("Starting to create game state GraphicOpt");

	m_hud = new Graphic::Hud(_game);

	LOG_LVL2("Created game state GraphicOpt");
}

// ************************************************************************* //
GSGraphicOpt::~GSGraphicOpt()
{
	delete m_hud;

	LOG_LVL2("Deleted game state GraphicOpt");
}

// ************************************************************************* //
void GSGraphicOpt::OnBegin()
{
	LOG_LVL2("Entered game state GraphicOpt");
}

// ************************************************************************* //
void GSGraphicOpt::OnEnd()
{
	LOG_LVL2("Left game state GraphicOpt");
}

// ************************************************************************* //
void GSGraphicOpt::Simulate( double _deltaTime )
{
}

// ************************************************************************* //
void GSGraphicOpt::Render( double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
}


// ************************************************************************* //
void GSGraphicOpt::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSGraphicOpt::KeyDown( int _key, int _modifiers )
{
	// hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSGraphicOpt::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSGraphicOpt::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSGraphicOpt::KeyDoubleClick( int _key )
{
}
