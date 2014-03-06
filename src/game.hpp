#pragma once

#include <chrono>
#include "predeclarations.hpp"
#include <jofilelib.hpp>



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
	Monolith( float _targetFrameRate );

	/// \brief Create a multi threaded variant with a limited frame rate for
	///		each thread.
	Monolith( float _targetRenderFR, float _targetUpdateFR, float _targetSoundFR );

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
	void PushState( TS* _state )											{ _PushState(_state); _state->OnBegin(); }
	template<class TS, typename T1>
	void PushState( TS* _state, T1 _p1 )									{ _PushState(_state); _state->OnBegin(_p1); }
	template<class TS, typename T1, typename T2>
	void PushState( TS* _state, T1 _p1, T2 _p2 )							{ _PushState(_state); _state->OnBegin(_p1, _p2); }
	template<class TS, typename T1, typename T2, typename T3>
	void PushState( TS* _state, T1 _p1, T2 _p2, T3 _p3 )					{ _PushState(_state); _state->OnBegin(_p1, _p2, _p3); }
	template<class TS, typename T1, typename T2, typename T3, typename T4>
	void PushState( TS* _state, T1 _p1, T2 _p2, T3 _p3, T4 _p4 )			{ _PushState(_state); _state->OnBegin(_p1, _p2, _p3, p4); }
	template<class TS, typename T1, typename T2, typename T3, typename T4, typename T5>
	void PushState( TS* _state, T1 _p1, T2 _p2, T3 _p3, T4 _p4, T5 _p5 )	{ _PushState(_state); _state->OnBegin(_p1, _p2, _p3, p4, _p5); }

	GSMainMenu* GetMainMenuState();
	GSPlay* GetPlayState();

	Graphic::Content* m_graficContent;

	Jo::Files::MetaFileWrapper Config;

	float Time() const { return (float)m_time; }
private:
	IGameStateP m_gameStates[2];	///< MainMenu, NewGame, Main, ...
	IGameStateP m_stateStack;		///< The head of a stack of game states
	bool m_singleThreaded;
	bool m_running;

	double m_time;					///< Total time since run in seconds
	std::chrono::microseconds m_microSecPerFrame;

	/// \brief The kernel of the push state method
	void _PushState( IGameStateP _state );

	/// \brief Remove all finished states.
	/// \return true as long there is a not-finished state.
	bool ClearStack();

	void BuildDefaultConfig();
};