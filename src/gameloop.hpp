#pragma once

#include "timer.hpp"

#include <cstdint>
#include <string>
#include <mutex>

/// \brief General base class which allows different types of continuous loop
///		updates.
class GameLoop
{
public:
	/// \brief Create a game loop.
	/// \param [in] _duration Duration of a frame in seconds.
	///		To get a continuous loop set the duration to 0.
	GameLoop( const std::string& _name, double _targetFrameDuration ) :
		m_name(_name),
		m_targetFrameDuration(_targetFrameDuration),
		m_stepCounter(0),
		m_pause(false)
	{
	}

	/// \brief The method is called once per frame _deltaTime
	/// \param [in] _deltaTime Time since last Step call in seconds.
	virtual void Step( double _deltaTime ) = 0;

	/// \brief Set the rate to which this loop should be clamped in case the
	///		work is below that time.
	/// \details To get a continuous loop set the duration to 0.
	/// \param [in] _duration Duration of a frame in seconds.
	void SetTargetFrameDuration( double _duration );

	/// \brief Start the loop. This call is locking.
	/// \details For a multi-threaded application first spawn a thread which
	///		calls Run().
	void Run();

	/// \brief Stop all running game loops.
	/// \details The loops will continue up to the end of the current frame.
	static void StopAll();

	/// \brief Stop another thread temporarily. The call is blocking until the
	///		thread stopped.
	void Pause();
	/// \brief Continue a thread which was interrupted by Pause()
	void Continue();

	/// \brief If an uncaught exception occurs the OnFailure method is called,
	///		then the loop is continued.
	virtual void OnFailure() = 0;

	const std::string& Name() const { return m_name; }

private:
	std::string m_name;				///< Name of the loop for debug messages
	uint64_t m_stepCounter;			///< Measure time in frames
	double m_targetFrameDuration;	///< At what target frame rate this loop should run?

	std::recursive_mutex m_executing;			///< Locked as long as the thread works
	bool m_pause;					///< True if the thread should go into pause modus
};