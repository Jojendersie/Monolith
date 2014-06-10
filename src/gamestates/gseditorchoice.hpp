#pragma once

#include "gamestatebase.hpp"

/// \brief Game state to choose a model from a list or to create new models.
class GSEditorChoice: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSEditorChoice(Monolith* _game);
	~GSEditorChoice();

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
};