#include "device.hpp"
#include "../opengl.hpp"
#include <cstdio>

namespace Graphic {

	static Device g_Device;

	void Device::Initialize( int _iWidth, int _iHeight, bool _bFullScreen )
	{
		// Create a new window with freeGLUT
		// Init without any arguments
		int iArgc = 0;
		glutInit(&iArgc,0);
		glutInitContextVersion(3, 3);
		//glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
		glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );//| GLUT_MULTISAMPLE );

		if( _bFullScreen )
		{
			// full screen - no window
			char acMode[32];
			sprintf(acMode, "%dx%d:32@60", _iWidth, _iHeight);
			glutGameModeString( acMode );
			g_Device.m_iWindow = glutEnterGameMode();
		} else {
			// Standard window
			glutInitWindowSize( _iWidth, _iHeight );
			g_Device.m_iWindow = glutCreateWindow( "Monolith" );
			// The ResizeFunction is not called here, but after Window init everything
			// should be correct. Subtract border manually for current renderable area.
//			g_iScreenWidth = _iWidth - 2*glutGet( GLUT_WINDOW_BORDER_WIDTH );
	//		g_iScreenHeight = _iHeight - 2*glutGet( GLUT_WINDOW_BORDER_WIDTH );
		}
		
		//if(g_iWindow < 1)
			//OrE::Log::FatalError("[Window::Init] Could not create a new rendering window.");

		// Init glew.
		// There is a known bug in the NVidia driver -> DO NOT DELETE OR SET TO GL_FALSE
		// Otherwise the application crashes during the next glew call.
		glewExperimental = GL_TRUE;
		GLenum GlewInitResult = glewInit();

		//if (GLEW_OK != GlewInitResult)
		//	OrE::Log::FatalError("%s", glewGetErrorString(GlewInitResult));
		//else glGetError();	// Sometimes glewInit creates an arrow even if return val correct
	}


	void Device::Exit()
	{
	}
};