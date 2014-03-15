#include <thread>
#include <cassert>
#include "game.hpp"
#include "gamestates/gsmainmenu.hpp"
#include "gamestates/gsplay.hpp"
#include "gamestates/gseditor.hpp"
#include "opengl.hpp"
#include "timer.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/content.hpp"
#include "input/input.hpp"
#include "voxel/voxel.hpp"

static double g_fInvFrequency;

// ************************************************************************* //
Monolith::Monolith( float _fTargetFrameRate ) :
	m_singleThreaded( true ),
	m_running( true ),
	m_time( 0.0 ),
	m_stateStack( nullptr ),
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
		LOG_ERROR("Failed to load config file with message:\n" + _message + '\n' + "\nLoading default configuration.");
		BuildDefaultConfig();
	}

	assert(glGetError() == GL_NO_ERROR);

	Input::Manager::Initialize( Graphic::Device::GetWindow(), Config[std::string("Input")] );

	Voxel::TypeInfo::Initialize();

	// Load the graphic stuff
	m_graficContent = new Graphic::Content();

	// Create game states
	m_gameStates[0] = new GSMainMenu(this);
	m_gameStates[1] = new GSPlay(this);
	m_gameStates[2] = new GSEditor(this);

	PushState( GetMainMenuState() );
}

// ************************************************************************* //
Monolith::~Monolith()
{
	Input::Manager::Close();

	try {
		Jo::Files::HDDFile file( "config.json" );
		Config.Write( file, Jo::Files::Format::JSON );
	} catch(...) {
		LOG_ERROR("Could not write a config file!");
	}
	delete m_graficContent;
	for( int i = 0; i < 3; ++i )
		delete m_gameStates[i];

	Voxel::TypeInfo::Unload();
}

// ************************************************************************* //
void Monolith::Run()
{
	if( m_singleThreaded )
	{
		m_stateStack->Update( m_time, 0.0 );
		TimeQuerySlot frameTimer;
		TimeQuery( frameTimer );
		double deltaTime = 0.0;
		while( m_running && !glfwWindowShouldClose(Graphic::Device::GetWindow()) && ClearStack() )
		{			
			// Call stuff
			Input::Manager::Update();
			m_stateStack->Render( m_time, deltaTime );
			m_stateStack->Update( m_time, deltaTime );

			glfwSwapBuffers(Graphic::Device::GetWindow());
			glfwPollEvents();

			//glFinish();

			// Calculate time since last frame
			double deltaFrameTime = TimeQuery( frameTimer );
			// Smooth frame time
			deltaTime = deltaTime * 0.8 + deltaFrameTime * 0.2;
			m_time += deltaTime;
			m_graficContent->globalUBO["Time"] = (float)m_time;

			// Limiting to target fps
			//std::this_thread::sleep_for( m_microSecPerFrame - std::chrono::microseconds(unsigned(deltaFrameTime * 1000000.0))  );
		}
	}
}

// ************************************************************************* //
void Monolith::_PushState( IGameStateP _state )
{
	// Set in input manager
	Input::Manager::SetGameState( _state );

	// Update stack
	_state->m_previous = m_stateStack;
	_state->m_finished = false;
	m_stateStack = _state;
}

// ************************************************************************* //
bool Monolith::ClearStack()
{
	if( m_stateStack->IsFinished() )
	{
		// Pop
		m_stateStack->OnEnd();
		m_stateStack = m_stateStack->m_previous;
		// If there is none anymore stop
		if( !m_stateStack ) return false;
		// Otherwise resume and continue
		m_stateStack->OnResume();
		Input::Manager::SetGameState( m_stateStack );
	}
	return true;
}

// ************************************************************************* //
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

// ************************************************************************* //
GSMainMenu* Monolith::GetMainMenuState()
{
	return static_cast<GSMainMenu*>(m_gameStates[0]);
}
GSPlay* Monolith::GetPlayState()
{
	return static_cast<GSPlay*>(m_gameStates[1]);
}
GSEditor* Monolith::GetEditorState()
{
	return static_cast<GSEditor*>(m_gameStates[2]);
}