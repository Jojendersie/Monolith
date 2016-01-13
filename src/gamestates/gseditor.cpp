#include "voxel/model.hpp"
#include "game.hpp"
#include "gseditor.hpp"
#include "input/camera.hpp"
#include "input/input.hpp"
#include "gseditorhud.hpp"
#include "graphic/marker/box.hpp"
#include "graphic/marker/sphericalfunciton.hpp"
#include "graphic/interface/singlecomponentrenderer.hpp"
#include "graphic/content.hpp"
#include "GLFW/glfw3.h"
#include "utilities/assert.hpp"
#include "math/sphericalfunction.hpp"
#include "utilities/stringutils.hpp"
#include "math/fixedpoint.hpp"
#include "math/ray.hpp"

#include <jofilelib.hpp>

using namespace ei;
using namespace Math;

// ************************************************************************* //
GSEditor::GSEditor(Monolith* _game) : IGameState(_game),
	m_ship( nullptr ),
	m_rayHits( false ),
	m_deletionMode( false ),
	m_lvl0Position( 0 ),
	m_currentComponent( Voxel::ComponentType::WATER ),
	m_recreateThrustVis( false )
{
	LOG_LVL2("Starting to create game state Editor");

	m_singleComponentRenderer = new Graphic::SingleComponentRenderer();
	m_hud = new Graphic::HudGsEditor(_game, m_singleComponentRenderer);

	//add every (available) voxel to the list which fits the criteria
	for (int i = 0; i < Voxel::TypeInfo::GetNumVoxels() - 2; i++)
	{
		Voxel::ComponentType type = (Voxel::ComponentType)(i + 2);
		m_hud->m_voxelContainer->CreateComponent(type, Vec2(-0.76f, 0.91f - i*0.2f), 0.158f);
		//head:лмнопя
		m_hud->m_voxelContainer->CreateBtn("componentBtn", "<s 018>        " + Voxel::TypeInfo::GetName(type)
			+ "\n       " + StringUtils::ToConstDigit(Voxel::TypeInfo::GetHydrogen(type), 2)
			+ " " + StringUtils::ToConstDigit(Voxel::TypeInfo::GetCarbon(type), 2)
			+ " " + StringUtils::ToConstDigit(Voxel::TypeInfo::GetMetals(type), 2)
			+ " " + StringUtils::ToConstDigit(Voxel::TypeInfo::GetRareEarthElements(type), 2)
			+ " " + StringUtils::ToConstDigit(Voxel::TypeInfo::GetSemiconductors(type), 2)
			+ " " + StringUtils::ToConstDigit(Voxel::TypeInfo::GetHeisenbergium(type), 2),
			Vec2(-1.f, 1.f - i*0.2f), Vec2(1.8f, 0.2f), Graphic::RealDimension::width,
			[this, type](){ m_currentComponent = Voxel::Voxel(type); }, false);
		/*		voxelContainer->CreateBtn("componentBtn", "<s 022>       "+Voxel::TypeInfo::GetName(type)
		+ " л:" + std::to_string(Voxel::TypeInfo::GetHydrogen(type))
		+ " \n      м:" + std::to_string(Voxel::TypeInfo::GetCarbon(type))
		+ " н:" + std::to_string(Voxel::TypeInfo::GetMetals(type))
		+ " о:" + std::to_string(Voxel::TypeInfo::GetRareEarthElements(type))
		+ " п:" + std::to_string(Voxel::TypeInfo::GetSemiconductors(type))
		+ " я:" + std::to_string(Voxel::TypeInfo::GetHeisenbergium(type)),
		Math::Vec2(-1.f,1.f-i*0.2f), Math::Vec2(1.8f, 0.2f), Graphic::no,
		[this,type](){m_currentType = type;}, false);*/
	}

	m_hud->m_modelInfoContainer->CreateBtn("menuBtn", "load", Vec2(-0.9f, 0.90f), Vec2(0.8f, 0.22f), Graphic::RealDimension::width, [&]()
	{
		//copy and past from quick load
		ScopedPtr<Ship> ship = new Ship;
		ship->Load(Jo::Files::HDDFile("savegames/" + m_hud->m_nameEdit->GetText() + ".vmo"));
		{
			std::unique_lock<std::mutex> lock(m_criticalModelWork);
			// TODO: REquest for the old model
			m_deleteList.PushBack(std::move(m_ship));
			m_ship = std::move(ship);
			m_modelCamera->ZoomAt(*m_ship, Input::Camera::REFERENCE_ONLY);
			m_recreateThrustVis = true;
		}
	});

	m_hud->m_modelInfoContainer->CreateBtn("menuBtn", "save", Vec2(0.0f, 0.90f), Vec2(0.8f, 0.22f), Graphic::RealDimension::width, [&]()
	{
		m_ship->Save(Jo::Files::HDDFile("savegames/" + m_hud->m_nameEdit->GetText() + ".vmo", Jo::Files::HDDFile::OVERWRITE));
	});

	// TODO: view port for this camera in the upper right corner
	m_modelCamera = new Input::Camera( FixVec3( Fix(0.0), Fix(0.0), Fix(0.0) ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		Graphic::Device::GetAspectRatio() );

	m_hud->m_voxelContainer->SetCamera(m_modelCamera);

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

	UpdateModelInformation();

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
			case HitSide::X_NEG: m_lvl0Position[0] -= 1; break;
			case HitSide::X_POS: m_lvl0Position[0] += 1; break;
			case HitSide::Y_NEG: m_lvl0Position[1] -= 1; break;
			case HitSide::Y_POS: m_lvl0Position[1] += 1; break;
			case HitSide::Z_NEG: m_lvl0Position[2] -= 1; break;
			case HitSide::Z_POS: m_lvl0Position[2] += 1; break;
			}
		}

		ValidatePosition();

		// Update the side-flags mask
		if( !m_deletionMode && m_validPosition )
		{
			// Set one for each side without an neighbor
			m_currentSideFlags  = Voxel::TypeInfo::IsSolid(m_ship->Get(m_lvl0Position + IVec3(0,0,1))) ? 0 : 0x20;
			m_currentSideFlags |= Voxel::TypeInfo::IsSolid(m_ship->Get(m_lvl0Position - IVec3(0,0,1))) ? 0 : 0x10;
			m_currentSideFlags |= Voxel::TypeInfo::IsSolid(m_ship->Get(m_lvl0Position + IVec3(0,1,0))) ? 0 : 0x08;
			m_currentSideFlags |= Voxel::TypeInfo::IsSolid(m_ship->Get(m_lvl0Position - IVec3(0,1,0))) ? 0 : 0x04;
			m_currentSideFlags |= Voxel::TypeInfo::IsSolid(m_ship->Get(m_lvl0Position + IVec3(1,0,0))) ? 0 : 0x02;
			m_currentSideFlags |= Voxel::TypeInfo::IsSolid(m_ship->Get(m_lvl0Position - IVec3(1,0,0))) ? 0 : 0x01;
		}
	}
}

// ************************************************************************* //
void GSEditor::Render( double _deltaTime )
{
	m_modelCamera->UpdateMatrices();

	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	// The model's transformation matrix to render additional information
	Mat4x4 modelView;

	// Draw the model which is edited
	if( m_criticalModelWork.try_lock() )
	{
		m_modelCamera->Set( Graphic::Resources::GetUBO(Graphic::UniformBuffers::CAMERA) );
		Graphic::Device::SetEffect(	Graphic::Resources::GetEffect(Graphic::Effects::VOXEL_RENDER) );
		m_ship->Draw( *m_modelCamera );
		m_ship->GetModelMatrix( modelView, *m_modelCamera );
		Mat4x4 modelViewProjection = m_modelCamera->GetProjection() * modelView;

		// Draw the thrust function
//		if( m_recreateThrustVis ) { m_thrustFunction = new Graphic::Marker::SphericalFunction( m_ship->DebugGet() ); m_recreateThrustVis = false; }
//		m_thrustFunction->Draw( modelViewProjection * translation(m_ship->GetCenter()) );

		m_deleteList.Clear();
		m_criticalModelWork.unlock();
	}

	// Draw the marker in the same view
	if( m_rayHits )
	{
		// Draw a ghost of the new component
		if( !m_deletionMode && m_validPosition )
		{
			Mat4x4 ghostTransform = modelView * scalingH( 1.0f ) * translation( m_lvl0Position );
			m_singleComponentRenderer->Draw( m_currentComponent, m_currentSideFlags, ghostTransform, m_modelCamera->GetProjection() );
		}
		// Use model coordinate system
		// Compute position of edited voxel.
		Vec3 voxelPos = m_lvl0Position + 0.50001f;
		Mat4x4 markerTransform = m_modelCamera->GetProjection() * modelView * scalingH( 1.0f ) * translation( voxelPos );
		if( m_deletionMode || !m_validPosition )
			m_redBox->Draw( markerTransform );
		else
			m_greenBox->Draw( markerTransform );
	}

	// Draw hud and components in another view
	m_hud->Draw( _deltaTime );
}


// ************************************************************************* //
void GSEditor::MouseMove( double _dx, double _dy )
{
	// Rotate the model and not the camera in editor view
	// Read config file for speed
	double rotSpeed = m_game->Config[std::string("Input")][std::string("CameraRotationSpeed")];
	if( Input::Manager::IsVirtualKeyPressed(Input::VirtualKey::ROTATE_CAMERA) )
		m_ship->Rotate( Quaternion( float(-_dy * rotSpeed), float(-_dx * rotSpeed), 0.0f ) );

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

	switch( _key )
	{
	case GLFW_KEY_ESCAPE:
		m_finished = true;
		break;
	case GLFW_KEY_W: m_currentComponent.RotateX(true); break;
	case GLFW_KEY_S: m_currentComponent.RotateX(false); break;
	case GLFW_KEY_E: m_currentComponent.RotateY(true); break;
	case GLFW_KEY_Q: m_currentComponent.RotateY(false); break;
	case GLFW_KEY_D: m_currentComponent.RotateZ(true); break;
	case GLFW_KEY_A: m_currentComponent.RotateZ(false); break;
	}

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
		m_currentComponent = Voxel::Voxel(Voxel::ComponentType(_key - GLFW_KEY_0));
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
			m_ship->AddComponent( m_lvl0Position, m_currentComponent );
		}
		m_ship->ComputeParameters();
		m_recreateThrustVis = true;

		UpdateModelInformation();
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
	if( all(m_lvl0Position == m_ship->GetCentralComputerPosition()) )
		m_validPosition = false;
}

// ************************************************************************* //
void GSEditor::UpdateModelInformation()
{
	struct CostFetcher : public Voxel::Model::ModelData::SVOProcessor
	{
	public:
		std::array < unsigned int, 6> costs;

		CostFetcher() :
			costs({ { 0, 0, 0, 0, 0, 0 } })
		{
		}

		bool PreTraversal(const IVec4& _position, const Voxel::Model::ModelData::SVON* _node)
		{
			if (_position[3] == 0)
			{
				costs[0] += Voxel::TypeInfo::GetHydrogen(_node->Data().type);
				costs[1] += Voxel::TypeInfo::GetCarbon(_node->Data().type);
				costs[2] += Voxel::TypeInfo::GetMetals(_node->Data().type);
				costs[3] += Voxel::TypeInfo::GetRareEarthElements(_node->Data().type);
				costs[4] += Voxel::TypeInfo::GetSemiconductors(_node->Data().type);
				costs[5] += Voxel::TypeInfo::GetHeisenbergium(_node->Data().type);
			}

			return true;
		}
	};

	CostFetcher costFetcher;
	m_ship->GetVoxelTree().Traverse(costFetcher);

	for (int i = 0; i < 6; ++i)
	{
		m_hud->m_resourceCosts[i]->SetText( StringUtils::ToConstDigit( costFetcher.costs[i], 8 ) );
	}

	m_hud->m_mass->SetText(StringUtils::ToConstDigit((int)m_ship->GetMass(), 8));
}
