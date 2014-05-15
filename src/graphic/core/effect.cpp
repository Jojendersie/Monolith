#include "effect.hpp"
#include "opengl.hpp"
#include "uniformbuffer.hpp"
#include "utilities\assert.hpp"

#include <cstdint>

#ifdef AUTO_SHADER_RELOAD
	#include "../../../dependencies/FileWatcher/FileWatcher.h"

	#include <unordered_set>
	#include <unordered_map>
#endif


namespace Graphic {

	static unsigned LoadShader( const char* _fileName, unsigned _shaderType )
	{
		if( !_fileName || *_fileName==0 ) return 0;
		unsigned shaderID = glCreateShader(_shaderType);
		if(!shaderID) {LOG_ERROR("Could not create a shader!"); return 0;}

		Assert(strstr(_fileName, "\\") == nullptr, "A shader path uses backslashes. Please avoid this, since Unix systems rely on forward slashes as path separators!");

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
		GL_CALL(glShaderSource, shaderID, 1, &sourceConst, nullptr);
		if (GL_CALL(glCompileShader, shaderID) == GLResult::FAILED)
			return 0;
		free( source );

		// Compilation success?
		GLint res;
		GL_CALL(glGetShaderiv, shaderID, GL_COMPILE_STATUS, &res);
		if( res == GL_TRUE )
		{
			LOG_LVL2("Successfully loaded shader: '" + std::string(_fileName) + "'");
		} else
		{
			GLsizei charsWritten  = 0;

			GL_CALL(glGetShaderiv, shaderID, GL_INFO_LOG_LENGTH, &res);
			char* log = (char *)malloc(res);
			GL_CALL(glGetShaderInfoLog, shaderID, res, &charsWritten, log);
			LOG_ERROR("Error in compiling the shader '" + std::string(_fileName) + "': " + log);
			free( log );

			return 0;
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
		m_programID = GL_RET_CALL(glCreateProgram);

		// Link
		GL_CALL(glAttachShader, m_programID, m_vertexShader);
		GL_CALL(glAttachShader, m_programID, m_pixelShader);
		GL_CALL(glLinkProgram, m_programID);

		// Check of linking the shader program worked
		GLint testResult;
		GL_CALL(glGetProgramiv, m_programID, GL_LINK_STATUS, &testResult);
		if (testResult == GL_FALSE) {
			char acInfoLog[512];
			GL_CALL(glGetProgramInfoLog, m_programID, 512, nullptr, acInfoLog);
			LOG_ERROR(std::string("Failed to build shader program:") + acInfoLog);
		}

#ifdef AUTO_SHADER_RELOAD
		AddToFileWatcher(_VSFile, "", _PSFile);
#endif
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
		m_programID = GL_RET_CALL(glCreateProgram);

		// Link
		GL_CALL(glAttachShader, m_programID, m_vertexShader);
		GL_CALL(glAttachShader, m_programID, m_geometryShader);
		GL_CALL(glAttachShader, m_programID, m_pixelShader);
		GL_CALL(glLinkProgram, m_programID);

		// Check of linking the shader program worked
		GLint testResult;
		GL_CALL(glGetProgramiv, m_programID, GL_LINK_STATUS, &testResult);
		if (testResult == GL_FALSE) {
			char acInfoLog[512];
			GL_CALL(glGetProgramInfoLog, m_programID, 512, nullptr, acInfoLog);
			LOG_ERROR(std::string("Failed to build shader program:") + acInfoLog);
		}


#ifdef AUTO_SHADER_RELOAD
		AddToFileWatcher(_VSFile, _GSFile, _PSFile);
#endif
	}

	// ********************************************************************* //
	Effect::~Effect()
	{
#ifdef AUTO_SHADER_RELOAD
		RemoveFromFileWatcher();
#endif

		GL_CALL(glDeleteProgram, m_programID);

		GL_CALL(glDeleteShader, m_vertexShader);
		GL_CALL(glDeleteShader, m_geometryShader);
		GL_CALL(glDeleteShader, m_pixelShader);
	}

#ifdef AUTO_SHADER_RELOAD

	class ShaderFileWatch : FW::FileWatchListener
	{
	public:

		/// Registers a new effect to the watcher to be notified when changes occur in its directory of interest.
		void RegisterEffect(const std::string& watchedDirectory, Effect* listeningEffect)
		{
			if (watchedDirectory == "")
				return;

			auto element = m_listeningEffects.emplace(watchedDirectory, std::unordered_set<Effect*>());
			if (!element.second) // New directory
				m_fileWatcher.addWatch(watchedDirectory, this);
			element.first->second.insert(listeningEffect);
		}

		/// Removes effect from all directories.
		void UnregisterEffect(Effect* effect)
		{
			for (auto dirEntry = m_listeningEffects.begin(); dirEntry != m_listeningEffects.end(); ++dirEntry)
			{
				dirEntry->second.erase(effect);
				if (dirEntry->second.empty())
				{
					m_fileWatcher.removeWatch(dirEntry->first);
					dirEntry = m_listeningEffects.erase(dirEntry);
					if (dirEntry == m_listeningEffects.end())
						break;
				}
			}
		}

		/// Updates intern filewatcher
		void Update()
		{
			m_fileWatcher.update();
		}

	private: 
		/// Broadcasts file changes to listening shaders.
		virtual void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action) override
		{
			if (action != FW::Actions::Modified)
				return;

			auto entry = m_listeningEffects.find(dir);
			Assert(entry != m_listeningEffects.end(), "Changed directory is not registered as watched directory!");
			for (Effect* effect : entry->second)
				effect->HandleChangedShaderFile(dir + filename);
		}

		FW::FileWatcher m_fileWatcher;
		std::unordered_map<std::string, std::unordered_set<Effect*>> m_listeningEffects;
	};

	static ShaderFileWatch s_fileChangeListener;

	void Effect::UpdateShaderFileWatcher()
	{
		s_fileChangeListener.Update();
	}

	void Effect::AddToFileWatcher(const std::string& _VSFile, const std::string& _GSFile, const std::string& _PSFile)
	{
		m_OriginalVSFile = _VSFile;
		m_OriginalGSFile = _GSFile;
		m_OriginalPSFile = _PSFile;

		s_fileChangeListener.RegisterEffect(_VSFile.substr(0, _VSFile.find_last_of('/') + 1), this);
		s_fileChangeListener.RegisterEffect(_GSFile.substr(0, _GSFile.find_last_of('/') + 1), this);
		s_fileChangeListener.RegisterEffect(_PSFile.substr(0, _PSFile.find_last_of('/') + 1), this);
	}

	void Effect::RemoveFromFileWatcher()
	{
		s_fileChangeListener.UnregisterEffect(this);
	}

	void Effect::HandleChangedShaderFile(const std::string& shaderFilename)
	{
		unsigned int shaderType = 0;
		if (shaderFilename == m_OriginalVSFile)
			shaderType = GL_VERTEX_SHADER;
		else if (shaderFilename == m_OriginalPSFile)
			shaderType = GL_FRAGMENT_SHADER;
		else if (shaderFilename == m_OriginalGSFile)
			shaderType = GL_GEOMETRY_SHADER;
		else
			return;


		unsigned int newShader = LoadShader(shaderFilename.c_str(), shaderType);
		if (newShader == 0)
		{
			LOG_ERROR("Failed to reload shader " + shaderFilename + " on change. Will keep old one");
			return;
		}

		unsigned int newProgramID = GL_RET_CALL(glCreateProgram);

		GL_CALL(glAttachShader, newProgramID, shaderType == GL_VERTEX_SHADER ? newShader : m_vertexShader);
		GL_CALL(glAttachShader, newProgramID, shaderType == GL_FRAGMENT_SHADER ? newShader : m_pixelShader);
		if (m_geometryShader != 0)
			GL_CALL(glAttachShader, newProgramID, shaderType == GL_GEOMETRY_SHADER ? newShader : m_geometryShader);

		if (GL_CALL(glLinkProgram, newProgramID) == GLResult::SUCCESS)
		{
			m_programID = newProgramID;

			switch (shaderType)
			{
			case GL_VERTEX_SHADER:
				GL_CALL(glDeleteShader, m_vertexShader);
				m_vertexShader = newShader;
				break;

			case GL_FRAGMENT_SHADER:
				GL_CALL(glDeleteShader, m_pixelShader);
				m_pixelShader = newShader;
				break;

			case GL_GEOMETRY_SHADER:
				GL_CALL(glDeleteShader, m_geometryShader);
				m_geometryShader = newShader;
				break;

			default:
				Assert(true, "Incomplete shader auto reload implementation for the given shader type!");
			};

			// Rebind UBOs
			auto oldBindings = m_boundUniformBuffers;
			m_boundUniformBuffers.clear();
			for (auto uniformBuffer : oldBindings)
				BindUniformBuffer(*uniformBuffer);

			// Rebind Textures
			auto oldTextures = m_boundTextures;
			m_boundTextures.clear();
			for (auto& textureBinding : oldTextures)
				BindTexture(textureBinding.first, textureBinding.second.location, *textureBinding.second.sampler);

			LOG_LVL0("Successfully reloaded the shader " + shaderFilename);
		}
		else
		{
			GL_CALL(glDeleteProgram, newProgramID);
			GL_CALL(glDeleteShader, newShader);
		}
	}

#endif

	// ********************************************************************* //
	void Effect::BindUniformBuffer( UniformBuffer& _uniformBuffer )
	{
		for( size_t i=0; i<m_boundUniformBuffers.size(); ++i )
			if( m_boundUniformBuffers[i] == &_uniformBuffer )
				LOG_LVL0("The constant buffer is already bound to the shader.");

		m_boundUniformBuffers.push_back( &_uniformBuffer );

		unsigned index = GL_RET_CALL(glGetUniformBlockIndex, m_programID, _uniformBuffer.GetName().c_str()); /// \todo Do this in bulk at loading. This a slow operation that can be avoided
		
		// Ignore the errors. There are shaders without the blocks
		if( index != GL_INVALID_INDEX )
			GL_CALL(glUniformBlockBinding, m_programID, index, _uniformBuffer.GetBufferBaseIndex());
	}

	// ********************************************************************* //
	void Effect::BindTexture(const std::string& _name, unsigned _location, const SamplerState& _sampler )
	{
		auto textureEntryIt = m_boundTextures.emplace(_name, SamplerStateBinding(_location, &_sampler));

		// First bind the texture
		if (textureEntryIt.second || textureEntryIt.first->second.location != _location) // Means: New or not yet set.
		{
			GL_CALL(glUseProgram, m_programID); /// \todo Often called redundant currently. Device should know which program is bound.
			GLint uniformLocation = GL_RET_CALL(glGetUniformLocation, m_programID, _name.c_str());
			LogGlError("[Effect::BindTexture] Uniform location not found");

			GL_CALL(glUniform1i, uniformLocation, _location);
			LogGlError("[Effect::BindTexture] Failed to set the uniform sampler variable.");

			textureEntryIt.first->second.location = _location;
		}

		// Now store or overwrite the sampler state binding.
		if (textureEntryIt.second || textureEntryIt.first->second.sampler != &_sampler) // Means: New or not yet set.
		{
			textureEntryIt.first->second.sampler = &_sampler;
		}
	}

	// ********************************************************************* //
	void Effect::CommitUniformBuffers() const
	{
		for( size_t i=0; i<m_boundUniformBuffers.size(); ++i )
			m_boundUniformBuffers[i]->Commit();
	}

};