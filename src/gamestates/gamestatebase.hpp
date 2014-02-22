#pragma once

#include "../predeclarations.hpp"

/// \brief A game state is an instance with a number of handling methods which
///		are called by the main loop.
///	\details \see class Monolith which manages the game states for a description
///		of the general behavior.
class IGameState
{
public:
	IGameState(Monolith* _game) : m_game(_game), m_finished(false), m_previous(nullptr) {}

	virtual ~IGameState() {}

	/// \brief The game state must implement one OnBegin with 0 to 5 input
	///		parameters.
	// void OnBegin();

	/// \brief This method is called if a state on top of this one is finished
	virtual void OnResume()	{}

	/// \brief OnEnd should opposite OnBegin and remove all content
	///		loaded there.
	///	\details This is called before the moment where the state is removed
	///		from the stack.
	virtual void OnEnd() = 0;

	/// \brief Update the movement, damages... of all objects
	/// \param [in] _time Total time since game start in seconds.
	/// \param [in] _deltaTime Time since last Update call.
	virtual void Update( double _time, double _deltaTime ) = 0;

	/// \brief Render one frame.
	/// \param [in] _time Total time since game start in seconds.
	/// \param [in] _deltaTime Time since last Render call.
	virtual void Render( double _time, double _deltaTime ) = 0;

	// TODO: more Input

	/// \brief The input update is called synchronized with the renderer.
	/// \details This method must recompute camera matrices and other things
	///		which relay on input but are used in rendering too.
	virtual void UpdateInput() = 0;

	/// \brief React to mouse move input.
	/// \details Do not update anything accessed by the renderer. Try to
	///		deferr them till UpdateInput().
	/// \param [in] _dx Delta x. Change of position in x direction
	virtual void MouseMove( double _dx, double _dy ) = 0;

	/// \brief handle vertical and horizontal scroll events.
	virtual void Scroll( double _dx, double _dy ) {}

	/// \brief Called if any keyboard or mouse button is pushed down.
	// virtual void KeyDown( int ) = 0;

	/// \brief Return the parent game state on stack or nullptr.
	IGameState* ParentGameState() const		{ return m_previous; }

	/// \brief Game states can finish them self. Test if this one did so.
	bool IsFinished() const		{ return m_finished; }
protected:
	Monolith* m_game;
	bool m_finished;		///< The game state is finished and gets closed next frame

private:
	/// \brief Stack of game states where each can access its parent
	IGameState* m_previous;
	friend class Monolith;
};