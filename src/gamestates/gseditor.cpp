#include "voxel/model.hpp"
#include "game.hpp"
#include "gseditor.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/marker/box.hpp"
#include "graphic/marker/sphericalfunciton.hpp"
#include "graphic/content.hpp"
#include "GLFW/glfw3.h"
#include "utilities/assert.hpp"
#include "math/sphericalfunction.hpp"


#include <jofilelib.hpp>

using namespace Math;

// THIS IS A TEST FUNCTION
static SphericalFunction g_superFunc( [](const Math::Vec3& _dir){ return abs(_dir[1]) * 3; } );

// ************************************************************************* //
GSEditor::GSEditor(Monolith* _game) : IGameState(_game),
	m_ship( nullptr ),
	m_rayHits( false ),
	m_deletionMode( false ),
	m_lvl0Position( 0 ),
	m_currentType( Voxel::ComponentType::WATER ),
	m_recreateThrustVis( false )
{
	LOG_LVL2("Starting to create game state Editor");

	m_hud = new Graphic::Hud(_game);

	Graphic::EditField& nameEdit = m_hud->CreateEditField(Vec2(-0.98f, 0.94f), Vec2(0.66f, 0.1f), 1, 0.f);

	Graphic::Hud* voxelContainer = m_hud->CreateContainer(Vec2(-0.98f,-1.0f), Vec2(0.66f,1.8f));//Math::Vec2(0.6f,1.75f));
	voxelContainer->SetScrollable(true);

	//add every (available) voxel to the list which fits the criteria
	for(int i = 0; i < Voxel::TypeInfo::GetNumVoxels()-1; i++)
	{
		Voxel::ComponentType type = (Voxel::ComponentType)(i+1);
		Voxel::Model* vox = new Voxel::Model;
		vox->Set(Math::IVec3(0,0,0), type);
		voxelContainer->CreateModel(Math::Vec2(-0.76f,0.9f-i*0.2f), Math::Vec2(0.f, 0.f), vox);
		voxelContainer->CreateBtn("voxelBtn", "<s 022>       "+Voxel::TypeInfo::GetName(type)
									+ " Ì:" + std::to_string(Voxel::TypeInfo::GetHydrogen(type))
									+ " \n      Í:" + std::to_string(Voxel::TypeInfo::GetCarbon(type))
									+ " Î:" + std::to_string(Voxel::TypeInfo::GetMetals(type))
									+ " Ï:" + std::to_string(Voxel::TypeInfo::GetRareEarthElements(type))
									+ " Ð:" + std::to_string(Voxel::TypeInfo::GetSemiconductors(type))
									+ " Ñ:" + std::to_string(Voxel::TypeInfo::GetHeisenbergium(type)), 
									Math::Vec2(-1.f,1.f-i*0.2f), Math::Vec2(1.8f, 0.2f), Graphic::no,
									[this,type](){m_currentType = type;}, false);
	}

	//box holding informations about the current model
	Graphic::Hud* modelInfoContainer = m_hud->CreateContainer(Math::Vec2(0.2f,-0.9f), Math::Vec2(0.8f,0.6f));

	modelInfoContainer->CreateBtn("menuBtn", "load", Vec2(-0.9f, 0.92f), Vec2(0.8f, 0.22f), Graphic::width, [&]()
	{
		//copy and past from quick load
		ScopedPtr<Ship> ship = new Ship;
		ship->Load(Jo::Files::HDDFile("savegames/" + nameEdit.GetText() + ".vmo"));
		{
			std::unique_lock<std::mutex> lock(m_criticalModelWork);
			// TODO: REquest for the old model
			m_deleteList.PushBack(std::move(m_ship));
			m_ship = std::move(ship);
			m_modelCamera->ZoomAt(*m_ship, Input::Camera::REFERENCE_ONLY);
			m_recreateThrustVis = true;
		}
	});

	modelInfoContainer->CreateBtn("menuBtn", "save", Vec2(0.0f, 0.92f), Vec2(0.8f, 0.22f), Graphic::width, [&]()
	{
		m_ship->Save(Jo::Files::HDDFile("savegames/" + nameEdit.GetText() + ".vmo", Jo::Files::HDDFile::OVERWRITE));
	});

	// TODO: view port for this camera in the upper right corner
	m_modelCamera = new Input::Camera( FixVec3( Fix(0.0), Fix(0.0), Fix(0.0) ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		Graphic::Device::GetAspectRatio() );

	voxelContainer->SetCamera(m_modelCamera);

	// Create boxes for change previews
	m_redBox = new Graphic::Marker::Box( Vec3( 1.0002f, 1.0002f, 1.0002f ), 0.55f, Utils::Color32F(0.9f, 0.1f, 0.1f, 1.0f) );
	m_greenBox = new Graphic::Marker::Box( Vec3( 1.0002f, 1.0002f, 1.0002f ), 0.55f, Utils::Color32F(0.1f, 0.9f, 0.1f, 1.0f) );
	
	LOG_LVL2("Created game state Editor");
}

// ************************************************************************* //
GSEditor::~GSEditor()
{
	delete m_hud;
	delete m_redBox;
	delete m_greenBox;
	delete m_modelCamera;

	LOG_LVL2("Deleted game state EditorChoice");
}

// ************************************************************************* //
void GSEditor::OnBegin()
{
	// TODO: do not always create the standard model: parameters: Model*, bool _copy
	CreateNewModel();

	LOG_LVL2("Entered game state Editor");
}

// ************************************************************************* //
void GSEditor::OnEnd()
{
	// TODO: User store request
	m_ship = nullptr;

	LOG_LVL2("Left game state Editor");
}

// ************************************************************************* //
void GSEditor::Simulate( double _deltaTime )
{
	// Find out the position where the cursor points to.
	WorldRay ray = m_modelCamera->GetRay( Input::Manager::GetCursorPosScreenSpace() );
	Voxel::Model::ModelData::HitResult hit;
	float maxRange = 1e10f;
	m_rayHits = m_ship->RayCast( ray, 0, hit, maxRange );
	if( m_rayHits )
	{
		m_lvl0Position = hit.position;
		m_deletionMode = Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::EDITOR_DELETIONMODE);
		if(!m_deletionMode)
		{
			// Use offsetting in direction of the hit side when adding voxels.
			switch( hit.side )
			{
			case Intersect::Side::LEFT: m_lvl0Position[0] -= 1; break;
			case Intersect::Side::RIGHT: m_lvl0Position[0] += 1; break;
			case Intersect::Side::BOTTOM: m_lvl0Position[1] -= 1; break;
			case Intersect::Side::TOP: m_lvl0Position[1] += 1; break;
			case Intersect::Side::FRONT: m_lvl0Position[2] -= 1; break;
			case Intersect::Side::BACK: m_lvl0Position[2] += 1; break;
			}
		}

		ValidatePosition();
	}
}

// ************************************************************************* //
void GSEditor::Render( double _deltaTime )
{
	m_modelCamera->UpdateMatrices();

	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	// The model's transformation matrix to render additional information
	Mat4x4 modelViewProjection;

	// Draw the model which is edited
	if( m_criticalModelWork.try_lock() )
	{
		m_modelCamera->Set( Graphic::Resources::GetUBO(Graphic::UniformBuffers::CAMERA) );
		Graphic::Device::SetEffect(	Graphic::Resources::GetEffect(Graphic::Effects::VOXEL_RENDER) );
		m_ship->Draw( *m_modelCamera );
		m_ship->GetModelMatrix( modelViewProjection, *m_modelCamera );
		//modelViewProjection = Mat4x4::Translation(m_ship->GetModel().GetCenter()) * modelViewProjection;
		modelViewProjection *= m_modelCamera->GetProjection();

		// Draw the thrust function
		if( m_recreateThrustVis ) { m_thrustFunction = new Graphic::Marker::SphericalFunction( g_superFunc ); m_recreateThrustVis = false; }
		m_thrustFunction->Draw( Mat4x4::Translation(m_ship->GetCenter()) * modelViewProjection );

		m_deleteList.Clear();
		m_criticalModelWork.unlock();
	}

	// Draw the marker in the same view
	if( m_rayHits )
	{
		// Use model coordinate system
		// Compute position of edited voxel.
		Math::Vec3 voxelPos = m_lvl0Position + 0.50001f;
		if( m_deletionMode || !m_validPosition )
			m_redBox->Draw( Math::Mat4x4::Translation( voxelPos ) * Math::Mat4x4::Scaling( 1.0f, 1.0f, 1.0f ) * modelViewProjection );
		else 
			m_greenBox->Draw( Math::Mat4x4::Translation( voxelPos ) * Math::Mat4x4::Scaling( 1.0f, 1.0f, 1.0f ) * modelViewProjection );
	}

	// Draw hud and components in another view
	m_hud->Draw( _deltaTime );

/*	Graphic::Device::SetEffect(	*Graphic::Resources::GetEffect(Graphic::Effects::VOXEL_RENDER) );
	for(int i = 0; i < Voxel::TypeInfo::GetNumVoxels()-1; i++)
	{
		Math::Ray ray = m_modelCamera->GetRay(Math::Vec2(-0.887f,0.9f-i*0.2f));
		m_availableVoxels[i].SetCenter(ray.m_start+35.f*ray.m_direction);
		m_availableVoxels[i].Draw( *m_modelCamera, _time );
	}*/
}


// ************************************************************************* //
void GSEditor::MouseMove( double _dx, double _dy )
{
	// Rotate the model and not the camera in editor view
	// Read config file for speed
	double rotSpeed = m_game->Config[std::string("Input")][std::string("CameraRotationSpeed")];
	if( Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::ROTATE_CAMERA) )
		m_ship->Rotate( Quaternion( float(-_dy * rotSpeed), float(_dx * rotSpeed), 0.0f ) );

	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSEditor::Scroll( double _dx, double _dy )
{
	if(m_hud->Scroll(_dx, _dy)) return;
	double scrollSpeed = m_game->Config[std::string("Input")][std::string("CameraScrollSpeed")];
	m_modelCamera->Scroll( float(_dy * scrollSpeed) );
}

// ************************************************************************* //
void GSEditor::KeyDown( int _key, int _modifiers )
{
	// hud overrides input
	if(m_hud->KeyDown(_key, _modifiers)) return;

	if( _key == GLFW_KEY_ESCAPE )
		m_finished = true;

	if( Input::Manager::IsVirtualKey(_key, Input::VirtualKey::QUICK_SAVE) )
		m_ship->Save( Jo::Files::HDDFile( "savegames/test.vmo", Jo::Files::HDDFile::OVERWRITE ) );
	if( Input::Manager::IsVirtualKey(_key, Input::VirtualKey::QUICK_LOAD) )
	{
		ScopedPtr<Ship> ship = new Ship;
		ship->Load( Jo::Files::HDDFile( "savegames/test.vmo" ) );
		{
			std::unique_lock<std::mutex> lock(m_criticalModelWork);
			// TODO: REquest for the old model
			m_deleteList.PushBack( std::move(m_ship) );
			m_ship = std::move(ship);
			m_modelCamera->ZoomAt( *m_ship, Input::Camera::REFERENCE_ONLY );
			m_recreateThrustVis = true;
		}
	}

	// DEBUG CODE TO TEST EDITING WITHOUT THE HUD
	if( _key >= GLFW_KEY_0 && _key <= GLFW_KEY_9 )
	{
		m_currentType = Voxel::ComponentType(_key - GLFW_KEY_0);
	}
}

// ************************************************************************* //
void GSEditor::KeyRelease( int _key )
{
	if(m_hud->KeyUp(_key, 0)) return;
}

// ************************************************************************* //
void GSEditor::KeyClick( int _key )
{
	if( _key == GLFW_MOUSE_BUTTON_1 && m_validPosition && m_rayHits )
	{
		if( m_deletionMode )
		{
			// Delete
			m_ship->RemoveComponent( m_lvl0Position );
		} else {
			// Add a voxel of the chosen type
			m_ship->AddComponent( m_lvl0Position, m_currentType );
		}
		m_recreateThrustVis = true;
	}
}

// ************************************************************************* //
void GSEditor::KeyDoubleClick( int _key )
{
}

// ************************************************************************* //
void GSEditor::CreateNewModel( const Voxel::Model* _copyFrom )
{
	// Currently undefined situation: Delete/store model with a request.
	Assert( !m_ship, "Need to save model before creating a new one!" );

	if( _copyFrom )
	{
	// TODO copy construction
	//m_model = new Voxel::Model(*_copyFrom);
	} else {
		// Create empty model with computer
		m_ship = new Ship();

		m_recreateThrustVis = true;
	}

	m_modelCamera->ZoomAt( *m_ship, Input::Camera::REFERENCE_ONLY );
}

// ************************************************************************* //
void GSEditor::ValidatePosition()
{
	m_validPosition = true;

	// Check coordinates in general
	if( m_lvl0Position[0] < 0 || m_lvl0Position[1] < 0 || m_lvl0Position[2] < 0 )
		m_validPosition = false;
	if( m_lvl0Position[0] >= 4096 || m_lvl0Position[1] >= 4096 || m_lvl0Position[2] >= 4096 )
		m_validPosition = false;

	// Do not delete the last computer
	if( m_lvl0Position == m_ship->GetCentralComputerPosition() )
		m_validPosition = false;
}
