#include "effect.hpp"
#include "../opengl.hpp"
#include "uniformbuffer.hpp"
#include <cstdint>

namespace Graphic {

	static unsigned LoadShader( const char* _fileName, unsigned _shaderType )
	{
		if( !_fileName || *_fileName==0 ) return 0;
		unsigned shaderID = glCreateShader(_shaderType);
		if(!shaderID) {LOG_ERROR("Could not create a shader!"); return 0;}

		// Read in file
		FILE* file = fopen( _fileName, "rb" );
		if( !file ) {LOG_ERROR("Could not find file '" + std::string(_fileName) + "'"); glDeleteShader(shaderID); return 0;}
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
			LOG_LVL2("Successfully loaded shader: '" + std::string(_fileName) + "'");
		} else
		{
			GLsizei charsWritten  = 0;

			glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &res );
			char* log = (char *)malloc(res);
			glGetShaderInfoLog( shaderID, res, &charsWritten, log );
			LOG_ERROR("Error in compiling the shader '" + std::string(_fileName) + "': " + log);
			free( log );
		}

		return shaderID;
	}

	// ********************************************************************* //
	Effect::Effect( const std::string& _VSFile, const std::string& _PSFile,
			RasterizerState::CULL_MODE _cullMode, RasterizerState::FILL_MODE _fillMode,
			BlendState::BLEND_OPERATION _blendOp, BlendState::BLEND _srcOp, BlendState::BLEND _dstOp,
			DepthStencilState::COMPARISON_FUNC _depthFunc, bool _zWrite ) :
		m_rasterizerState(_cullMode, _fillMode),
		m_blendState(_blendOp, _srcOp, _dstOp ),
		m_depthStencilState(_depthFunc, _zWrite)
	{
		m_vertexShader = LoadShader( _VSFile.c_str(), GL_VERTEX_SHADER );
		m_geometryShader = 0;
		m_pixelShader = LoadShader( _PSFile.c_str(), GL_FRAGMENT_SHADER );
		if( m_vertexShader == 0 || m_pixelShader == 0 )
		{
			LOG_ERROR("One or more shaders were not loaded. Effect will be wrong.");
			return;
		}

		// Create new program
		m_programID = glCreateProgram();

		// Link
		glAttachShader(m_programID, m_vertexShader);
		glAttachShader(m_programID, m_pixelShader);
		glLinkProgram(m_programID);

		// Check of linking the shader program worked
		GLint testResult;
		glGetProgramiv(m_programID, GL_LINK_STATUS, &testResult);
		if (testResult == GL_FALSE) {
			char acInfoLog[512];
			glGetProgramInfoLog(m_programID, 512, 0, acInfoLog);
			LOG_ERROR(std::string("Failed to build shader program:") + acInfoLog);
		}
	}

	// ********************************************************************* //
	Effect::Effect( const std::string& _VSFile, const std::string& _GSFile, const std::string& _PSFile,
			RasterizerState::CULL_MODE _cullMode, RasterizerState::FILL_MODE _fillMode,
			BlendState::BLEND_OPERATION _blendOp, BlendState::BLEND _srcOp, BlendState::BLEND _dstOp,
			DepthStencilState::COMPARISON_FUNC _depthFunc, bool _zWrite ) :
		m_rasterizerState(_cullMode, _fillMode),
		m_blendState(_blendOp, _srcOp, _dstOp ),
		m_depthStencilState(_depthFunc, _zWrite)
	{
		m_vertexShader = LoadShader( _VSFile.c_str(), GL_VERTEX_SHADER );
		m_geometryShader = LoadShader( _GSFile.c_str(), GL_GEOMETRY_SHADER );
		m_pixelShader = LoadShader( _PSFile.c_str(), GL_FRAGMENT_SHADER );
		if( m_vertexShader == 0 || m_geometryShader == 0 || m_pixelShader == 0 )
		{
			LOG_ERROR("One or more shaders were not loaded. Effect will be wrong.");
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
			LOG_ERROR(std::string("Failed to build shader program:") + acInfoLog);
		}
	}

	// ********************************************************************* //
	Effect::~Effect()
	{
		glDeleteProgram(m_programID);

		glDeleteShader(m_vertexShader);
		glDeleteShader(m_geometryShader);
		glDeleteShader(m_pixelShader);
	}

	// ********************************************************************* //
	void Effect::BindUniformBuffer( UniformBuffer& _uniformBuffer )
	{
		for( size_t i=0; i<m_boundUniformBuffers.size(); ++i )
			if( m_boundUniformBuffers[i] == &_uniformBuffer )
				LOG_LVL0("The constant buffer is already bound to the shader.");

		m_boundUniformBuffers.push_back( &_uniformBuffer );

		unsigned index = glGetUniformBlockIndex(m_programID, _uniformBuffer.GetName().c_str());
		// Ignore the errors. There are shaders without the blocks
		if( !glGetError() && index!=GL_INVALID_INDEX ) {
			glUniformBlockBinding(m_programID, index, _uniformBuffer.GetBufferBaseIndex());
		}
	}

	// ********************************************************************* //
	void Effect::BindTexture( const char* _name, unsigned _location, const SamplerState& _sampler )
	{
		// First bind the texture
		glUseProgram( m_programID );
		GLint uniformLocation = glGetUniformLocation(m_programID, _name);
		LogGlError("[Effect::BindTexture] Uniform location not found");

		glUniform1i(uniformLocation, _location);
		LogGlError("[Effect::BindTexture] Failed to set the uniform sampler variable.");

		// Now store or overwrite the sampler state binding.
		for( size_t i=0; i<m_samplerStates.size(); ++i )
			if( m_samplerStates[i].location == _location ) {
				m_samplerStates[i].sampler = &_sampler;
				return;
			}
		SamplerStateBinding newBinding;
		newBinding.location = _location;
		newBinding.sampler = &_sampler;
		m_samplerStates.push_back(newBinding);
	}

	// ********************************************************************* //
	void Effect::CommitUniformBuffers() const
	{
		for( size_t i=0; i<m_boundUniformBuffers.size(); ++i )
			m_boundUniformBuffers[i]->Commit();
	}

};