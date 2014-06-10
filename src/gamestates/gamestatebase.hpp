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
	/// \param [in] _deltaTime Time since last Update call.
	virtual void Simulate( double _deltaTime ) = 0;

	/// \brief Render one frame.
	/// \param [in] _deltaTime Time since last Render call.
	virtual void Render( double _deltaTime ) = 0;

	/// \brief React to mouse move input.
	/// \details Do not update anything accessed by the renderer. Try to
	///		deferr them till UpdateInput().
	/// \param [in] _dx Delta x. Change of position in x direction
	virtual void MouseMove( double _dx, double _dy ) = 0;

	/// \brief handle vertical and horizontal scroll events.
	virtual void Scroll( double _dx, double _dy ) {}

	/// \brief Called if any keyboard or mouse button is pushed down.
	/// \details The key values are the same as from glew (e.g. GLFW_KEY_UP)
	///		and can be GLEW_KEY_ or GLEW_MOUSE_ respectively. A keyboard repeat
	///		will call down again (even if already pushed).
	///		
	///		To convert keyboard input to characters use the function
	///		Input::KeyToChar.
	///	\param [in] _modifiers GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT
	///		or GLFW_MOD_SUPER in any combination.
	virtual void KeyDown( int _key, int _modifiers ) {}

	/// \brief Called if any keyboard or mouse button is released.
	/// \copydetails KeyDown()
	virtual void KeyRelease( int _key ) {}

	/// \brief Called if any keyboard or mouse button is pushed and released
	///		in a short interval.
	/// \copydetails KeyDown()
	///		
	///		If holding a button for a time longer than 0.15s no click is
	///		induced. For buttons in a gui KeyRelease() may be a better choice.
	///		The click event is sent after the release event.
	virtual void KeyClick( int _key ) {}

	/// \brief Called if any keyboard or mouse button is pushed and released
	///		twice in a short interval.
	/// \copydetails KeyDown()
	///		
	///		The double click is induced if the click condition (<0.15s between
	///		push and release) is met twice in an interval of 0.25s and the
	///		mouse cursor did not move to far.
	virtual void KeyDoubleClick( int _key ) {}

	/// \brief Return the parent game state on stack or nullptr.
	IGameState* ParentGameState() const		{ return m_previous; }

	/// \brief Game states can finish them self. Test if this one did so.
	bool IsFinished() const		{ return m_finished; }
protected:
	Monolith* m_game;

//	void Finish() { m_finished = true;}
	bool m_finished;		///< The game state is finished and gets closed next frame

private:
	/// \brief Stack of game states where each can access its parent
	IGameState* m_previous;
	friend class Monolith;
};