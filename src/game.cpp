#include <thread>
#include <cassert>
#include "game.hpp"
#include "opengl.hpp"
#include "timer.hpp"
#include "graphic/device.hpp"
#include "graphic/uniformbuffer.hpp"

static double g_fInvFrequency;

Monolith::Monolith( float _fTargetFrameRate ) :
	m_singleThreaded( true ),
	m_running( true ),
	m_time( 0.0 ),
	m_currentGameState( 0 )
{
	// Init timer
	g_fInvFrequency = std::chrono::high_resolution_clock::period::num/double(std::chrono::high_resolution_clock::period::den);
	m_microSecPerFrame = std::chrono::microseconds(unsigned(1000000.0 / _fTargetFrameRate));

	assert(glGetError() == GL_NO_ERROR);

	// Create game states
	m_gameStates[0] = new GSMain();
	m_gameStates[0]->m_parent = this;
}

Monolith::~Monolith()
{
	delete m_gameStates[0];
}

void Monolith::Run()
{
	if( m_singleThreaded )
	{
		m_gameStates[m_currentGameState]->Update( m_time, 0.0 );
		TimeQuerySlot frameTimer;
		TimeQuery( frameTimer );
		double deltaTime = 0.0;
		while( m_running && !glfwWindowShouldClose(Graphic::Device::GetWindow()) )
		{
			// Call stuff
			m_gameStates[m_currentGameState]->Render( m_time, deltaTime );
			m_gameStates[m_currentGameState]->Update( m_time, deltaTime );

			glfwSwapBuffers(Graphic::Device::GetWindow());
			glfwPollEvents();

			//glFinish();

			// Calculate time since last frame
			double deltaFrameTime = TimeQuery( frameTimer );
			// Smooth frame time
			deltaTime = deltaTime * 0.8 + deltaFrameTime * 0.2;
			m_time += deltaTime;

			// Limiting to target fps
			//std::this_thread::sleep_for( m_microSecPerFrame - std::chrono::microseconds(unsigned(deltaFrameTime * 1000000.0))  );
		}
	}
}


Monolith::_Content::_Content() :
	voxelRenderEffect( "shader/voxel.vs", "shader/voxel.gs", "shader/voxel.ps",
		Graphic::RasterizerState::CULL_MODE::BACK, Graphic::RasterizerState::FILL_MODE::SOLID ),
		objectUBO( "Object" ), cameraUBO( "Camera" ),
	pointSampler(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::POINT,
				Graphic::SamplerState::SAMPLE::POINT, Graphic::SamplerState::SAMPLE::LINEAR ),
	linearSampler(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::LINEAR,
				Graphic::SamplerState::SAMPLE::LINEAR, Graphic::SamplerState::SAMPLE::LINEAR )
{
	// Init the constant buffers
	objectUBO.AddAttribute( "WorldViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	objectUBO.AddAttribute( "Corner000", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner001", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner010", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner011", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner100", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner101", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner110", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );
	objectUBO.AddAttribute( "Corner111", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4 );

	cameraUBO.AddAttribute( "View", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	cameraUBO.AddAttribute( "Projection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	cameraUBO.AddAttribute( "ViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX );
	cameraUBO.AddAttribute( "ProjectionInverse", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC3 );
	cameraUBO.AddAttribute( "Far", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT );

	// Bind constant buffers to effects
	voxelRenderEffect.BindUniformBuffer( objectUBO );
	voxelRenderEffect.BindUniformBuffer( cameraUBO );
	voxelRenderEffect.BindTexture("u_diffuseTex", 0, pointSampler);
	assert(glGetError() == GL_NO_ERROR);
}