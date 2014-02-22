#pragma once

#include "../predeclarations.hpp"
#include <jofilelib.hpp>

namespace Input {

	/// \brief The input manager passes inputs to the current set game state
	///		and manages some input state options.
	/// \details The manager must be singleton to allow correct callback setup.
	class Manager
	{
	public:
		/// \brief Initializes callbacks and settings for GLFW
		/// \details This input manager only allows single window input systems.
		static void Initialize( GLFWwindow* _window, Jo::Files::MetaFileWrapper::Node& _keyConfig );

		/// \brief Release all resources
		static void Close();

		/// \brief Switch which game state gets the input.
		/// \param [in] _gameState null to disable input handling or a
		///		game state which should receive the input events.
		static void SetGameState( IGameState* _gameState );

		/// \brief Do a synchronized update of things like the camera.
		static void Update();

		/// \brief Test if a key or button is currently pressed.
		static bool IsKeyPressed( Keys _key );
	private:
		IGameState* m_gameState;
		GLFWwindow* m_window;		///< The one reference window
		Jo::Files::MetaFileWrapper::Node** m_keyMap;		///< Faster access to the correct nodes of the config file.

		double m_cursorX;	///< Last known position of the cursor
		double m_cursorY;	///< Last known position of the cursor
		bool m_justEntered;	///< Has the cursor (re)entered the window since last mouse move.

		static void CursorPosFun(GLFWwindow *, double, double);
		static void CursorEnterFun(GLFWwindow *, int);
		static void ScrollFun(GLFWwindow *, double, double);
	};

	// Ingame hard coded keys. They are mapped by a configuration file.
	// It is even possible that the keys are mapped to more than one key.
	enum struct Keys {
		MOVE_CAMERA,
		ROTATE_CAMERA,
		ZOOM
	};

} // namespace Input