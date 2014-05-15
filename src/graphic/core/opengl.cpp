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

namespace Details
{
	GLResult CheckGLError(const char* openGLFunctionName)
	{
		GLenum Error = glGetError();
		if (Error != GL_NO_ERROR)
		{
			const char* errorString;
			const char* description;
			switch (Error)
			{
			case GL_INVALID_ENUM:
				errorString = "GL_INVALID_ENUM";
				description = "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
				break;

			case GL_INVALID_VALUE:
				errorString = "GL_INVALID_VALUE";
				description = "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
				break;

			case GL_INVALID_OPERATION:
				errorString = "GL_INVALID_OPERATION";
				description = "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
				description = "The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete. "
					"The offending command is ignored and has no other side effect than to set the error flag.";
				break;

			case GL_OUT_OF_MEMORY:
				errorString = "GL_OUT_OF_MEMORY";
				description = "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
				break;

			default:
				errorString = "UNKNOWN";
				description = "Unknown error code.";
				break;
			}
			LOG_ERROR(std::string("OpenGL Error during ") + openGLFunctionName + ": " + errorString + "(" + description + ")");
			return GLResult::FAILED;
		}

		return GLResult::SUCCESS;
	}

	bool CheckGLFunctionExistsAndReport(const char* openGLFunctionName, const void* fkt)
	{
		if (fkt != nullptr)
			return true;
		else
		{
			LOG_ERROR(std::string("OpenGL operation ") + openGLFunctionName + " is not available, the function is nullptr!");
			return false;
		}
	};

}