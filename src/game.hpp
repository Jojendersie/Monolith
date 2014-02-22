#pragma once

#include <chrono>
#include "predeclarations.hpp"
#include <jofilelib.hpp>



/// \brief The game consists of a main-loop and several different game-states.
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

	Graphic::Content* m_graficContent;

	Jo::Files::MetaFileWrapper Config;
private:
	IGameStateP m_gameStates[1];	///< MainMenu, NewGame, Main, 
	int m_currentGameState;
	bool m_singleThreaded;
	bool m_running;

	double m_time;					///< Total time since run in seconds
	std::chrono::microseconds m_microSecPerFrame;

	void BuildDefaultConfig();
};