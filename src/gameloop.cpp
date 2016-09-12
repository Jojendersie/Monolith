#include "gameloop.hpp"
#include "utilities/logger.hpp"
#include <ei/elementarytypes.hpp>
#include <thread>

using namespace ei;

static bool g_stopLoops = false;

// ************************************************************************* //
void GameLoop::SetTargetFrameDuration( double _duration )
{
	m_targetFrameDuration = _duration;
}

// ************************************************************************* //
void GameLoop::Run()
{
	LOG_LVL2( "Starting game loop " + m_name );
	double deltaTime = 0.0;
	TimeQuerySlot frameTimer;
	while( !g_stopLoops )
	{
		{	// artificial scope for the lock
			std::unique_lock<std::recursive_mutex> lock(m_executing);
			// Busy waiting?
			while( m_pause )
			{
				if( g_stopLoops ) return;
				std::this_thread::sleep_for( std::chrono::microseconds(50) );
			}

			TimeQuery( frameTimer );

			// Do the work
			try {
				Step( deltaTime );
			} catch( const std::exception& _e ) {
				LOG_ERROR("Caught std::exception in game loop " + m_name + ": \"" + std::string(_e.what()) + "\"");
				OnFailure();
			} catch( const std::string& _e ) {
				LOG_ERROR("Caught string-exception in game loop " + m_name + ": \"" + _e + "\"");
				OnFailure();
			} catch( ... ) {
				LOG_ERROR("Caught exception of unknown type in game loop " + m_name);
				OnFailure();
			}
		}

		++m_stepCounter;

		// Calculate time since last frame
		double deltaFrameTime = TimeQuery( frameTimer );
		// Smooth frame time
//		deltaTime = deltaTime * 0.8 + deltaFrameTime * 0.2;
//		deltaTime = max( deltaFrameTime, m_targetFrameDuration );
		// Because of sleeping the time of a frame always takes the desired
		// length. If it takes longer the game will run slower, but to large
		// steps make many things too instable.
		deltaTime = m_targetFrameDuration;

		// Limiting to target fps
		double timeDifference = m_targetFrameDuration - deltaFrameTime;
		//timeDifference = max(0.0, timeDifference);
		if( timeDifference > 0.0 )
		{
			std::this_thread::sleep_for( std::chrono::microseconds(unsigned(timeDifference * 1000000.0)) );
		}
	}

	LOG_LVL2( "Exited game loop " + m_name );
}

// ************************************************************************* //
void GameLoop::StopAll()
{
	g_stopLoops = true;
}

// ************************************************************************* //
void GameLoop::Pause()
{
	// Lock until work is finished
	std::unique_lock<std::recursive_mutex> lock(m_executing);
	// The next time the loop will go into work lock it sees the pause flag set.
	m_pause = true;
}

// ************************************************************************* //
void GameLoop::Continue()
{
	m_pause = false;
}