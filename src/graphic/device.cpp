#include "device.hpp"
#include "../opengl.hpp"
#include "vertexbuffer.hpp"
#include <cstdio>
#include <iostream>
#include <cassert>

namespace Graphic {

	static Device g_Device;

	static void ErrorCallBack(int _iError, const char* _sDescription)
	{
		std::cout << "Error " << _iError << ": " << _sDescription;
	}

	GLFWwindow* Device::GetWindow()		{ return g_Device.m_window; }

	void Device::Initialize( int _width, int _height, bool _fullScreen )
	{
		// Create a new window with GLFW
		glfwSetErrorCallback(ErrorCallBack);
		if( !glfwInit() ) std::cout << "[Device::Initialize] Could not initialise GLFW.\n";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		g_Device.m_window = glfwCreateWindow(_width, _height, "Monolith", nullptr, nullptr);
		if( !g_Device.m_window ) std::cout << "[Device::Initialize] Window was not created.\n";
		glfwMakeContextCurrent(g_Device.m_window);

		// Init glew.
		// There is a known bug in the NVidia driver -> DO NOT DELETE OR SET TO GL_FALSE
		// Otherwise the application crashes during the next glew call.
		glewExperimental = GL_TRUE;
		GLenum GlewInitResult = glewInit();

		if (GLEW_OK != GlewInitResult)
			std::cout << glewGetErrorString(GlewInitResult);
		else glGetError();	// Sometimes glewInit creates an arrow even if return val correct

		glViewport(0, 0, _width, _height);
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
			// Set all relateted states now.
			_state.Apply();
		}
	}


	void Device::SetSamplerState( unsigned _location, const SamplerState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_samplerStates[_location] )
		{
			g_Device.m_samplerStates[_location] = iHash;
			// Set all relateted states now.
			_state.Apply(_location);
		}
	}


	void Device::SetBlendState( const BlendState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_blendState )
		{
			g_Device.m_blendState = iHash;
			// Set all relateted states now.
			_state.Apply();
		}
	}


	void Device::SetDepthStencilState( const DepthStencilState& _state )
	{
		int iHash = _state.GetHash();
		if( iHash != g_Device.m_depthStencilState )
		{
			g_Device.m_depthStencilState = iHash;
			// Set all relateted states now.
			_state.Apply();
		}
	}


	void Device::SetEffect( const Effect& _effect )
	{
		assert(glGetError() == GL_NO_ERROR);
		SetRasterizerState( _effect.m_rasterizerState );
		//SetSamplerState( 0, _effect.
		SetBlendState( _effect.m_blendState );
		SetDepthStencilState( _effect.m_depthStencilState );
		assert(glGetError() == GL_NO_ERROR);
		glUseProgram( _effect.m_programID );
		assert(glGetError() == GL_NO_ERROR);
	}


	void Device::Clear( float _r, float _g, float _b )
	{
		glClearColor( _r, _g, _b, 1.0f );
		glClearDepth( 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void Device::ClearZ()
	{
		glClearDepth( 1.0f );
		glClear( GL_DEPTH_BUFFER_BIT );
	}


	void Device::DrawVertices( const VertexBuffer& _buffer, int _from, int _count )
	{
		_buffer.Bind();
#ifdef _DEBUG
		LogGlError("[Device::DrawVertices] Could not bind the vertex buffer");
#endif
		glDrawArrays( unsigned(_buffer.GetPrimitiveType()), _from, _count );

#ifdef _DEBUG
		LogGlError("[Device::DrawVertices] glDrawArrays failed");
#endif
	}
};