#include "effect.hpp"
#include "opengl.hpp"
#include "uniformbuffer.hpp"
#include "utilities\assert.hpp"

#include "jofilelib.hpp"
#include "utilities/pathutils.hpp"

#include <cstdint>
#include <fstream>


#ifdef AUTO_SHADER_RELOAD
	#include "../../../dependencies/FileWatcher/FileWatcher.h"

	#include <unordered_set>
	#include <unordered_map>
#endif
#include <memory>


// Define this macro to safe all preprocessed shaders in the given location with generic file endings that are suitable for the glsllang parser
#define SHADER_CODE_DUMP_PATH "processedshaderdump/"


namespace Graphic {

	/// \param fileIndex	This will used as second parameter for each #line macro. It is a kind of file identifier.
	/// \remarks Uses a lot of potentially slow string operations.
	static std::string LoadShaderCodeFromFile(const std::string& _shaderFilename, const std::string& _prefixCode, std::unordered_set<std::string>& _includedFiles, unsigned int _fileIndex = 0)
	{
		// open file
		std::ifstream file(_shaderFilename.c_str());
		if (file.bad())
		{
			LOG_ERROR("Unable to open shader file " + _shaderFilename);
			return "";
		}

		// Reserve
		std::string sourceCode;
		file.seekg(0, std::ios::end);
		sourceCode.reserve(static_cast<size_t>(file.tellg()));
		file.seekg(0, std::ios::beg);

		// Read
		sourceCode.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		std::string insertionBuffer;
		size_t parseCursorPos = 0;
		size_t parseCursorOriginalFileNumber = 1;
		size_t versionPos = sourceCode.find("#version");

		// Add #line macro for proper error output (see http://stackoverflow.com/questions/18176321/is-line-0-valid-in-glsl)
		// The big problem: Officially you can only give a number as second argument, no shader filename 
		// Don't insert one if this is the main file, recognizable by a #version tag!
		if (versionPos == std::string::npos)
		{
			insertionBuffer = "#line 1 " + std::to_string(_fileIndex) + "\n";
			sourceCode.insert(0, insertionBuffer);
			parseCursorPos = insertionBuffer.size();
			parseCursorOriginalFileNumber = 1;
		}

		unsigned int lastFileIndex = _fileIndex;

		// Prefix code (optional)
		if (!_prefixCode.empty())
		{
			if (versionPos != std::string::npos)
			{
				// Insert after version and surround by #line macro for proper error output.
				size_t nextLineIdx = sourceCode.find_first_of("\n", versionPos);
				size_t numLinesOfPrefixCode = std::count(_prefixCode.begin(), _prefixCode.end(), '\n') + 1;
				size_t numLinesBeforeVersion = std::count(sourceCode.begin(), sourceCode.begin() + versionPos, '\n');

				insertionBuffer = "\n#line 1 " + std::to_string(++lastFileIndex) + "\n";
				insertionBuffer += _prefixCode;
				insertionBuffer += "\n#line " + std::to_string(numLinesBeforeVersion + 1) + " " + std::to_string(_fileIndex) + "\n";

				sourceCode.insert(nextLineIdx, insertionBuffer);

				// parse cursor moves on
				parseCursorPos = nextLineIdx + insertionBuffer.size(); // This is the main reason why currently no #include files in prefix code is supported. Changing these has a lot of side effects in line numbering!
				parseCursorOriginalFileNumber = numLinesBeforeVersion + 1; // jumped over #version!
			}
		}

		// push into file list to prevent circular includes
		_includedFiles.insert(_shaderFilename);

		// parse all include tags
		size_t includePos = sourceCode.find("#include", parseCursorPos);
		std::string relativePath = PathUtils::GetDirectory(_shaderFilename);
		while (includePos != std::string::npos)
		{
			parseCursorOriginalFileNumber += std::count(sourceCode.begin() + parseCursorPos, sourceCode.begin() + includePos, '\n');
			parseCursorPos = includePos;

			// parse filepath
			size_t quotMarksFirst = sourceCode.find("\"", includePos);
			if (quotMarksFirst == std::string::npos)
			{
				LOG_ERROR("Invalid #include directive in shader file " + _shaderFilename + ". Expected \"");
				break;
			}
			size_t quotMarksLast = sourceCode.find("\"", quotMarksFirst + 1);
			if (quotMarksFirst == std::string::npos)
			{
				LOG_ERROR("Invalid #include directive in shader file " + _shaderFilename + ". Expected \"");
				break;
			}

			size_t stringLength = quotMarksLast - quotMarksFirst - 1;
			if (stringLength == 0)
			{
				LOG_ERROR("Invalid #include directive in shader file " + _shaderFilename + ". Quotation marks empty!");
				break;
			}


			std::string includeCommand = sourceCode.substr(quotMarksFirst + 1, stringLength);
			std::string includeFile = PathUtils::AppendPath(relativePath, includeCommand);
			 
			// check if already included
			if (_includedFiles.find(includeFile) != _includedFiles.end())
			{
				sourceCode.replace(includePos, includePos - quotMarksLast + 1, "\n");
				// just do nothing...
			}
			else
			{
				parseCursorOriginalFileNumber++; // after the include!

				insertionBuffer = LoadShaderCodeFromFile(includeFile, "", _includedFiles, ++lastFileIndex);
				insertionBuffer += "\n#line " + std::to_string(parseCursorOriginalFileNumber) + " " + std::to_string(_fileIndex) + "\n"; // whitespace replaces #include!
				sourceCode.replace(includePos, quotMarksLast - includePos + 1, insertionBuffer);

				parseCursorPos += insertionBuffer.size();
			}

			// find next include
			includePos = sourceCode.find("#include", parseCursorPos);
		}

		return sourceCode;
	}

	/// \param includedFiles[out] Set of all included files.
	static unsigned LoadShader(const std::string& _fileName, unsigned _shaderType, const std::string& _prefixCode, std::unordered_set<std::string>& includedFiles)
	{
		unsigned shaderID = glCreateShader(_shaderType);
		if(!shaderID)
		{
			LOG_ERROR("Could not create a shader!");
			return 0;
		}

		// Read source code
		std::string sourceCode(LoadShaderCodeFromFile(_fileName, _prefixCode, includedFiles));
		if (sourceCode == "")
			return 0;

		// Optional shader dump for easier debugging and validating.
#ifdef SHADER_CODE_DUMP_PATH
		{
			std::string shaderEnding = "";
			if (_shaderType == GL_VERTEX_SHADER)
				shaderEnding = ".vert";
			else if (_shaderType == GL_FRAGMENT_SHADER)
				shaderEnding = ".frag";
			else if (_shaderType == GL_GEOMETRY_SHADER)
				shaderEnding = ".geom";
			else
				Assert(true, "SHADER_CODE_DUMP functionality does not know this shader type!");

			static unsigned int shaderUID = 0;
			std::string shaderDmpFilename = PathUtils::AppendPath(SHADER_CODE_DUMP_PATH, _fileName + "_" + std::to_string(++shaderUID) + shaderEnding);

			Jo::Files::HDDFile shaderDmp(shaderDmpFilename, Jo::Files::HDDFile::CREATE_FILE);
			shaderDmp.Write(sourceCode.c_str(), sourceCode.size());
		}
#endif

		// Add prefix after first

		// Compile
		const GLchar* sourceConst = static_cast<const GLchar*>(sourceCode.c_str());
		GL_CALL(glShaderSource, shaderID, 1, &sourceConst, nullptr);
		if (GL_CALL(glCompileShader, shaderID) == GLResult::FAILED)
			return 0;

		// Compilation success?
		GLint res;
		GL_CALL(glGetShaderiv, shaderID, GL_COMPILE_STATUS, &res);
		if( res == GL_TRUE )
		{
			LOG_LVL2("Successfully loaded shader: '" + std::string(_fileName) + "'");
		} else
		{
			GL_CALL(glGetShaderiv, shaderID, GL_INFO_LOG_LENGTH, &res);
			std::unique_ptr<char[]> log(new char[res + 1]);
			GL_CALL(glGetShaderInfoLog, shaderID, res, &res, log.get());
			log[res] = '\0'; // important for some gpu vendors!

			LOG_ERROR("Error in compiling shader: '" + _fileName + "': " + log.get());

			return 0;
		}

		return shaderID;
	}

	// ********************************************************************* //
	Effect::Effect(const std::string& _VSFile, const std::string& _PSFile, const std::string& _GSFile, const std::string& _prefixCode) :
		m_rasterizerState(RasterizerState::CULL_MODE::BACK, RasterizerState::FILL_MODE::SOLID),
		m_blendState(BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::ONE, BlendState::BLEND::ZERO),
		m_depthStencilState(DepthStencilState::COMPARISON_FUNC::LESS, true),
		

		m_vertexShader(0),
		m_pixelShader(0),
		m_geometryShader(0),
		m_programID(0),

		m_prefixCode(_prefixCode),
		m_VSMainFile(_VSFile),
		m_PSMainFile(_PSFile),
		m_GSMainFile(_GSFile)
	{
		m_vertexShader = LoadShader(_VSFile.c_str(), GL_VERTEX_SHADER, _prefixCode, m_VSFiles);
		if (!_GSFile.empty())
			m_geometryShader = LoadShader(_GSFile.c_str(), GL_GEOMETRY_SHADER, _prefixCode, m_GSFiles);
		m_pixelShader = LoadShader(_PSFile.c_str(), GL_FRAGMENT_SHADER, _prefixCode, m_PSFiles);
		if (m_vertexShader == 0 || (!_GSFile.empty() && m_geometryShader == 0) || m_pixelShader == 0)
		{
			LOG_ERROR("One or more shaders were not loaded. Effect will be wrong.");
			return;
		}

		// Create new program
		m_programID = GL_RET_CALL(glCreateProgram);

		// Link
		GL_CALL(glAttachShader, m_programID, m_vertexShader);
		if (!_GSFile.empty())
			GL_CALL(glAttachShader, m_programID, m_geometryShader);
		GL_CALL(glAttachShader, m_programID, m_pixelShader);
		GL_CALL(glLinkProgram, m_programID);

		// Check of linking the shader program worked
		CheckShaderProgramError();


#ifdef AUTO_SHADER_RELOAD
		AddToFileWatcher();
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
		if (m_geometryShader != 0)
			GL_CALL(glDeleteShader, m_geometryShader);
		GL_CALL(glDeleteShader, m_pixelShader);
	}

#ifdef AUTO_SHADER_RELOAD

	// ********************************************************************* //
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

	// ********************************************************************* //
	void Effect::UpdateShaderFileWatcher()
	{
		s_fileChangeListener.Update();
	}

	// ********************************************************************* //
	void Effect::AddToFileWatcher()
	{
		for (const std::string& file : m_VSFiles)
			s_fileChangeListener.RegisterEffect(file.substr(0, file.find_last_of('/') + 1), this);
		for (const std::string& file : m_GSFiles)
			s_fileChangeListener.RegisterEffect(file.substr(0, file.find_last_of('/') + 1), this);
		for (const std::string& file : m_PSFiles)
			s_fileChangeListener.RegisterEffect(file.substr(0, file.find_last_of('/') + 1), this);
	}

	// ********************************************************************* //
	void Effect::RemoveFromFileWatcher()
	{
		s_fileChangeListener.UnregisterEffect(this);
	}

	// ********************************************************************* //
	void Effect::HandleChangedShaderFile(const std::string& shaderFilename)
	{
		unsigned int possibleShaderTypes[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER };
		std::unordered_set<std::string>* fileLists[] = { &m_VSFiles, &m_PSFiles, &m_GSFiles };
		std::string* mainFileList[] = { &m_VSMainFile, &m_PSMainFile, &m_GSMainFile };

		for (int i = 0; i < 3; ++i)
		{
			unsigned int shaderType = possibleShaderTypes[i];
			if (fileLists[i]->find(shaderFilename) == fileLists[i]->end())
				continue;

			fileLists[i]->clear(); // This is safe if we assume that there is always only one change.
			unsigned int newShader = LoadShader(*mainFileList[i], shaderType, m_prefixCode, *fileLists[i]);
			if (newShader == 0)
			{
				LOG_ERROR("Failed to reload shader " + *mainFileList[i] + " on change. Will keep old one");
				return;
			}

			unsigned int newProgramID = GL_RET_CALL(glCreateProgram);

			GL_CALL(glAttachShader, newProgramID, shaderType == GL_VERTEX_SHADER ? newShader : m_vertexShader);
			GL_CALL(glAttachShader, newProgramID, shaderType == GL_FRAGMENT_SHADER ? newShader : m_pixelShader);
			if (m_geometryShader != 0)
				GL_CALL(glAttachShader, newProgramID, shaderType == GL_GEOMETRY_SHADER ? newShader : m_geometryShader);

			if (GL_CALL(glLinkProgram, newProgramID) == GLResult::SUCCESS && CheckShaderProgramError())
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

				LOG_LVL0("Successfully reloaded the shader " + *mainFileList[i]);
			}
			else
			{
				GL_CALL(glDeleteProgram, newProgramID);
				GL_CALL(glDeleteShader, newShader);
			}
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

			GL_CALL(glUniform1i, uniformLocation, _location);

			textureEntryIt.first->second.location = _location;
		}

		// Now store or overwrite the sampler state binding.
		if (textureEntryIt.second || textureEntryIt.first->second.sampler != &_sampler) // Means: New or not yet set.
		{
			textureEntryIt.first->second.sampler = &_sampler;
		}
	}

	// ********************************************************************* //
	void Effect::SetRasterizerState(const RasterizerState& rasterizerState)
	{
		m_rasterizerState = rasterizerState;
	}

	// ********************************************************************* //
	void Effect::SetBlendState(const BlendState& blendState)
	{
		m_blendState = blendState;
	}

	// ********************************************************************* //
	void Effect::SetDepthStencilState(const DepthStencilState& depthStencilState)
	{
		m_depthStencilState = depthStencilState;
	}

	// ********************************************************************* //
	void Effect::CommitUniformBuffers() const
	{
		for( size_t i=0; i<m_boundUniformBuffers.size(); ++i )
			m_boundUniformBuffers[i]->Commit();
	}

	// ********************************************************************* //
	bool Effect::CheckShaderProgramError()
	{
		// Check of linking the shader program worked
		GLint testResult;
		GL_CALL(glGetProgramiv, m_programID, GL_LINK_STATUS, &testResult);
		if (testResult == GL_FALSE)
		{
			GLsizei logLength = 0;
			GL_CALL(glGetProgramiv, m_programID, GL_INFO_LOG_LENGTH, &logLength);
			std::unique_ptr<char[]> log(new char[logLength + 1]);
			GL_CALL(glGetProgramInfoLog, m_programID, logLength, &logLength, log.get());
			log[logLength] = '\0'; // important for some gpu vendors!

			LOG_ERROR(std::string("Failed to build shader program: ") + log.get());

			return false;
		}

		return true;
	}

};