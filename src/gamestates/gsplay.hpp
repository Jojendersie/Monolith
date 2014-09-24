#pragma once

#include "gamestatebase.hpp"

#include "../physics/universe.hpp"

/// \brief State for the main phase: in game.
class GSPlay: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSPlay(Monolith* _game);
	~GSPlay();

	void OnBegin();
	virtual void OnEnd() override;

	virtual void Simulate( double _deltaTime ) override;
	virtual void Render( double _deltaTime ) override;
	virtual void MouseMove( double _dx, double _dy ) override;
	virtual void Scroll( double _dx, double _dy ) override;
	virtual void KeyDown( int _key, int _modifiers ) override;
	virtual void KeyClick( int _key ) override;
private:
	Physics::Universe* m_universe;
	Generators::Asteroid* m_center;
	Graphic::Marker::Grid* m_objectPlane;
	Graphic::Hud* m_hud;
	Input::Camera* m_camera;

	/// \brief Subroutine of the rendering: Showing the navigation grid.
	void DrawReferenceGrid() const;
};