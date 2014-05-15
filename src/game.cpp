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

static double g_fInvFrequency;

// ************************************************************************* //
Monolith::Monolith( float _fTargetFrameRate ) :
	m_singleThreaded( true ),
	m_running( true ),
	m_time( 0.0 ),
	m_stateStack( nullptr )
{
	// Init scene framebuffer
	{
		using namespace Graphic;
		m_sceneColorTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
			Texture::Format(4, 8, Texture::Format::ChannelType::UINT));
		m_sceneDepthTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
			Texture::Format(1, 32, Texture::Format::ChannelType::FLOAT, Texture::Format::FormatType::DEPTH));
		m_sceneFramebuffer = new Framebuffer(Framebuffer::Attachment(m_sceneColorTexture), Framebuffer::Attachment(m_sceneDepthTexture));

		m_postProcessing = new PostProcessing();
	}

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

	Assert(glGetError() == GL_NO_ERROR, "GL during initialization!");

	Resources::LoadLanguageData( Config[std::string("Game")][std::string("Language")] );
	Input::Manager::Initialize( Graphic::Device::GetWindow(), Config[std::string("Input")] );

	Voxel::TypeInfo::Initialize();

	// Create game states
	m_gameStates[0] = new GSMainMenu(this);
	m_gameStates[1] = new GSPlay(this);
	m_gameStates[2] = new GSEditor(this);
	m_gameStates[3] = new GSEditorChoice(this);
	m_gameStates[4] = new GSGraphicOpt(this);
	m_gameStates[5] = new GSInputOpt(this);
	m_gameStates[6] = new GSGameplayOpt(this);
	m_gameStates[7] = new GSSoundOpt(this);

	PushState( GetMainMenuState() );
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
		m_stateStack->Update( m_time, 0.0 );
		TimeQuerySlot frameTimer;
		TimeQuery( frameTimer );
		double deltaTime = 0.0;
		while( m_running && !glfwWindowShouldClose(Graphic::Device::GetWindow()) && ClearStack() )
		{			
			// Call stuff

			// Render to scene framebuffer.
			Graphic::Device::BindFramebuffer(m_sceneFramebuffer);

			m_stateStack->Render( m_time, deltaTime );

			// Postprocessing and draw to backbuffer.
			m_postProcessing->PerformPostProcessing(*m_sceneColorTexture, *m_sceneDepthTexture);

			// Perform updates
			Input::Manager::Update();
			m_stateStack->Update(m_time, deltaTime);

			glfwSwapBuffers(Graphic::Device::GetWindow());
			glfwPollEvents();

#ifdef AUTO_SHADER_RELOAD
			Graphic::Effect::UpdateShaderFileWatcher();
#endif


			//glFinish();

			// Calculate time since last frame
			double deltaFrameTime = TimeQuery( frameTimer );
			// Smooth frame time
			deltaTime = deltaTime * 0.8 + deltaFrameTime * 0.2;
			m_time += deltaTime;
			(*Graphic::Resources::GetUBO(Graphic::UniformBuffers::GLOBAL))["Time"] = (float)m_time;

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
	cinput[std::string("EditorDeletionMode")][0] = 340;

	auto& cgame = Config[std::string("Game")];
	cgame[std::string("Language")] = "english.json";
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