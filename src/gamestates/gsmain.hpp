#pragma once

#include "gamestatebase.hpp"

/// \brief Game state for the game itself.
class GSMain: public IGameState
{
public:
	/// \brief Create main state specific content.
	GSMain(Monolith* _parent);
	~GSMain();

	virtual void Update( double _time, double _deltaTime ) override;
	virtual void Render( double _time, double _deltaTime ) override;
	virtual void UpdateInput() override;
	virtual void MouseMove( double _dx, double _dy ) override;
	virtual void Scroll( double _dx, double _dy ) override;
private:
	Generators::Asteroid* m_astTest;
	Graphic::Font* m_fontTest;
	Graphic::TextRender* m_textTest;
	Graphic::Texture* m_textures;
	Input::Camera* m_camera;
};