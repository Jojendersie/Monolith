#include "../game.hpp"
#include "../graphic/device.hpp"
#include "../math/math.hpp"
using namespace Math;
#include <cassert>

// TODO: remove after test
#include "../generators/asteroid.hpp"
#include "../graphic/font.hpp"

GSMain::GSMain()
{
	m_astTest = new Generators::Asteroid( 32, 32, 32 );
	m_astTest->ComputeVertexBuffer();
	m_astTest->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );

	m_fontTest = new Graphic::Font();
}

GSMain::~GSMain()
{
	delete m_astTest;
	delete m_fontTest;
}

void GSMain::Update( double _time, double _deltaTime )
{
//	_time = 0.1 * cos(_time*0.5);
	Matrix view = MatrixCamera( Vec3(sin(_time)*100,40.0f,cos(_time)*100), Vec3(0.0f,0.0f,0.0f) );
	Matrix projection = MatrixProjection( 0.6f, 1.3f, 0.5f, 400.0f );
	Matrix viewProjection = view * projection;
	m_parent->content.cameraUBO["View"] = view;
	m_parent->content.cameraUBO["Projection"] = projection;
	m_parent->content.cameraUBO["ViewProjection"] = viewProjection;
	m_parent->content.cameraUBO["ProjectionInverse"] = Vec3(1.0f/projection.m11, 1.0f/projection.m22, 1.0f/projection.m33);
	m_parent->content.cameraUBO["Far"] = 400.0f;
}

void GSMain::Render( double _time, double _deltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	Graphic::Device::SetEffect(	m_parent->content.voxelRenderEffect );

	m_astTest->Draw( m_parent->content.objectUBO, m_parent->content.cameraUBO["ViewProjection"] );
}