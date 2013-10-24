#include "effect.hpp"
#include "../opengl.hpp"
#include <iostream>
#include <cstdint>

namespace Graphic {

	static unsigned LoadShader( const char* _sFile, unsigned _iShaderType )
	{
		unsigned iShaderID = glCreateShader(_iShaderType);
		if(!iShaderID) {std::cout << "[LoadShader] Could not create a shader!\n"; return 0;}

		// Read in file
		FILE* pFile = fopen( _sFile, "rb" );
		if( !pFile ) {std::cout << "[LoadShader] Could not find file '" << _sFile << "'\n"; glDeleteShader(iShaderID); return 0;}
		fseek( pFile, 0, SEEK_END );
		long iSize = ftell( pFile );
		uint8_t* pSource = (uint8_t*)malloc(iSize);
		fseek( pFile, 0, SEEK_SET );
		fread( pSource, iSize, 1, pFile );
		fclose( pFile );

		// Compile
		const GLchar* pSourceConst = (GLchar*)pSource;
		glShaderSource(iShaderID, 1, &pSourceConst, nullptr);
		glCompileShader(iShaderID);
		free( pSource );

		// Compilation success?
		GLint iRes;
		glGetShaderiv( iShaderID, GL_COMPILE_STATUS, &iRes );
		if( iRes == GL_TRUE )
		{
			std::cout << "[LoadShader] Successfully loaded shader: '" << _sFile << "'\n";
		} else
		{
			GLsizei charsWritten  = 0;

			glGetShaderiv( iShaderID, GL_INFO_LOG_LENGTH, &iRes );
			char* pLog = (char *)malloc(iRes);
			glGetShaderInfoLog( iShaderID, iRes, &charsWritten, pLog );
			std::cout << "[LoadShader] Error in compiling the shader '" << _sFile << "': " << pLog << '\n';
			free( pLog );
		}

		return iShaderID;
	}

	Effect::Effect( const char* _sVSFile, const char* _sGSFile, const char* _sPSFile,
			RasterizerState::CULL_MODE _CullMode, RasterizerState::FILL_MODE _FillMode ) :
		m_RasterizerState(_CullMode, _FillMode),
	//	m_SamplerState(),
		m_BlendState(BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::ONE, BlendState::BLEND::ZERO ),
		m_DepthStencilState(DepthStencilState::COMPARISON_FUNC::LESS, true)
	{
		m_iVertexShader = LoadShader( _sVSFile, GL_VERTEX_SHADER );
		m_iGeometryShader = LoadShader( _sVSFile, GL_FRAGMENT_SHADER );
		m_iPixelShader = LoadShader( _sVSFile, GL_GEOMETRY_SHADER );
		if( m_iVertexShader == 0 || m_iGeometryShader == 0 || m_iPixelShader == 0 )
		{
			std::cout << "[Effect::Effect] One or more shaders were not loaded. Effect will be wrong.\n";
			return;
		}

		// Create new program
		m_iProgramID = glCreateProgram();

		// Link
		glAttachShader(m_iProgramID, m_iVertexShader);
		glAttachShader(m_iProgramID, m_iGeometryShader);
		glAttachShader(m_iProgramID, m_iPixelShader);
		glLinkProgram(m_iProgramID);

		// Check of linking the shader program worked
		GLint iTestResult;
		glGetProgramiv(m_iProgramID, GL_LINK_STATUS, &iTestResult);
		if (iTestResult == GL_FALSE) {
			char acInfoLog[512];
			glGetProgramInfoLog(m_iProgramID, 512, 0, acInfoLog);
			std::cout << "[Effect::Effect] Failed to build shader program:\n" << acInfoLog << '\n';
		}
	}

	Effect::~Effect()
	{
		glDeleteProgram(m_iProgramID);

		glDeleteShader(m_iVertexShader);
		glDeleteShader(m_iGeometryShader);
		glDeleteShader(m_iPixelShader);
	}

};