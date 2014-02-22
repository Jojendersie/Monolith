#include "input.hpp"
#include "../opengl.hpp"
#include "../gamestates/gamestatebase.hpp"

using namespace std;

namespace Input {

	static Manager InputManagerInstance;

	// ********************************************************************* //
	void Manager::Initialize( GLFWwindow* _window, Jo::Files::MetaFileWrapper::Node& _keyConfig )
	{
		// GLFW setup
		glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );	// TODO use internal/no cursor
		glfwSetInputMode( _window, GLFW_STICKY_KEYS, GL_FALSE );
		glfwSetInputMode( _window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE );

		// Callback setup
		glfwSetCursorPosCallback( _window, CursorPosFun );
		glfwSetCursorEnterCallback( _window, CursorEnterFun );
		glfwSetScrollCallback( _window, ScrollFun );

		// Avoid invalid pointers
		InputManagerInstance.m_gameState = nullptr;
		InputManagerInstance.m_window = _window;

		// Read key mapping
		InputManagerInstance.m_keyMap = new Jo::Files::MetaFileWrapper::Node*[3];
		InputManagerInstance.m_keyMap[(int)Keys::MOVE_CAMERA] = &_keyConfig[string("MoveCamera")];
		InputManagerInstance.m_keyMap[(int)Keys::ROTATE_CAMERA] = &_keyConfig[string("RotateCamera")];
		InputManagerInstance.m_keyMap[(int)Keys::ZOOM] = &_keyConfig[string("Zoom")];
	}

	// ********************************************************************* //
	void Manager::Close()
	{
		delete[] InputManagerInstance.m_keyMap;
	}

	// ********************************************************************* //
	void Manager::SetGameState( IGameState* _gameState )
	{
		InputManagerInstance.m_gameState = _gameState;
	}

	// ********************************************************************* //
	void Manager::Update()
	{
		if( InputManagerInstance.m_gameState )
			InputManagerInstance.m_gameState->UpdateInput();
	}

	// ********************************************************************* //
	bool Manager::IsKeyPressed( Keys _key )
	{
		Jo::Files::MetaFileWrapper::Node& currentKeys = *InputManagerInstance.m_keyMap[(int)_key];
		for( int i=0; i<currentKeys.Size(); ++i ) {
			int key = (int)currentKeys[i];
			bool isPressed;
			if( key < 32 )
				isPressed = GLFW_PRESS == glfwGetMouseButton( InputManagerInstance.m_window, key );
			else
				isPressed = GLFW_PRESS == glfwGetKey( InputManagerInstance.m_window, key );
			// Stop if any of the buttons from the list is pressed
			if( isPressed ) return true;
		}
		return false;
	}

	// ********************************************************************* //
	void Manager::CursorPosFun(GLFWwindow*, double _x, double _y)
	{
		// Leave/enter events are handled by m_justEntered (reset position without jump)
		// Compute change of cursor and update its position
		double dx = _x - InputManagerInstance.m_cursorX;
		double dy = _y - InputManagerInstance.m_cursorY;
		if( InputManagerInstance.m_justEntered ) dx = 0.0, dy = 0.0;
		InputManagerInstance.m_cursorX = _x;
		InputManagerInstance.m_cursorY = _y;
		InputManagerInstance.m_justEntered = false;
		// Do a mouse move event.
		if( InputManagerInstance.m_gameState )
			InputManagerInstance.m_gameState->MouseMove( dx, dy );
	}

	// ********************************************************************* //
	void Manager::CursorEnterFun(GLFWwindow *, int _entered)
	{
		if( _entered == GL_FALSE )
			InputManagerInstance.m_justEntered = true;
	}

	// ********************************************************************* //
	void Manager::ScrollFun(GLFWwindow *, double _dx, double _dy)
	{
		InputManagerInstance.m_gameState->Scroll( _dx, _dy );
	}

} // namespace Input