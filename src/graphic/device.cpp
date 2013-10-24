#include "device.hpp"
#include "../opengl.hpp"
#include <cstdio>
#include <iostream>

namespace Graphic {

	static Device g_Device;

	static void ErrorCallBack(int _iError, const char* _sDescription)
	{
		std::cout << "Error " << _iError << ": " << _sDescription;
	}

	GLFWwindow* Device::GetWindow()		{ return g_Device.m_Window; }

	void Device::Initialize( int _iWidth, int _iHeight, bool _bFullScreen )
	{
		// Create a new window with GLFW
		glfwSetErrorCallback(ErrorCallBack);
		glfwInit();
		g_Device.m_Window = glfwCreateWindow(_iWidth, _iHeight, "Monolith", nullptr, nullptr);
		glfwMakeContextCurrent(g_Device.m_Window);

		// Init glew.
		// There is a known bug in the NVidia driver -> DO NOT DELETE OR SET TO GL_FALSE
		// Otherwise the application crashes during the next glew call.
		glewExperimental = GL_TRUE;
		GLenum GlewInitResult = glewInit();

		if (GLEW_OK != GlewInitResult)
			std::cout << glewGetErrorString(GlewInitResult);
		else glGetError();	// Sometimes glewInit creates an arrow even if return val correct
	}


	void Device::Exit()
	{
		glfwDestroyWindow(g_Device.m_Window);
		glfwTerminate();
	}


	void Device::SetRasterizerState( const RasterizerState& _State )
	{
		int iHash = _State.GetHash();
		if( iHash != g_Device.m_iRasterizerState )
		{
			g_Device.m_iRasterizerState = iHash;
			// Set all relateted states now.
			_State.Apply();
		}
	}


	void Device::SetSamplerState( unsigned _iLocation, const SamplerState& _State )
	{
		int iHash = _State.GetHash();
		if( iHash != g_Device.m_piSamplerStates[_iLocation] )
		{
			g_Device.m_piSamplerStates[_iLocation] = iHash;
			// Set all relateted states now.
			_State.Apply(_iLocation);
		}
	}


	void Device::SetBlendState( const BlendState& _State )
	{
		int iHash = _State.GetHash();
		if( iHash != g_Device.m_iBlendState )
		{
			g_Device.m_iBlendState = iHash;
			// Set all relateted states now.
			_State.Apply();
		}
	}


	void Device::SetDepthStencilState( const DepthStencilState& _State )
	{
		int iHash = _State.GetHash();
		if( iHash != g_Device.m_iDepthStencilState )
		{
			g_Device.m_iDepthStencilState = iHash;
			// Set all relateted states now.
			_State.Apply();
		}
	}

	void Device::Clear( float _r, float _g, float _b )
	{
		glClearColor( _r, _g, _b, 1.0f );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Device::ClearZ()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}
};