#include "../game.hpp"
#include "gseditor.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
using namespace Math;
#include <cassert>

// ************************************************************************* //
GSEditor::GSEditor(Monolith* _game) : IGameState(_game)
{
	m_hud = new Graphic::Hud(_game->m_graficContent, _game);
}

// ************************************************************************* //
GSEditor::~GSEditor()
{
	delete m_hud;
}

// ************************************************************************* //
void GSEditor::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSEditor::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );
}

// ************************************************************************* //
void GSEditor::UpdateInput()
{
}

// ************************************************************************* //
void GSEditor::MouseMove( double _dx, double _dy )
{
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSEditor::KeyDown( int _key, int _modifiers )
{
	// hud overides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSEditor::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSEditor::KeyClick( int _key )
{
}

// ************************************************************************* //
void GSEditor::KeyDoubleClick( int _key )
{
}
