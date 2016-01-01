#pragma once

#include "gamestatebase.hpp"
#include "utilities/scopedpointer.hpp"

/// \brief Game state for the initial menu.
class GSMainMenu: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSMainMenu(Monolith* _game);
	~GSMainMenu();

	void OnBegin();
	virtual void OnEnd() override;

	virtual void Simulate( double _deltaTime ) override;
	virtual void Render( double _deltaTime ) override;
	virtual void MouseMove( double _dx, double _dy ) override;
	virtual void KeyDown( int _key, int _modifiers ) override;
	virtual void KeyRelease(int _key) override;
	virtual void KeyClick( int _key ) override;
	virtual void KeyDoubleClick( int _key ) override;
private:
	Graphic::Hud* m_hud;
	ScopedPtr<Galaxy> m_galaxy;		///< Galaxy in the background
};