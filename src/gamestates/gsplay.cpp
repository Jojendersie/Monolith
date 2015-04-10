#include "game.hpp"
#include "gsplay.hpp"
#include "graphic/core/device.hpp"
#include "math/math.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/marker/grid.hpp"
#include "GLFW/glfw3.h"
using namespace Math;
using namespace Graphic;
#include "utilities/assert.hpp"
#include "generators/random.hpp"

// TODO: remove after test
#include "../generators/asteroid.hpp"
#include "../graphic/core/texture.hpp"
#include "../generators/random.hpp"
#include "../voxel/voxel.hpp"
#include "../graphic/content.hpp"
#include "../gameplay/galaxy.hpp"

namespace RenderStat {
	int g_numVoxels;
	int g_numChunks;
}


// ************************************************************************* //
GSPlay::GSPlay(Monolith* _game) : IGameState(_game)
{
	LOG_LVL2("Starting to create game state Play");

	m_hud = new Graphic::Hud(_game);
	
	m_camera = new Input::Camera( FixVec3( Fix(0.0), Fix(0.0), Fix(0.0) ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		Graphic::Device::GetAspectRatio() );

	m_objectPlane = new Graphic::Marker::Grid( 40, 40, 8.0f, Utils::Color32F( 0.5f, 0.5f, 1.0f, 0.5f ), true );
	//m_objectPlane = new Graphic::Marker::Grid( 30, 30, 30, 10.0f, Utils::Color32F( 0.5f, 0.5f, 1.0f, 0.5f ) );

	m_galaxy = new Galaxy(1000, 20000.f);

	LOG_LVL2("Created game state Play");
}

// ************************************************************************* //
GSPlay::~GSPlay()
{
	delete m_objectPlane;
	delete m_camera;
	delete m_hud;

	delete m_galaxy;

	LOG_LVL2("Deleted game state Play");
}

// ************************************************************************* //
void GSPlay::OnBegin()
{
	if( m_scene.NumActiveObjects() == 0 )
	{
		for( int i = 0; i < 25; ++i )
		{
			Generators::Random rnd(i*4+1);
			auto model = new Generators::Asteroid( rnd.Uniform(10, 30), rnd.Uniform(10, 30), rnd.Uniform(10, 30), i );
			Vec3 pos(rnd.Uniform(-150.0f, 150.0f), rnd.Uniform(-150.0f, 150.0f), rnd.Uniform(-150.0f, 150.0f));
			model->SetPosition( FixVec3(pos) );
			m_scene.AddObject(model);
			if(i==0) m_camera->ZoomAt( *model );
		}
	}

	LOG_LVL2("Entered game state Play");
}

// ************************************************************************* //
void GSPlay::OnEnd()
{
	LOG_LVL2("Left game state Play");
}

// ************************************************************************* //
void GSPlay::Simulate( double _deltaTime )
{
	m_scene.UpdateGraph();
	/*static Generators::Random Rnd(1435461);
	for( int i = 0; i < 100; ++i )
		m_astTest->Set( IVec3(Rnd.Uniform(0,79), Rnd.Uniform(0,49), Rnd.Uniform(0,29)), 0, Voxel::VoxelType::UNDEFINED );//*/
}

// ************************************************************************* //
void GSPlay::Render( double _deltaTime )
{
	//m_scene.UpdateGraph();
	m_camera->UpdateMatrices();

	RenderStat::g_numVoxels = 0;
	RenderStat::g_numChunks = 0;
	m_camera->Set( Resources::GetUBO(UniformBuffers::CAMERA) );

	Graphic::Device::Clear( 0.05f, 0.05f, 0.06f );

	m_galaxy->Draw(*m_camera);

	Graphic::Device::SetEffect(	Resources::GetEffect(Effects::VOXEL_RENDER) );

	Jo::HybridArray<SOHandle, 32> visibleObjects;
	m_scene.FrustumQuery(visibleObjects);
	for( unsigned i = 0; i < visibleObjects.Size(); ++i ) {
		Voxel::Model* model = dynamic_cast<Voxel::Model*>(&visibleObjects[i]);
		Assert(model != nullptr, "Thread error? Currently there are only models");
		if(model) model->Draw( *m_camera );
	}

	if( m_selectedObject )
	{
		Mat4x4 modelView;
		m_selectedObjectModPtr->GetModelMatrix( modelView, *m_camera );
		modelView = Mat4x4::Translation(m_selectedObjectModPtr->GetCenter()) * modelView;
		m_objectPlane->Draw( modelView * m_camera->GetProjection() );
	}
	
	m_hud->m_dbgLabel->SetText("<s 024>" + std::to_string(_deltaTime * 1000.0) + " ms\n#Vox: " + std::to_string(RenderStat::g_numVoxels) + "\n#Chunks: " + std::to_string(RenderStat::g_numChunks)+"</s>");
	m_hud->Draw( _deltaTime );
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
		m_selectedObject = m_scene.RayQuery(ray, hit);
		if( m_selectedObject ) {
			m_selectedObjectModPtr = dynamic_cast<Voxel::Model*>(&m_selectedObject);
			m_selectedObjectModPtr->Set( hit.position, 0, Voxel::VoxelType::UNDEFINED );
		}
	}
}