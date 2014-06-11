// TESTING CODE
#include "graphic/core/opengl.hpp"
// -- PLEASE REMOVE WHEN DONE WITH POSTPROCESSING

#include <thread>
#include "utilities/assert.hpp"
#include "game.hpp"
#include "gamestates/gsmainmenu.hpp"
#include "gamestates/gsplay.hpp"
#include "gamestates/gseditor.hpp"
#include "gamestates/gseditorchoice.hpp"
#include "gamestates/gsgraphicopt.hpp"
#include "gamestates/gsinputopt.hpp"
#include "gamestates/gsgameplayopt.hpp"
#include "gamestates/gssoundopt.hpp"
#include "timer.hpp"

#include "graphic/core/device.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/core/framebuffer.hpp"
#include "graphic/core/effect.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/highlevel/postprocessing.h"
#include "graphic/content.hpp"

#include "input/input.hpp"
#include "voxel/voxel.hpp"
#include "resources.hpp"

double Monolith::m_time = 0.0;

// ************************************************************************* //
void RenderLoop::Step( double _deltaTime )
{
	(*Graphic::Resources::GetUBO(Graphic::UniformBuffers::GLOBAL))["Time"] = (float)m_game.m_time;

	// Render to scene frame buffer.
	Graphic::Device::BindFramebuffer( m_game.m_sceneFramebuffer );

	m_game.GetState()->Render( _deltaTime );

	// Post processing and draw to back buffer.
	m_game.m_postProcessing->PerformPostProcessing(*m_game.m_sceneColorTexture, *m_game.m_sceneDepthTexture);

	glfwSwapBuffers(Graphic::Device::GetWindow());
	glFinish();

#ifdef AUTO_SHADER_RELOAD
	Graphic::Effect::UpdateShaderFileWatcher();
#endif
}

// ************************************************************************* //
void SimulationLoop::Step( double _deltaTime )
{
	// Perform updates
	m_game.GetState()->Simulate(_deltaTime);

	glfwPollEvents();

	if( glfwWindowShouldClose(Graphic::Device::GetWindow()) || !m_game.ClearStack() )
		GameLoop::StopAll();

	m_game.m_time += _deltaTime;
}

void SimulationLoop::OnFailure()
{
}

// ************************************************************************* //
void CompositeLoop::Step( double _deltaTime )
{
	for(unsigned i = 0; i < m_loops.Size(); ++i)
		m_loops[i]->Step( _deltaTime );
}

void CompositeLoop::OnFailure()
{
	for(unsigned i = 0; i < m_loops.Size(); ++i)
		m_loops[i]->OnFailure();
}

void CompositeLoop::AddLoop( GameLoop* _loop )
{
	m_loops.PushBack( _loop );
}

// ************************************************************************* //
Monolith::Monolith( bool _singleThreaded ) :
	m_renderLoop( *this ),
	m_simulationLoop( *this ),
	m_singleThreaded( _singleThreaded ),
	m_stateStack( nullptr )
{
	// Load configuration
	try {
		Jo::Files::HDDFile file( "config.json" );
		Config.Read( file, Jo::Files::Format::JSON );
	} catch( std::string _message ) {
		LOG_ERROR("Failed to load config file with message:\n" + _message + '\n' + "\nLoading default configuration.");
		BuildDefaultConfig();

		try {
			Jo::Files::HDDFile file("config.json", Jo::Files::HDDFile::CREATE_FILE);
			Config.Write(file, Jo::Files::Format::JSON);
		} catch (std::string _message) {
			LOG_ERROR("Failed to write config file with message:\n" + _message + '\n' + "\n");
		}
	}

	// Create a device with a window
	int screenWidth = Config[std::string("Graphics")][std::string("ScreenWidth")].Get(1366);
	int screenHeight = Config[std::string("Graphics")][std::string("ScreenHeight")].Get(768);
	Graphic::Device::Initialize(screenWidth, screenHeight, false);

	Resources::LoadLanguageData( Config[std::string("Game")][std::string("Language")] );
	Input::Manager::Initialize( Graphic::Device::GetWindow(), Config[std::string("Input")] );

	Voxel::TypeInfo::Initialize();

	// Init scene frame buffer
	{
		using namespace Graphic;
		m_sceneColorTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
			Texture::Format(4, 8, Texture::Format::ChannelType::UINT));
		m_sceneDepthTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
			Texture::Format(1, 32, Texture::Format::ChannelType::FLOAT, Texture::Format::FormatType::DEPTH));
		m_sceneFramebuffer = new Framebuffer(Framebuffer::Attachment(m_sceneColorTexture), Framebuffer::Attachment(m_sceneDepthTexture));
		PostProcessing::AmbientOcclusionConfig aoConfig = static_cast<PostProcessing::AmbientOcclusionConfig>(
										Config[std::string("Graphics")][std::string("SSAO")].Get(static_cast<int>(PostProcessing::AmbientOcclusionConfig::OFF)));
		m_postProcessing = new PostProcessing(aoConfig);
	}

	// Create game states
	m_gameStates[0] = new GSMainMenu(this);
	m_gameStates[1] = new GSPlay(this);
	m_gameStates[2] = new GSEditor(this);
	m_gameStates[3] = new GSEditorChoice(this);
	m_gameStates[4] = new GSGraphicOpt(this);
	m_gameStates[5] = new GSInputOpt(this);
	m_gameStates[6] = new GSGameplayOpt(this);
	m_gameStates[7] = new GSSoundOpt(this);

	PushState(GetMainMenuState());
}

// ************************************************************************* //
Monolith::~Monolith()
{
	// Finalize all game states (not all must be closed if an exception or
	// message caused the program to close
	while( m_stateStack )
	{
		// Pop
		m_stateStack->OnEnd();
		m_stateStack = m_stateStack->m_previous;
	}

	Input::Manager::Close();

	try {
		Jo::Files::HDDFile file( "config.json" );
		Config.Write( file, Jo::Files::Format::JSON );
	} catch(...) {
		LOG_ERROR("Could not write a config file!");
	}
	for( int i = 0; i < 8; ++i )
		delete m_gameStates[i];

	Graphic::Resources::Unload();
	Voxel::TypeInfo::Unload();

	delete m_postProcessing;
	delete m_sceneFramebuffer;
	delete m_sceneColorTexture;
	delete m_sceneDepthTexture;
}

// ************************************************************************* //
void Monolith::Run()
{
	if( m_singleThreaded )
	{
		LOG_LVL2( "Starting single-threaded game loop." );
		m_compositeLoop.AddLoop( &m_renderLoop );
		m_compositeLoop.AddLoop( &m_simulationLoop );
		m_compositeLoop.Run();
	} else {
		// Run most of the loops in Parallel
		LOG_LVL2( "Starting multi-threaded game loop." );

		// Pass the context to the render thread and get it back later
		glfwMakeContextCurrent( nullptr );
		std::thread thread1([](GameLoop* _loop){
				LOG_LVL2( "Created GameLoop thread." );
				glfwMakeContextCurrent( Graphic::Device::GetWindow() );
				_loop->Run();
				glfwMakeContextCurrent( nullptr );
			}, &m_renderLoop
		);

		// The last one execute in this thread
		m_simulationLoop.Run();

		thread1.join();
		LOG_ERROR("ASDF3");
		glfwMakeContextCurrent( Graphic::Device::GetWindow() );
		LOG_ERROR("ASDF4");
	}
	LOG_LVL1("Returned from game loops.");
}

// ************************************************************************* //
void Monolith::_PushState( IGameStateP _state )
{
	PauseAllLoops();

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
		PauseAllLoops();
		LOG_LVL2( std::string("Finishing game state: ") + typeid(*m_stateStack).name() );
		// Pop
		m_stateStack->OnEnd();
		m_stateStack = m_stateStack->m_previous;
		// If there is none anymore stop
		if( !m_stateStack ) return false;
		// Otherwise resume and continue
		m_stateStack->OnResume();
		Input::Manager::SetGameState( m_stateStack );
		LOG_LVL2( std::string("Resumed to game state: ") + typeid(*m_stateStack).name() );
		ContinueAllLoops();
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
	cinput[std::string("EditorDeletionMode")][0] = 340;

	auto& cgame = Config[std::string("Game")];
	cgame[std::string("Language")] = "english.json";

	auto& cgraphics = Config[std::string("Graphics")];
	cgraphics[std::string("ScreenWidth")] = 1366;
	cgraphics[std::string("ScreenHeight")] = 768;
	cgraphics[std::string("SSAO")] = 0;
}

// ************************************************************************* //
GSMainMenu* Monolith::GetMainMenuState()			{ return static_cast<GSMainMenu*>(m_gameStates[0]); }
GSPlay* Monolith::GetPlayState()					{ return static_cast<GSPlay*>(m_gameStates[1]); }
GSEditor* Monolith::GetEditorState()				{ return static_cast<GSEditor*>(m_gameStates[2]); }
GSEditorChoice* Monolith::GetEditorChoiceState()	{ return static_cast<GSEditorChoice*>(m_gameStates[3]); }
GSGraphicOpt* Monolith::GetGraphicOptState()		{ return static_cast<GSGraphicOpt*>(m_gameStates[4]); }
GSInputOpt* Monolith::GetInputOptState()			{ return static_cast<GSInputOpt*>(m_gameStates[5]); }
GSGameplayOpt* Monolith::GetGameplayOptState()		{ return static_cast<GSGameplayOpt*>(m_gameStates[6]); }
GSSoundOpt* Monolith::GetSoundOptState()			{ return static_cast<GSSoundOpt*>(m_gameStates[7]); }

// ************************************************************************* //
void Monolith::PauseAllLoops()
{
	m_simulationLoop.Pause();
	m_renderLoop.Pause();
}

void Monolith::ContinueAllLoops()
{
	m_simulationLoop.Continue();
	m_renderLoop.Continue();
}
