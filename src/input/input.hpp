#pragma once

#include "../predeclarations.hpp"
#include "../../dependencies/glfw-3.0.3/include/GLFW/glfw3.h"
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

		/// \brief Test if a  GLFW key or button is currently pressed.
		static bool IsKeyPressed( int _key );

		/// \brief Test if a mapped key or button is currently pressed.
		static bool IsVirtualKeyPressed( VirtualKey _key );
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
		static void KeyFun(GLFWwindow* _window, int _key, int _scanCode, int _action, int _modifiers);
		static void MouseButtonFun(GLFWwindow*, int _key, int _action, int _modifiers);
	};

	// Ingame hard coded keys. They are mapped by a configuration file.
	// It is even possible that the keys are mapped to more than one key.
	enum struct VirtualKey {
		MOVE_CAMERA,
		ROTATE_CAMERA,
		ZOOM
	};

	/// \brief Takes the glfw input and converts it to a printable character
	///		if possible.
	///	\details Currently this is a german keyboard implementation. As soon as
	///		glfw supports converting keys this will be used instead.
	/// \return The ASCII character for the pressed key+modifiers or 0 if it
	///		was a function key.
	char KeyToChar( int _key, int _modifiers );
} // namespace Input