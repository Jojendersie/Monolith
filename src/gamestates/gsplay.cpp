#include "game.hpp"
#include "gsplay.hpp"
#include "graphic/core/device.hpp"
#include "math/math.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/marker/grid.hpp"
#include "../dependencies/glfw-3.0.3/include/GLFW/glfw3.h"
using namespace Math;
using namespace Graphic;
#include "utilities/assert.hpp"

// TODO: remove after test
#include "../generators/asteroid.hpp"
#include "../graphic/core/texture.hpp"
#include "../generators/random.hpp"
#include "../voxel/voxel.hpp"
#include "../graphic/content.hpp"

namespace RenderStat {
	int g_numVoxels;
	int g_numChunks;
}

// ************************************************************************* //
GSPlay::GSPlay(Monolith* _game) : IGameState(_game), m_astTest(nullptr)
{
	LOG_LVL2("Starting to create game state Play");

	m_hud = new Graphic::Hud(_game);

	m_camera = new Input::Camera( FixVec3( Fix(0ll), Fix(0ll), Fix(0ll) ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		Graphic::Device::GetAspectRatio() );

	m_objectPlane = new Graphic::Marker::Grid( 40, 40, 20.0f, Utils::Color32F( 0.5f, 0.5f, 1.0f, 0.5f ), true );
	//m_objectPlane = new Graphic::Marker::Grid( 30, 30, 30, 10.0f, Utils::Color32F( 0.5f, 0.5f, 1.0f, 0.5f ) );

	LOG_LVL2("Created game state Play");
}

// ************************************************************************* //
GSPlay::~GSPlay()
{
	delete m_objectPlane;
	delete m_camera;
	delete m_astTest;
	delete m_hud;

	LOG_LVL2("Deleted game state Play");
}

// ************************************************************************* //
void GSPlay::OnBegin()
{
	if( !m_astTest )
	{
		m_astTest = new Generators::Asteroid( 80, 50, 30, 2 );
		m_astTest->SetPosition( FixVec3(Fix(10000000.0)) );
	}
	m_camera->ZoomAt( *m_astTest );

	LOG_LVL2("Entered game state Play");
}

// ************************************************************************* //
void GSPlay::OnEnd()
{
	LOG_LVL2("Left game state Play");
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
	m_camera->Set( *Resources::GetUBO(UniformBuffers::CAMERA) );

	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	Graphic::Device::SetEffect(	*Resources::GetEffect(Effects::VOXEL_RENDER) );
	m_astTest->Draw( *m_camera, _time );

	Mat4x4 modelView;
	m_astTest->GetModelMatrix( modelView, *m_camera );
	modelView = Mat4x4::Translation(m_astTest->GetCenter()) * modelView;
	m_objectPlane->Draw( modelView * m_camera->GetProjection() );
	
	m_hud->m_dbgLabel->SetText("<s 024>" + std::to_string(_deltaTime * 1000.0) + " ms\n#Vox: " + std::to_string(RenderStat::g_numVoxels) + "\n#Chunks: " + std::to_string(RenderStat::g_numChunks)+"</s>");
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
		WorldRay ray = m_camera->GetRay( Input::Manager::GetCursorPosScreenSpace() );
		Voxel::Model::ModelData::HitResult hit;
		if( m_astTest->RayCast(ray, 0, hit) )
			m_astTest->Set( hit.position, 0, Voxel::VoxelType::UNDEFINED );
	}
}