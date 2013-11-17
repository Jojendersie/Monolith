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
			default: assert(false);	// If breaks here add specific error code above
		}
		std::cerr << '\n';
	}
}