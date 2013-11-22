#pragma once

#include "../predeclarations.hpp"

namespace Input {

	/// \brief The input manager passes inputs to the current set gamestate
	///		and manages some input state options.
	/// \details The manager must be singleton to allow correct callback setup.
	class Manager
	{
	public:
		/// \brief Initializes callbacks and settings for GLFW
		/// \details This input manager only allows single window input systems.
		static void Initialize( GLFWwindow* _window );

		/// \brief Switch which gamestate gets the input.
		/// \param [in] _gameState null to disable input handling or a
		///		gamestate which should recive the input events.
		static void SetGameState( IGameState* _gameState );

		/// \brief Do a synchronised update of things like the camera.
		static void Update();

		/// \brief Test if a key or button is currently pressed.
		static bool IsKeyPressed( Keys _key );
	private:
		IGameState* m_gameState;
		GLFWwindow* m_window;		///< The one reference window

		double m_cursorX;	///< Last known position of the cursor
		double m_cursorY;	///< Last known position of the cursor
		bool m_justEntered;	///< Has the cursor (re)entered the window scince last mouse move.

		static void CursorPosFun(GLFWwindow *, double, double);
		static void CursorEnterFun(GLFWwindow *, int);
		static void ScrollFun(GLFWwindow *, double, double);
	};

	// Ingame hard codes key. They are mapped by a configuration file. (TODO)
	// It is even possible that the keys are mapped to more than one key.
	enum struct Keys {
		MOVE_CAMERA,
		ROTATE_CAMERA,
		ZOOM
	};

} // namespace Input