#include "opengl.hpp"
#include <iostream>
#include <cassert>

void LogGlError( const char* _pMessage )
{
	GLuint iError = glGetError();
	if( iError != GL_NO_ERROR )
	{
		std::cerr << _pMessage << ": ";
		switch(iError)
		{
			case GL_INVALID_ENUM: std::cerr << "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE: std::cerr << "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION: std::cerr << "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW: std::cerr << "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW: std::cerr << "GL_STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY: std::cerr << "GL_OUT_OF_MEMORY"; break;
			default: assert(false);	// If breaks here add specific error code above
		}
		std::cerr << '\n';
	}
}