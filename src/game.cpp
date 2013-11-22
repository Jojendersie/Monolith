#include <thread>
#include <cassert>
#include <iostream>
#include "game.hpp"
#include "opengl.hpp"
#include "timer.hpp"
#include "graphic/device.hpp"
#include "graphic/uniformbuffer.hpp"
#include "graphic/texture.hpp"
#include "input/input.hpp"

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

	// Load configuration
	try {
		Jo::Files::HDDFile file( "config.json", true );
		Config.Read( file, Jo::Files::Format::JSON );
	} catch( std::string _message ) {
		std::cerr << "Failed to load config file with message:\n";
		std::cerr << _message << '\n';
		std::cerr << "Loading default configuration.\n";
		BuildDefaultConfig();
	}

	assert(glGetError() == GL_NO_ERROR);

	Input::Manager::Initialize( Graphic::Device::GetWindow() );

	// Create game states
	m_gameStates[0] = new GSMain();
	m_gameStates[0]->m_parent = this;

	Input::Manager::SetGameState( m_gameStates[0] );
}

Monolith::~Monolith()
{
	try {
		Jo::Files::HDDFile file( "config.json", false );
		Config.Write( file, Jo::Files::Format::JSON );
	} catch(...) {
		std::cerr << "Could not write a config file!";
	}
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
			Input::Manager::Update();
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

	// Load array texture for all voxels
	std::vector<std::string> textureNames;
	textureNames.push_back( "texture/none.png" );
	textureNames.push_back( "texture/rock1.png" );
	textureNames.push_back( "texture/water.png" );
	try {
		voxelTextures = new Graphic::Texture(textureNames);
	} catch( std::string _message ) {
		std::cerr << "Failed to load voxel textures!\n";
	}
}


Monolith::_Content::~_Content()
{
	delete voxelTextures;
}


void Monolith::BuildDefaultConfig()
{
	auto& cinput = Config[std::string("Input")];
	cinput[std::string("MoveCamera")][0] = std::string("MR");
	cinput[std::string("RotateCamera")][0] = std::string("MM");
	cinput[std::string("Zoom")][0] = std::string("MLD");
	cinput[std::string("Zoom")][1] = std::string("z");
	cinput[std::string("CameraRotationSpeed")] = 0.005;
	cinput[std::string("CameraMovementSpeed")] = 0.0025;
	cinput[std::string("CameraScrollSpeed")] = 5.0;
}