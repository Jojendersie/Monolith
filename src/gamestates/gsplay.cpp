#include "game.hpp"
#include "gsplay.hpp"
#include "graphic/core/device.hpp"
#include "math/math.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/marker/grid.hpp"
#include "GLFW/glfw3.h"
#include "utilities/assert.hpp"
#include "generators/random.hpp"
#include "gameplay/ship.hpp"
using namespace Math;
using namespace Graphic;

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

	m_galaxy = new Galaxy(1000, 20000.f, 10000);

	m_player = std::make_unique<PlayerController>(nullptr);

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
		Ship* playerModel = new Ship();
		playerModel->Load(Jo::Files::HDDFile("savegames/playership.vmo"));
		//position to 0
		playerModel->SetPosition(FixVec3(Fix(0.f)));
		SOHandle shipHandle = m_scene.AddObject(playerModel);
		m_camera->ZoomAt(*playerModel, Input::Camera::FOLLOW_AND_ROTATE);
		m_player->Possess(shipHandle);

		for( int i = 0; i < 25; ++i )
		{
			Generators::Random rnd(i*4+1);
			auto model = new Generators::Asteroid( rnd.Uniform(10, 30), rnd.Uniform(10, 30), rnd.Uniform(10, 30), i );
			Vec3 pos(rnd.Uniform(-150.0f, 150.0f), rnd.Uniform(-150.0f, 150.0f), rnd.Uniform(-150.0f, 150.0f));
			model->SetPosition( FixVec3(pos) );
			model->Rotate(rnd.Uniform(-PI, PI), rnd.Uniform(-PI, PI), rnd.Uniform(-PI, PI));
			m_scene.AddObject(model);
		//	if(i==0) m_camera->ZoomAt( *model, Input::Camera::FOLLOW_AND_ROTATE );
		}
		/*Generators::Random rnd(484);
		auto model = new Generators::Asteroid( 200, 200, 200, 846 );
		model->SetPosition( FixVec3(Vec3(0.0f)) );
		model->Rotate(rnd.Uniform(-PI, PI), rnd.Uniform(-PI, PI), rnd.Uniform(-PI, PI));
		m_scene.AddObject(model);*/
		//m_camera->ZoomAt( *model );
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
	m_scene.Simulate((float)_deltaTime);
	m_scene.UpdateGraph();
	m_player->Process( (float)_deltaTime );
	/*static Generators::Random Rnd(1435461);
	for( int i = 0; i < 100; ++i )
		m_astTest->Set( IVec3(Rnd.Uniform(0,79), Rnd.Uniform(0,49), Rnd.Uniform(0,29)), 0, Voxel::VoxelType::UNDEFINED );//*/
}

// ************************************************************************* //
void GSPlay::Render( double _deltaTime )
{
	m_camera->UpdateMatrices();

	RenderStat::g_numVoxels = 0;
	RenderStat::g_numChunks = 0;
	m_camera->Set( Resources::GetUBO(UniformBuffers::CAMERA) );

	Graphic::Device::Clear( 0.05f, 0.05f, 0.06f );

	m_galaxy->Draw(*m_camera);

	Graphic::Device::SetEffect(	Resources::GetEffect(Effects::VOXEL_RENDER) );
	Voxel::TypeInfo::BindVoxelTextureArray();
	Jo::HybridArray<SOHandle, 32> visibleObjects;
	m_scene.FrustumQuery(visibleObjects);
	for( unsigned i = 0; i < visibleObjects.Size(); ++i ) {
		Voxel::Model* model = dynamic_cast<Voxel::Model*>(&visibleObjects[i]);
		Assert(model != nullptr, "Thread error? Currently there are only models");
		if(model) model->Draw( *m_camera );
	}

	if( m_selectedObject )
		DrawReferenceGrid( m_selectedObjectModPtr );
	
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
	/*else if( Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::MOVE_CAMERA) )
		m_camera->Move( float(_dx * moveSpeed), float(_dy * moveSpeed) );*/
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
	if (_key == GLFW_KEY_SPACE) {
		bool flyingMode = (1 == (m_hud->CursorVisible() - 1));
		m_hud->ShowCursor(flyingMode ? 1 : 2 );
		m_player->SetMouseRotation(!flyingMode);
	}
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
		//	m_selectedObjectModPtr->Set( hit.position, 0, Voxel::VoxelType::UNDEFINED );
		}
	}
}

// ************************************************************************* //
void GSPlay::DrawReferenceGrid(const Voxel::Model* _model) const
{
	Mat4x4 modelView;
	Vec3 position;
	if( _model )
	{
		if( _model == m_camera->GetAttachedModel() )
		{
			// Compute relative to the model position
			position = m_camera->GetReferencePosition();
			modelView =  Mat4x4::Rotation(_model->GetRotation()) * Mat4x4::Translation(position) * Mat4x4(m_camera->RenderState().GetRotation());
			//_model->GetModelMatrix( modelView, *m_camera );
		} else {
			position = _model->GetCenter();
			_model->GetModelMatrix( modelView, *m_camera );
			modelView = Mat4x4::Translation(position) * modelView;
		}
	} else {
		// Compute relative to the camera
		position[0] = 150.0f * sin(m_camera->GetYRotation());
		position[1] = -120.0f;
		position[2] = 150.0f * cos(m_camera->GetYRotation());
		modelView = Mat4x4::Translation(position)
			* Mat4x4::RotationY(-m_camera->GetYRotation())
			* Mat4x4::RotationX(-0.4f);
		// TODO: toggle the plane one on and off
	}
	m_objectPlane->Draw( modelView * m_camera->GetProjection() );
}