#include "../game.hpp"
#include "../graphic/device.hpp"
#include "../math/math.hpp"
#include "../input/camera.hpp"
#include "../input/input.hpp"
using namespace Math;
#include <cassert>

// TODO: remove after test
#include "../generators/asteroid.hpp"
#include "../graphic/font.hpp"
#include "../graphic/texture.hpp"

// ************************************************************************* //
GSMain::GSMain(Monolith* _parent) : IGameState(_parent)
{
	m_astTest = new Generators::Asteroid( 80, 50, 30, 2 );
	//m_astTest->ComputeVertexBuffer();
	//m_astTest->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );

	m_fontTest = new Graphic::Font("arial", _parent->m_graficContent);
	m_textTest = new Graphic::TextRender(m_fontTest);
	//m_textTest->SetPos(Math::Vec2(-1.f,-1.f));
	m_textTest->SetPos(Math::Vec2(0.7f,0.9f));

	m_textures = new Graphic::Texture("texture/rock1.png");

	m_camera = new Input::Camera( Vec3( 0.0f, 80.0f, 250.0f ),
		Quaternion( 0.0f, 0.0f, 0.0f ),
		0.3f,
		1.3f );	// TODO: compute aspect
	m_camera->ZoomAt( *m_astTest );
}

// ************************************************************************* //
GSMain::~GSMain()
{
	delete m_camera;
	delete m_astTest;
	delete m_textTest;
	delete m_fontTest;
	delete m_textures;
}

// ************************************************************************* //
void GSMain::Update( double _time, double _deltaTime )
{
}

// ************************************************************************* //
void GSMain::Render( double _time, double _deltaTime )
{
//	_time = 0.1 * cos(_time*0.5);
	//Matrix view = MatrixCamera( Vec3(sin(_time)*250,80.0f,cos(_time)*250), Vec3(0.0f,0.0f,0.0f) );
	//Matrix projection = MatrixProjection( 0.3f, 1.3f, 0.5f, 400.0f );
	//Matrix viewProjection = view * projection;
	m_parent->m_graficContent->cameraUBO["View"] = m_camera->GetView();
	m_parent->m_graficContent->cameraUBO["Projection"] = m_camera->GetProjection();
	m_parent->m_graficContent->cameraUBO["ViewProjection"] = m_camera->GetViewProjection();
	m_parent->m_graficContent->cameraUBO["ProjectionInverse"] = Vec3(1.0f/m_camera->GetProjection()(0,0), 1.0f/m_camera->GetProjection()(1,1), 1.0f/m_camera->GetProjection()(2,2));
	m_parent->m_graficContent->cameraUBO["Far"] = 400.0f;

	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	Graphic::Device::SetEffect(	m_parent->m_graficContent->voxelRenderEffect );
	Graphic::Device::SetTexture( *m_parent->m_graficContent->voxelTextures, 0 );

	int numDrawn = m_astTest->Draw( m_parent->m_graficContent->objectUBO, *m_camera );
	
	m_textTest->SetText(std::to_string(_deltaTime * 1000.0) + " ms\n#Vox: " + std::to_string(numDrawn));
	m_textTest->Draw();
}

// ************************************************************************* //
void GSMain::UpdateInput()
{
	m_camera->UpdateMatrices();
}

// ************************************************************************* //
void GSMain::MouseMove( double _dx, double _dy )
{
	// Read config file for speed
	double rotSpeed = m_parent->Config[std::string("Input")][std::string("CameraRotationSpeed")];
	double moveSpeed = m_parent->Config[std::string("Input")][std::string("CameraMovementSpeed")];
	if( Input::Manager::IsKeyPressed(Input::Keys::ROTATE_CAMERA) )
		m_camera->Rotate( float(_dy * rotSpeed), float(_dx * rotSpeed) );
	else if( Input::Manager::IsKeyPressed(Input::Keys::MOVE_CAMERA) )
		m_camera->Move( float(_dx * moveSpeed), float(_dy * moveSpeed) );
}

// ************************************************************************* //
void GSMain::Scroll( double _dx, double _dy )
{
	double scrollSpeed = m_parent->Config[std::string("Input")][std::string("CameraScrollSpeed")];
	m_camera->Scroll( float(_dy * scrollSpeed) );
}