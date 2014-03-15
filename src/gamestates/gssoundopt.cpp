#include "../game.hpp"
#include "gssoundopt.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
using namespace Math;
#include <cassert>

// ************************************************************************* //
GSSoundOpt::GSSoundOpt(Monolith* _game) : IGameState(_game)
{
	m_hud = new Graphic::Hud(_game->m_graficContent, _game);
}

// ************************************************************************* //
GSSoundOpt::~GSSoundOpt()
{
	delete m_hud;
}

// ************************************************************************* //
void GSSoundOpt::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSSoundOpt::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
}

// ************************************************************************* //
void GSSoundOpt::UpdateInput()
{
}

// ************************************************************************* //
void GSSoundOpt::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSSoundOpt::KeyDown( int _key, int _modifiers )
{
	// hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSSoundOpt::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSSoundOpt::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSSoundOpt::KeyDoubleClick( int _key )
{
}
