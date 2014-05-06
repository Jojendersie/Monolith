#include "opengl.hpp"
#include <sstream>
#include "utilities/assert.hpp"
#ifndef LOG_NO_LOCALIZATION
#	define LOG_NO_LOCALIZATION
#endif
#include "../../utilities/logger.hpp"

void LogGlError( const char* _pMessage )
{
	GLuint iError = glGetError();
	if( iError != GL_NO_ERROR )
	{
		std::stringstream message;
		message << _pMessage << ": ";
		switch(iError)
		{
			case GL_INVALID_ENUM: message << "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE: message << "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION: message << "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW: message << "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW: message << "GL_STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY: message << "GL_OUT_OF_MEMORY"; break;
			default: Assert(false, "Unknown GL error.");	// If breaks here add specific error code above
		}
		LOG_ERROR(message.str());
	}
}