#include "../game.hpp"
#include "gseditor.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
#include "../graphic/interface/hud.hpp"
#include "../graphic/marker/box.hpp"
#include "../voxel/model.hpp"
#include <cassert>

using namespace Math;

// ************************************************************************* //
GSEditor::GSEditor(Monolith* _game) : IGameState(_game),
	m_model( nullptr ),
	m_rayHits( false ),
	m_deletionMode( false ),
	m_lvl0Position( 0 ),
	m_currentType( Voxel::VoxelType::WATER )
{
	LOG_LVL2("Starting to create game state Editor");

	m_hud = new Graphic::Hud(_game->m_graficContent, _game);

	// TODO: viewport for this camera in the upper right corner
	m_modelCamera = new Input::Camera( Vec3( 0.0f, 0.0f, 0.0f ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		Graphic::Device::GetAspectRatio() );

	// Create boxes for change previews
	m_redBox = new Graphic::Marker::Box( Vec3( 1.0002f, 1.0002f, 1.0002f ), 0.55f, Utils::Color32F(0.9f, 0.1f, 0.1f, 1.0f), _game->m_graficContent );
	m_greenBox = new Graphic::Marker::Box( Vec3( 1.0002f, 1.0002f, 1.0002f ), 0.55f, Utils::Color32F(0.1f, 0.9f, 0.1f, 1.0f), _game->m_graficContent );

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
	delete m_model;
	m_model = nullptr;

	LOG_LVL2("Left game state Editor");
}

// ************************************************************************* //
void GSEditor::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSEditor::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	// Draw the model which is edited
	m_modelCamera->Set( m_game->m_graficContent->cameraUBO );
	Graphic::Device::SetEffect(	m_game->m_graficContent->voxelRenderEffect );
	m_model->Draw( m_game->m_graficContent->voxelObjectUBO, *m_modelCamera );
	// Draw the marker in the same view
	if( m_rayHits )
	{
		// Use model coordinate system
		Math::Mat4x4 modelTransform;
		m_model->GetModelMatrix(modelTransform);
		// Compute position of edited voxel.
		Math::Vec3 voxelPos = m_lvl0Position + 0.50001f;
		if( m_deletionMode || !m_validPosition )
			m_redBox->Draw( Math::Mat4x4::Translation( voxelPos ) * Math::Mat4x4::Scaling( 1.0f, 1.0f, 1.0f ) * modelTransform * m_modelCamera->GetViewProjection() );
		else 
			m_greenBox->Draw( Math::Mat4x4::Translation( voxelPos ) * Math::Mat4x4::Scaling( 1.0f, 1.0f, 1.0f ) * modelTransform * m_modelCamera->GetViewProjection() );
	}

	// Draw hud and components in another view
	m_hud->Draw(  _time, _deltaTime );
}

// ************************************************************************* //
void GSEditor::UpdateInput()
{
	m_modelCamera->UpdateMatrices();

	// Find out the position where the cursor points to.
	Ray ray = m_modelCamera->GetRay( Input::Manager::GetCursorPosScreenSpace() );
	Voxel::Model::ModelData::HitResult hit;
	m_rayHits = m_model->RayCast( ray, 0, hit );
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
void GSEditor::MouseMove( double _dx, double _dy )
{
	// Rotate the model and not the camera in editor view
	// Read config file for speed
	double rotSpeed = m_game->Config[std::string("Input")][std::string("CameraRotationSpeed")];
	if( Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::ROTATE_CAMERA) )
		m_model->Rotate( Quaternion( float(-_dy * rotSpeed), float(_dx * rotSpeed), 0.0f ) );

	m_hud->MouseMove(_dx, _dy);
}

// ************************************************************************* //
void GSEditor::Scroll( double _dx, double _dy )
{
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

	// DEBUG CODE TO TEST EDITING WITHOUT THE HUD
	if( _key >= GLFW_KEY_0 && _key <= GLFW_KEY_9 )
	{
		m_currentType = Voxel::VoxelType(_key - GLFW_KEY_0);
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
	if( _key == GLFW_MOUSE_BUTTON_1 && m_validPosition )
	{
		if( m_deletionMode )
		{
			// Delete
			m_model->Set( m_lvl0Position, 0, Voxel::VoxelType::UNDEFINED );
		} else {
			// Add a voxel of the chosen type
			m_model->Set( m_lvl0Position, 0, m_currentType );
		}
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
	assert( !m_model );

	if( _copyFrom )
	{
	// TODO copy construction
	//m_model = new Voxel::Model(*_copyFrom);
	} else {
		// Create empty model
		m_model = new Voxel::Model();

		// Insert the computer
		m_model->Set(IVec3(2048,2048,2048), 0, Voxel::VoxelType::ROCK_1 );
	}

	m_modelCamera->ZoomAt( *m_model );
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

	// TODO: Do not delete the last computer
}
