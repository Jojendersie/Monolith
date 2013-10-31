#pragma once

#define GLEW_STATIC
#define GLFW_INCLUDE_GLU

#include "../dependencies/GLEW-1.10.0/include/GL/glew.h"
#include "../dependencies/glfw-3.0.3/include/GLFW/glfw3.h"
//#include "../dependencies/freeglut/include/GL/freeglut.h"

/// \brief Calls glGetError() and reports the message and the error code
///		if there is an error.
void LogGlError( const char* _pMessage );