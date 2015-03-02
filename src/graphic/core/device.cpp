#include "device.hpp"
#include "opengl.hpp"
#include "../../predeclarations.hpp"
#include "vertexbuffer.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
//#include <cstdio>
#include "utilities/assert.hpp"

namespace Graphic {

	static Device g_Device;

	static void ErrorCallBack(int _iError, const char* _sDescription)
	{
		LOG_ERROR( std::string("glfw [") + std::to_string(_iError) + "] " + _sDescription );
	}

	static void APIENTRY DebugErrorCallback(
		GLenum _source,
		GLenum _type,
		GLuint _id,
		GLenum _severity,
		GLsizei _length,
		const GLchar* _message,
		GLvoid* _userParam
		)
	{
		std::string debSource, debType;
		int eventType = 2;	// Use LOG_LVL1 if no other option is better

		switch( _source )
		{
		case GL_DEBUG_SOURCE_API_ARB:             debSource = "OpenGL";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   debSource = "Windows";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: debSource = "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:     debSource = "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION_ARB:     debSource = "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER_ARB:           debSource = "Other";
			break;
		}

		switch ( _type )
		{
		case GL_DEBUG_TYPE_ERROR_ARB:
			eventType = 0;
			debType = "error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
			debType = "deprecated behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
			eventType = 1;
			debType = "undefined behavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY_ARB:
			debType = "portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE_ARB:
			debType = "performance";
			break;
		case GL_DEBUG_TYPE_OTHER_ARB:
			debType = "message";
			break;
		}

		if(_severity == GL_DEBUG_SEVERITY_HIGH_ARB)
			LOG_CRITICAL( "[" + debSource + "|" + debType + "]: " + _message );

		switch(eventType)
		{
		case 0: LOG_ERROR( "[" + debSource + "|" + debType + "]: " + _message );
			break;
		case 1: LOG_LVL2( "[" + debSource + "|" + debType + "]: " + _message );
			break;
		case 2: LOG_LVL1( "[" + debSource + "|" + debType + "]: " + _message );
			break;
		}
	}

	static void ActivateOpenGLDebugging()
	{
		GL_CALL(glEnable, GL_DEBUG_OUTPUT);
		GL_CALL(glEnable, GL_DEBUG_OUTPUT_SYNCHRONOUS);

		GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
		GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		GL_CALL(glDebugMessageCallback, &DebugErrorCallback, nullptr);
	}

	GLFWwindow* Device::GetWindow()
	{
		return g_Device.m_window;
	}

	Math::IVec2 Device::GetBackbufferSize()
	{
		Math::IVec2 size;
		glfwGetFramebufferSize( g_Device.m_window, &size[0], &size[1] );
		return size;
	}

	float Device::GetAspectRatio()
	{
		int width, height;
		glfwGetFramebufferSize( g_Device.m_window, &width, &height );
		return width / float(height);
	}

	void Device::Initialize( int _width, int _height, bool _fullScreen )
	{
		// Create a new window with GLFW
		glfwSetErrorCallback(ErrorCallBack);
		if( !glfwInit() )
			LOG_CRITICAL("Could not initialize GLFW.");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
		g_Device.m_window = glfwCreateWindow(_width, _height, "Monolith", nullptr, nullptr);
		if( !g_Device.m_window )
			LOG_CRITICAL("Window was not created.");
		glfwMakeContextCurrent(g_Device.m_window);
		glfwSwapInterval(0);

		// Init glew.
		// There is a known bug in the NVidia driver -> DO NOT DELETE OR SET TO GL_FALSE
		// Otherwise the application crashes during the next glew call.
		glewExperimental = GL_TRUE;
		GLenum GlewInitResult = glewInit();

		if (GLEW_OK != GlewInitResult)
			LOG_ERROR(std::string((char*)glewGetErrorString(GlewInitResult)));
		else glGetError();	// Sometimes glewInit creates an error even if return val correct

		ActivateOpenGLDebugging();

		// Null the references
		g_Device.m_currentEffect = nullptr;
		g_Device.m_blendState = -1;
		g_Device.m_depthStencilState = -1;
		g_Device.m_rasterizerState = -1;
		for (int i = 0; i<8; ++i)
			g_Device.m_samplerStates[i] = -1;

		// Bind the hardware-backbuffer.
		GL_CALL(glViewport, 0, 0, _width, _height);
		g_Device.m_BoundFrameBuffer = NULL;

		// Uses standard counterclockwise culling
		GL_CALL(glFrontFace, GL_CCW);
	}


	void Device::Exit()
	{
		glfwDestroyWindow(g_Device.m_window);
		glfwTerminate();
	}


	void Device::SetRasterizerState( const RasterizerState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_rasterizerState )
		{
			g_Device.m_rasterizerState = iHash;
			// Set all related states now.
			_state.Apply();
		}
	}


	void Device::SetSamplerState( unsigned _location, const SamplerState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_samplerStates[_location] )
		{
			g_Device.m_samplerStates[_location] = iHash;
			// Set all related states now.
			_state.Apply(_location);
		}
	}


	void Device::SetBlendState( const BlendState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_blendState )
		{
			g_Device.m_blendState = iHash;
			// Set all related states now.
			_state.Apply();
		}
	}


	void Device::SetDepthStencilState( const DepthStencilState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_depthStencilState )
		{
			g_Device.m_depthStencilState = iHash;
			// Set all related states now.
			_state.Apply();
		}
	}


	void Device::SetEffect( const Effect& _effect )
	{
		//glBindTexture( 1234, 1234 );
		if( g_Device.m_currentEffect != &_effect )
		{
			SetRasterizerState( _effect.m_rasterizerState );
			SetBlendState( _effect.m_blendState );
			SetDepthStencilState( _effect.m_depthStencilState );
			for (auto& it : _effect.m_boundTextures)
				SetSamplerState(it.second.location, *it.second.sampler);

			GL_CALL(glUseProgram, _effect.m_programID);
			g_Device.m_currentEffect = &_effect;
		}
	}


	void Device::SetTexture( const Texture& _texture, unsigned _location )
	{
		GL_CALL(glActiveTexture, _location + GL_TEXTURE0);
		GL_CALL(glBindTexture, _texture.m_bindingPoint, _texture.m_textureID);
	}


	void Device::BindFramebuffer(const Framebuffer* _framebuffer, bool _autoViewportSet)
	{
		unsigned int previousColorTargetCount = 1;
		if (g_Device.m_BoundFrameBuffer != nullptr)
			previousColorTargetCount = (unsigned)g_Device.m_BoundFrameBuffer->m_colorAttachments.size();
		unsigned int currentColorTargetCount = previousColorTargetCount;

		if (_framebuffer)
		{
			if (g_Device.m_BoundFrameBuffer != _framebuffer)
			{
				currentColorTargetCount = (unsigned)_framebuffer->m_colorAttachments.size();

				GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, _framebuffer->m_framebuffer);
				g_Device.m_BoundFrameBuffer = _framebuffer;

				if (_autoViewportSet)
				{
					const Framebuffer::Attachment* pSizeSource = NULL;
					if (_framebuffer->m_depthStencil.pTexture)
						pSizeSource = &_framebuffer->m_depthStencil;
					else
						pSizeSource = &_framebuffer->m_colorAttachments[0];

					// Due to asserts on creation, pSizeSource should be now non zero!
					Math::IVec2 size = pSizeSource->pTexture->Size2D();
					for (unsigned int mipLevel = 0; mipLevel < pSizeSource->mipLevel; ++mipLevel)
					{
						size[0] /= 2;
						size[1] /= 2;
					}
					GL_CALL(glViewport, 0, 0, size[0], size[1]);
				}
			}
		}
		else
		{
			if (g_Device.m_BoundFrameBuffer != NULL)
			{
				GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
				g_Device.m_BoundFrameBuffer = NULL;
				currentColorTargetCount = 1;

				if (_autoViewportSet)
				{
					GL_CALL(glViewport, 0, 0, GetBackbufferSize()[0], GetBackbufferSize()[1]);
				}
			}
		}


		// setup draw buffers
		if(previousColorTargetCount != currentColorTargetCount)
		{
			static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
				GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
				GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15 };
			GL_CALL(glDrawBuffers, previousColorTargetCount, drawBuffers);
		}
	}

	const Framebuffer* Device::GetCurrentFramebufferBinding()
	{
		return g_Device.m_BoundFrameBuffer;
	}

	void Device::Clear( float _r, float _g, float _b )
	{
		GL_CALL(glClearColor, _r, _g, _b, 1.0f);
		// Need to write z in clear call
		GLboolean writeDepth; 
		GL_CALL(glGetBooleanv, GL_DEPTH_WRITEMASK, &writeDepth);	// TODO: Get functions should be avoided!
		GL_CALL(glDepthMask, true);
		GL_CALL(glClearDepth, 1.0f);
		GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Return to old state, otherwise effects could get confused
		GL_CALL(glDepthMask, writeDepth);
	}

	void Device::ClearZ()
	{
		GL_CALL(glClearDepth, 1.0f);
		GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);
	}


	void Device::DrawVertices( const VertexBuffer& _buffer, int _from, int _count )
	{
		_buffer.Bind();

		g_Device.m_currentEffect->CommitUniformBuffers();

		GL_CALL(glDrawArrays, unsigned(_buffer.GetPrimitiveType()), _from, _count);
	}
};