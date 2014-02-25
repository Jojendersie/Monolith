#include "../game.hpp"
#include "gsmainmenu.hpp"
#include "../graphic/device.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/hud.hpp"
using namespace Math;
#include <cassert>

// TODO: remove after test
#include "../generators/asteroid.hpp"
#include "../graphic/texture.hpp"
#include <iostream>

namespace RenderStat {
	int g_numVoxels;
	int g_numChunks;
}

// ************************************************************************* //
GSMainMenu::GSMainMenu(Monolith* _game) : IGameState(_game)
{
	m_astTest = new Generators::Asteroid( 64, 48, 32, 2 );
	//m_astTest->ComputeVertexBuffer();
	//m_astTest->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );

	m_hud = new Graphic::Hud(_game->m_graficContent, _game);

	m_textures = new Graphic::Texture("texture/rock1.png");

	m_camera = new Input::Camera( Vec3( 0.0f, 0.0f, 0.0f ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		1.3f );	// TODO: compute aspect
	m_camera->ZoomAt( *m_astTest );
}

// ************************************************************************* //
GSMainMenu::~GSMainMenu()
{
	delete m_camera;
	delete m_astTest;
	delete m_hud;
	delete m_textures;
}

// ************************************************************************* //
void GSMainMenu::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSMainMenu::Render( double _time, double _deltaTime )
{
	RenderStat::g_numVoxels = 0;
	RenderStat::g_numChunks = 0;
//	_time = 0.1 * cos(_time*0.5);
	//Matrix view = MatrixCamera( Vec3(sin(_time)*250,80.0f,cos(_time)*250), Vec3(0.0f,0.0f,0.0f) );
	//Matrix projection = MatrixProjection( 0.3f, 1.3f, 0.5f, 400.0f );
	//Matrix viewProjection = view * projection;
	m_game->m_graficContent->cameraUBO["View"] = m_camera->GetView();
	m_game->m_graficContent->cameraUBO["Projection"] = m_camera->GetProjection();
	m_game->m_graficContent->cameraUBO["ViewProjection"] = m_camera->GetViewProjection();
	m_game->m_graficContent->cameraUBO["ProjectionInverse"] = Vec4(1.0f/m_camera->GetProjection()(0,0), 1.0f/m_camera->GetProjection()(1,1), 1.0f/m_camera->GetProjection()(2,2), -m_camera->GetProjection()(3,2) / m_camera->GetProjection()(2,2));
//	m_parent->m_graficContent->cameraUBO["Far"] = 400.0f;

	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	Graphic::Device::SetEffect(	m_game->m_graficContent->voxelRenderEffect );
	Graphic::Device::SetTexture( *m_game->m_graficContent->voxelTextures, 0 );

	m_astTest->Draw( m_game->m_graficContent->objectUBO, *m_camera );
	
	m_hud->m_dbgLabel->SetText("<s 034>" + std::to_string(_deltaTime * 1000.0) + " ms\n#Vox: " + std::to_string(RenderStat::g_numVoxels) + "\n#Chunks: " + std::to_string(RenderStat::g_numChunks)+"</s>");
	m_hud->Draw(  _time, _deltaTime );
}

// ************************************************************************* //
void GSMainMenu::UpdateInput()
{
	m_camera->UpdateMatrices();
}

// ************************************************************************* //
void GSMainMenu::MouseMove( double _dx, double _dy )
{
	// Read config file for speed
	double rotSpeed = m_game->Config[std::string("Input")][std::string("CameraRotationSpeed")];
	double moveSpeed = m_game->Config[std::string("Input")][std::string("CameraMovementSpeed")];
	if( Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::ROTATE_CAMERA) )
		m_camera->Rotate( float(_dy * rotSpeed), float(_dx * rotSpeed) );
	else if( Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::MOVE_CAMERA) )
		m_camera->Move( float(_dx * moveSpeed), float(_dy * moveSpeed) );
	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSMainMenu::Scroll( double _dx, double _dy )
{
	double scrollSpeed = m_game->Config[std::string("Input")][std::string("CameraScrollSpeed")];
	m_camera->Scroll( float(_dy * scrollSpeed) );
}

// ************************************************************************* //
void GSMainMenu::KeyDown( int _key, int _modifiers )
{
	std::cout << Input::KeyToChar(_key, _modifiers);

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSMainMenu::KeyClick( int _key )
{
	std::cout << "c\n";
}

// ************************************************************************* //
void GSMainMenu::KeyDoubleClick( int _key )
{
	std::cout << "x\n";
}