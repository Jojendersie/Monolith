#include "../game.hpp"
#include "../graphic/device.hpp"
#include "../math/math.hpp"
using namespace Math;
#include <cassert>

// TODO: remove after test
#include "../generators/asteroid.hpp"

GSMain::GSMain()
{
	m_pAstTest = new Generators::Asteroid( 32, 32, 32 );
	m_pAstTest->ComputeVertexBuffer();
	m_pAstTest->SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );
}

GSMain::~GSMain()
{
	delete m_pAstTest;
}

void GSMain::Update( double _fTime, double _fDeltaTime )
{
//	_fTime = 0.1 * cos(_fTime*0.5);
	Matrix mView = MatrixCamera( Vec3(sin(_fTime)*100,40.0f,cos(_fTime)*100), Vec3(0.0f,0.0f,0.0f) );
	Matrix mProjection = MatrixProjection( 0.6f, 1.3f, 0.5f, 400.0f );
	Matrix mViewProjection = mView * mProjection;
	m_pParent->Content.CameraUBO["View"] = mView;
	m_pParent->Content.CameraUBO["Projection"] = mProjection;
	m_pParent->Content.CameraUBO["ViewProjection"] = mViewProjection;
	m_pParent->Content.CameraUBO["ProjectionInverse"] = Vec3(1.0f/mProjection.m11, 1.0f/mProjection.m22, 1.0f/mProjection.m33);
	m_pParent->Content.CameraUBO["Far"] = 400.0f;
}

void GSMain::Render( double _fTime, double _fDeltaTime )
{
	Graphic::Device::Clear( 0.5f, 0.5f, 0.0f );

	Graphic::Device::SetEffect(	m_pParent->Content.VoxelRenderEffect );
	m_pParent->Content.CameraUBO.Commit();

	m_pAstTest->Draw( m_pParent->Content.ObjectUBO, m_pParent->Content.CameraUBO["ViewProjection"] );
}