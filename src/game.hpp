#pragma once

#include <chrono>
#include "predeclarations.hpp"
#include "gameloop.hpp"
#include <jofilelib.hpp>
#include <hybridarray.hpp>

class Monolith;

/// \brief Game loop for fixed step size simulation code.
/// \details Handles simple simulations, input and poll network messages.
class RenderLoop: public GameLoop
{
public:
	RenderLoop(Monolith& _game) : GameLoop("RenderLoop", 0.0f/60.0f), m_game(_game) {}
	void Step( double _deltaTime );
	/// \brief Just continue rendering - error messages are reported by the game loop.
	void OnFailure()	{}
private:
	Monolith& m_game;
};

/// \brief Game loop for rendering.
class SimulationLoop: public GameLoop
{
public:
	SimulationLoop(Monolith& _game) : GameLoop("SimulationLoop", 1.0f/80.0f), m_game(_game) {}
	void Step( double _deltaTime );
	/// \brief Save the game and make an stop to check everything.
	void OnFailure();
private:
	Monolith& m_game;
};

/// \brief A loop which handles all other loops in one.
class CompositeLoop: public GameLoop
{
public:
	CompositeLoop() : GameLoop("CompositeLoop", 1.0f/60.0f) {}
	void Step( double _deltaTime );
	void OnFailure();
	/// \brief Register a loop. Memory management must be done outside.
	void AddLoop( GameLoop* _loop );
private:
	Jo::HybridArray<GameLoop*> m_loops;
};

/// \brief The game consists of a main-loop and several different game-states.
/// \details Game states are managed on a stack based on the following rules:
///		* Each state is constructed at startup and deleted automatically at
///		  the end. Load as much general stuff as possible here
///		* Anytime a state is pushed to the stack its OnBegin is called. Load
///		  context depending data here. To pass arguments into a state give
///		  additional parameters to PushState. Only one OnBegin with the
///		  appropriate number of input arguments must exist.
class Monolith
{
public:
	/// \brief Create a single threaded variant with limited frame rate.
	Monolith( bool _singleThreaded );

	/// \brief Delete all created resources
	~Monolith();

	/// \brief Start the game. This method will return after the game closed.
	void Run();

	/// \brief Push a state on top of the stack. Pass up to 5 arguments into
	///		the state's OnBegin (after pushing it).
	/// \details This makes the pushed state registered in the input manager
	///		and sets its internal parent state.
	///		
	///		Every state should be on the stack at most once. Do not design
	///		cyclic state changes. Just finish states to go back as long as needed.
	template<class TS>
	void PushState( TS* _state )											{ _PushState(_state); _state->OnBegin(); ContinueAllLoops(); }
	template<class TS, typename T1>
	void PushState( TS* _state, T1 _p1 )									{ _PushState(_state); _state->OnBegin(_p1); ContinueAllLoops(); }
	template<class TS, typename T1, typename T2>
	void PushState( TS* _state, T1 _p1, T2 _p2 )							{ _PushState(_state); _state->OnBegin(_p1, _p2); ContinueAllLoops(); }
	template<class TS, typename T1, typename T2, typename T3>
	void PushState( TS* _state, T1 _p1, T2 _p2, T3 _p3 )					{ _PushState(_state); _state->OnBegin(_p1, _p2, _p3); ContinueAllLoops(); }
	template<class TS, typename T1, typename T2, typename T3, typename T4>
	void PushState( TS* _state, T1 _p1, T2 _p2, T3 _p3, T4 _p4 )			{ _PushState(_state); _state->OnBegin(_p1, _p2, _p3, p4); ContinueAllLoops(); }
	template<class TS, typename T1, typename T2, typename T3, typename T4, typename T5>
	void PushState( TS* _state, T1 _p1, T2 _p2, T3 _p3, T4 _p4, T5 _p5 )	{ _PushState(_state); _state->OnBegin(_p1, _p2, _p3, p4, _p5); ContinueAllLoops(); }

	GSMainMenu* GetMainMenuState();
	GSPlay* GetPlayState();
	GSEditor* GetEditorState();
	GSEditorChoice* GetEditorChoiceState();
	GSGraphicOpt* GetGraphicOptState();
	GSInputOpt* GetInputOptState();
	GSGameplayOpt* GetGameplayOptState();
	GSSoundOpt* GetSoundOptState();

	IGameStateP GetState()	{ return m_stateStack; }

	Jo::Files::MetaFileWrapper Config;

	static float Time() { return (float)m_time; }
private:
	friend class RenderLoop;
	friend class CompositeLoop;
	friend class SimulationLoop;
	CompositeLoop m_compositeLoop;
	RenderLoop m_renderLoop;
	SimulationLoop m_simulationLoop;
	IGameStateP m_gameStates[8];	///< MainMenu, Play, Editor, EditorChoice, GraphicOpt, InputOpt, GameplayOpt, SoundOpt
	IGameStateP m_stateStack;		///< The head of a stack of game states
	bool m_singleThreaded;

	static double m_time;					///< Total time since run in seconds

	Graphic::Texture* m_sceneDepthTexture; ///< Main depth target for the 3D scene.
	Graphic::Texture* m_sceneColorTexture; ///< Main color target for the 3D scene.
	Graphic::Framebuffer* m_sceneFramebuffer; ///< Framebuffer into which the entire 3D scene is rendered before it is passed to the postprocessing module.
	Graphic::PostProcessing* m_postProcessing;

	/// \brief The kernel of the push state method
	void _PushState( IGameStateP _state );

	/// \brief Remove all finished states.
	/// \return true as long there is a not-finished state.
	bool ClearStack();

	void BuildDefaultConfig();

	/// \brief Stop parallel task if game states change
	void PauseAllLoops();
	void ContinueAllLoops();
};