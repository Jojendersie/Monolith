#include "effect.hpp"
#include "../opengl.hpp"
#include <iostream>
#include <cstdint>

namespace Graphic {

	static unsigned LoadShader( const char* _fileName, unsigned _shaderType )
	{
		if( !_fileName || *_fileName==0 ) return 0;
		unsigned shaderID = glCreateShader(_shaderType);
		if(!shaderID) {std::cout << "[LoadShader] Could not create a shader!\n"; return 0;}

		// Read in file
		FILE* file = fopen( _fileName, "rb" );
		if( !file ) {std::cout << "[LoadShader] Could not find file '" << _fileName << "'\n"; glDeleteShader(shaderID); return 0;}
		fseek( file, 0, SEEK_END );
		long size = ftell( file );
		uint8_t* source = (uint8_t*)malloc(size+1);
		fseek( file, 0, SEEK_SET );
		fread( source, size, 1, file );
		fclose( file );
		source[size] = 0;

		// Compile
		const GLchar* sourceConst = (GLchar*)source;
		glShaderSource(shaderID, 1, &sourceConst, nullptr);
		glCompileShader(shaderID);
		free( source );

		// Compilation success?
		GLint res;
		glGetShaderiv( shaderID, GL_COMPILE_STATUS, &res );
		if( res == GL_TRUE )
		{
			std::cout << "[LoadShader] Successfully loaded shader: '" << _fileName << "'\n";
		} else
		{
			GLsizei charsWritten  = 0;

			glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &res );
			char* log = (char *)malloc(res);
			glGetShaderInfoLog( shaderID, res, &charsWritten, log );
			std::cout << "[LoadShader] Error in compiling the shader '" << _fileName << "': " << log << '\n';
			free( log );
		}

		return shaderID;
	}

	Effect::Effect( const std::string& _VSFile, const std::string& _GSFile, const std::string& _PSFile,
			RasterizerState::CULL_MODE _cullMode, RasterizerState::FILL_MODE _fillMode ) :
		m_rasterizerState(_cullMode, _fillMode),
	//	m_SamplerState(),
		m_blendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::ONE, BlendState::BLEND::ZERO ),
		m_depthStencilState(DepthStencilState::COMPARISON_FUNC::LESS, true)
	{
		m_vertexShader = LoadShader( _VSFile.c_str(), GL_VERTEX_SHADER );
		m_geometryShader = LoadShader( _GSFile.c_str(), GL_GEOMETRY_SHADER );
		m_pixelShader = LoadShader( _PSFile.c_str(), GL_FRAGMENT_SHADER );
		if( m_vertexShader == 0 || m_geometryShader == 0 || m_pixelShader == 0 )
		{
			std::cout << "[Effect::Effect] One or more shaders were not loaded. Effect will be wrong.\n";
			return;
		}

		// Create new program
		m_programID = glCreateProgram();

		// Link
		glAttachShader(m_programID, m_vertexShader);
		glAttachShader(m_programID, m_geometryShader);
		glAttachShader(m_programID, m_pixelShader);
		glLinkProgram(m_programID);

		// Check of linking the shader program worked
		GLint testResult;
		glGetProgramiv(m_programID, GL_LINK_STATUS, &testResult);
		if (testResult == GL_FALSE) {
			char acInfoLog[512];
			glGetProgramInfoLog(m_programID, 512, 0, acInfoLog);
			std::cout << "[Effect::Effect] Failed to build shader program:\n" << acInfoLog << '\n';
		}
	}

	Effect::~Effect()
	{
		glDeleteProgram(m_programID);

		glDeleteShader(m_vertexShader);
		glDeleteShader(m_geometryShader);
		glDeleteShader(m_pixelShader);
	}

};