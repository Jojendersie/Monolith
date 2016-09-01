#pragma once

#include "gamestatebase.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/managment/playercontroller.hpp"
#include "gameplay/firemanager.hpp"

#include <memory>

namespace Graphic{
	class HudGsPlay;
}
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
	Graphic::Marker::Grid* m_objectPlane;
	Graphic::HudGsPlay* m_hud;
	Galaxy* m_galaxy;
	Input::Camera* m_camera;
	SceneGraph m_scene;
	FireManager m_fireManager;
	std::unique_ptr<PlayerController> m_player;

	SOHandle m_selectedObject;
	Voxel::Model* m_selectedObjectModPtr;

	/// \brief Subroutine of the rendering: Showing the navigation grid.
	void DrawReferenceGrid(const Voxel::Model* _model) const;
};