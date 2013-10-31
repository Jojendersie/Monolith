#include <thread>
#include <cassert>
#include "game.hpp"
#include "opengl.hpp"
#include "graphic/device.hpp"
#include "graphic/uniformbuffer.hpp"

static double g_fInvFrequency;

Monolith::Monolith( float _fTargetFrameRate ) :
	m_bSingleThreaded( true ),
	m_bRunning( true ),
	m_fTime( 0.0 ),
	m_iCurrentGameState( 0 )
{
	// Init timer
	g_fInvFrequency = std::chrono::high_resolution_clock::period::num/double(std::chrono::high_resolution_clock::period::den);
	m_iMicroSecPerFrame = std::chrono::microseconds(unsigned(1000000.0 / _fTargetFrameRate));

	assert(glGetError() == GL_NO_ERROR);

	// Create game states
	m_ppGameStates[0] = new GSMain();
	m_ppGameStates[0]->m_pParent = this;
}

Monolith::~Monolith()
{
	delete m_ppGameStates[0];
}

void Monolith::Run()
{
	if( m_bSingleThreaded )
	{
		m_ppGameStates[m_iCurrentGameState]->Update( m_fTime, 0.0 );
		std::chrono::high_resolution_clock::time_point StartPoint, EndPoint;
		StartPoint = std::chrono::high_resolution_clock::now();
		while( m_bRunning && !glfwWindowShouldClose(Graphic::Device::GetWindow()) )
		{
			// Calculate time since last frame
			EndPoint = std::chrono::high_resolution_clock::now();
			std::chrono::high_resolution_clock::rep Ticks = (EndPoint - StartPoint).count();
			StartPoint = EndPoint;

			double fDeltaTime = Ticks * g_fInvFrequency;
			m_fTime += fDeltaTime;

			// Call stuff
			m_ppGameStates[m_iCurrentGameState]->Render( m_fTime, fDeltaTime );
			m_ppGameStates[m_iCurrentGameState]->Update( m_fTime, fDeltaTime );

			glfwSwapBuffers(Graphic::Device::GetWindow());
			glfwPollEvents();

			glFinish();

			// Limiting to target fps
		//	std::this_thread::sleep_for( m_iMicroSecPerFrame - std::chrono::microseconds(unsigned(fDeltaTime * 1000000.0))  );
		}
	}
}


Monolith::_Content::_Content() :
	VoxelRenderEffect( "shader/voxel.vs", "shader/voxel.gs", "shader/voxel.ps",
	Graphic::RasterizerState::CULL_MODE::NONE, Graphic::RasterizerState::FILL_MODE::SOLID ),
	ObjectUBO( "Object" ), CameraUBO( "Camera" )
{
	// Init the constant buffers
	ObjectUBO.AddAttribute( "WorldViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	ObjectUBO.AddAttribute( "Corner000", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner001", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner010", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner011", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner100", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner101", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner110", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	ObjectUBO.AddAttribute( "Corner111", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );

	CameraUBO.AddAttribute( "View", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	CameraUBO.AddAttribute( "Projection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	CameraUBO.AddAttribute( "ViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	CameraUBO.AddAttribute( "ProjectionInverse", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC3 );
	CameraUBO.AddAttribute( "Far", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );

	// Bind constant buffers to effects
	ObjectUBO.BindToShader( VoxelRenderEffect.GetProgramID() );
	CameraUBO.BindToShader( VoxelRenderEffect.GetProgramID() );
	assert(glGetError() == GL_NO_ERROR);
}