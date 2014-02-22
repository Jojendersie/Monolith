#pragma once

#include "../predeclarations.hpp"

/// \brief A game state is an instance with a number of handling methods which
///		are called by the main loop.
class IGameState
{
public:
	IGameState(Monolith* _parent) : m_parent(_parent) {}

	virtual ~IGameState() {}

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

protected:
	Monolith* m_parent;
};