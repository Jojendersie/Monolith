#include <thread>
#include <cassert>
#include <iostream>
#include "game.hpp"
#include "gamestates/gsmain.hpp"
#include "opengl.hpp"
#include "timer.hpp"
#include "graphic/device.hpp"
#include "graphic/uniformbuffer.hpp"
#include "graphic/content.hpp"
#include "input/input.hpp"

static double g_fInvFrequency;

Monolith::Monolith( float _fTargetFrameRate ) :
	m_singleThreaded( true ),
	m_running( true ),
	m_time( 0.0 ),
	m_currentGameState( 0 ),
	m_graficContent( nullptr )
{
	// Init timer
	g_fInvFrequency = std::chrono::high_resolution_clock::period::num/double(std::chrono::high_resolution_clock::period::den);
	m_microSecPerFrame = std::chrono::microseconds(unsigned(1000000.0 / _fTargetFrameRate));

	// Load configuration
	try {
		Jo::Files::HDDFile file( "config.json" );
		Config.Read( file, Jo::Files::Format::JSON );
	} catch( std::string _message ) {
		std::cerr << "Failed to load config file with message:\n";
		std::cerr << _message << '\n';
		std::cerr << "Loading default configuration.\n";
		BuildDefaultConfig();
	}

	assert(glGetError() == GL_NO_ERROR);

	Input::Manager::Initialize( Graphic::Device::GetWindow(), Config[std::string("Input")] );

	// Load the graphic stuff
	m_graficContent = new Graphic::Content();

	// Create game states
	m_gameStates[0] = new GSMain(this);

	Input::Manager::SetGameState( m_gameStates[0] );
}

Monolith::~Monolith()
{
	Input::Manager::Close();

	try {
		Jo::Files::HDDFile file( "config.json" );
		Config.Write( file, Jo::Files::Format::JSON );
	} catch(...) {
		std::cerr << "Could not write a config file!";
	}
	delete m_graficContent;
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


void Monolith::BuildDefaultConfig()
{
	auto& cinput = Config[std::string("Input")];
	cinput[std::string("MoveCamera")][0] = 1;
	cinput[std::string("RotateCamera")][0] = 2;
	cinput[std::string("Zoom")][0] = 90;
	cinput[std::string("CameraRotationSpeed")] = 0.005;
	cinput[std::string("CameraMovementSpeed")] = 0.0025;
	cinput[std::string("CameraScrollSpeed")] = 5.0;
}