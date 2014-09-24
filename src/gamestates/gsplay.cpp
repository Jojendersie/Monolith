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

// TODO: remove after test
#include "../physics/universe.hpp"
#include <vector>
#include <sstream>

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
GSPlay::GSPlay(Monolith* _game) : IGameState(_game), m_center(nullptr)
{
	LOG_LVL2("Starting to create game state Play");

	m_hud = new Graphic::Hud(_game);

	m_camera = new Input::Camera( FixVec3( Fix(0.0), Fix(0.0), Fix(0.0) ),
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
	//delete m_astTest;
	delete m_hud;

	LOG_LVL2("Deleted game state Play");
}

// ************************************************************************* //
void GSPlay::OnBegin()
{
	if( !m_universe )
	{
		m_universe = new Physics::Universe();
		m_center = new Generators::Asteroid(70, 80, 90, 0);
		m_center->SetPosition(FixVec3(Fix(0.)));
		m_center->SetAngularVelocity(Quaternion(Vec3(0.f, 1.f, 0.f), 0.1f));
		m_universe->AddCelestial(m_center);
		//center->SetPosition(FixVec3());
		for (int i = 0; i < 3; ++i){
			Voxel::Model *asteroid= new Generators::Asteroid(30, 30, 30, i);
			asteroid->SetPosition(FixVec3(Fix(50 + 40. * i), Fix(10.*i), Fix(0.)));
			asteroid->SetVelocity(Vec3(10.f-i));
			Vec3 axis = Vec3(0.f, 1.f, i*1.f);
			asteroid->SetAngularVelocity(Quaternion(axis / length(axis), 0.1f));
			m_universe->AddModel(asteroid);
		}
	}
	m_camera->ZoomAt( *m_center );

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
	m_universe->Update(_deltaTime);
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

	Graphic::Device::SetEffect(	Resources::GetEffect(Effects::VOXEL_RENDER) );
	
	std::vector<Physics::IntersectionIdentifier> models=m_universe->getModels();
	for (auto mIter = models.begin(); mIter != models.end(); ++mIter){
		mIter->Model()->Draw(*m_camera);
	}
	//m_astTest->Draw( *m_camera );

	DrawReferenceGrid();
	
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
	if (_key == GLFW_KEY_A){
		m_universe->m_GravConst = m_universe->m_GravConst * 2;
		std::ostringstream ss;
		ss << "\n Gravity set to " << m_universe->m_GravConst;
		LOG_LVL2(ss.str());
	}
	if (_key == GLFW_KEY_Y){
		m_universe->m_GravConst = m_universe->m_GravConst / 2;
		std::ostringstream ss;
		ss << "\n Gravity set to " << m_universe->m_GravConst;
		LOG_LVL2(ss.str());
	}
	if (_key == GLFW_KEY_ESCAPE)
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
		if( m_center->RayCast(ray, 0, hit) )
			m_center->Set( hit.position, 0, Voxel::VoxelType::UNDEFINED );
	}
}

// ************************************************************************* //
void GSPlay::DrawReferenceGrid() const
{
	Mat4x4 modelView;
	Vec3 position;
	auto model = m_camera->GetAttachedModel();
	if( model )
	{
		// Compute relative to the model position
		position = m_camera->GetReferencePosition();
	} else {
		// Compute relative to the camera
		position[0] = position[2] = 0.0;
		position[1] = float(m_camera->RenderState().GetPosition()[1]);
		// TODO: toggle the plane one on and off
	}
	modelView = Mat4x4::Translation(position) * Mat4x4(m_camera->RenderState().GetRotation());
	m_objectPlane->Draw( modelView * m_camera->GetProjection() );
}