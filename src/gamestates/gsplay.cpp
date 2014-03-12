#include "../game.hpp"
#include "gsplay.hpp"
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
#include "../generators/random.hpp"
#include "../voxel/voxel.hpp"

namespace RenderStat {
	int g_numVoxels;
	int g_numChunks;
}

// ************************************************************************* //
GSPlay::GSPlay(Monolith* _game) : IGameState(_game), m_astTest(nullptr)
{
	m_hud = new Graphic::Hud(_game->m_graficContent, _game);

	m_camera = new Input::Camera( Vec3( 0.0f, 0.0f, 0.0f ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		Graphic::Device::GetAspectRatio() );
}

// ************************************************************************* //
GSPlay::~GSPlay()
{
	delete m_camera;
	delete m_astTest;
	delete m_hud;
}

// ************************************************************************* //
void GSPlay::OnBegin()
{
	if( !m_astTest )
		m_astTest = new Generators::Asteroid( 80, 50, 30, 2 );
	m_camera->ZoomAt( *m_astTest );
}

// ************************************************************************* //
void GSPlay::Update( double _time, double _deltaTime )
{
	/*static Generators::Random Rnd(1435461);
	for( int i = 0; i < 100; ++i )
		m_astTest->Set( IVec3(Rnd.Uniform(0,79), Rnd.Uniform(0,49), Rnd.Uniform(0,29)), 0, Voxel::VoxelType::UNDEFINED );//*/
}

// ************************************************************************* //
void GSPlay::Render( double _time, double _deltaTime )
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
	m_game->m_graficContent->cameraUBO["Position"] = m_camera->GetPosition();
//	m_parent->m_graficContent->cameraUBO["Far"] = 400.0f;

	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	Graphic::Device::SetEffect(	m_game->m_graficContent->voxelRenderEffect );

	m_astTest->Draw( m_game->m_graficContent->objectUBO, *m_camera );
	
	m_hud->m_dbgLabel->SetText("<s 034>" + std::to_string(_deltaTime * 1000.0) + " ms\n#Vox: " + std::to_string(RenderStat::g_numVoxels) + "\n#Chunks: " + std::to_string(RenderStat::g_numChunks)+"</s>");
	m_hud->Draw(  _time, _deltaTime );
}

// ************************************************************************* //
void GSPlay::UpdateInput()
{
	m_camera->UpdateMatrices();
}

// ************************************************************************* //
void GSPlay::MouseMove( double _dx, double _dy )
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
void GSPlay::Scroll( double _dx, double _dy )
{
	double scrollSpeed = m_game->Config[std::string("Input")][std::string("CameraScrollSpeed")];
	m_camera->Scroll( float(_dy * scrollSpeed) );
}

// ************************************************************************* //
void GSPlay::KeyDown( int _key, int _modifiers )
{
	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;
}

// ************************************************************************* //
void GSPlay::KeyClick( int _key )
{
	if( _key == GLFW_MOUSE_BUTTON_1 )
	{
		// Do a ray cast and delete the clicked voxel
		Ray ray = m_camera->GetRay( Input::Manager::GetCursorPosScreenSpace() );
		Voxel::Model::ModelData::HitResult hit;
		if( m_astTest->RayCast(ray, 0, hit) )
			m_astTest->Set( hit.position, 0, Voxel::VoxelType::UNDEFINED );
	}
}