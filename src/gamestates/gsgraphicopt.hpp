#pragma once

#include "gamestatebase.hpp"

/// \brief Option menu state for graphics.
class GSGraphicOpt: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSGraphicOpt(Monolith* _game);
	~GSGraphicOpt();

	void OnBegin()					{}
	virtual void OnEnd() override	{}

	virtual void Update( double _time, double _deltaTime ) override;
	virtual void Render( double _time, double _deltaTime ) override;
	virtual void UpdateInput() override;
	virtual void MouseMove( double _dx, double _dy ) override;
	virtual void KeyDown( int _key, int _modifiers ) override;
	virtual void KeyRelease(int _key) override;
	virtual void KeyClick( int _key ) override;
	virtual void KeyDoubleClick( int _key ) override;
private:
	Graphic::Hud* m_hud;
};