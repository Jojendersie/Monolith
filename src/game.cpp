#include <thread>
#include "game.hpp"
#include "opengl.hpp"
#include "graphic/device.hpp"

static double g_fInvFrequency;

Monolith::Monolith( float _fTargetFrameRate ) :
	m_bSingleThreaded( true ),
	m_bRunning( true ),
	m_fTime( 0.0 ),
	m_iCurrentGameState( 0 )
{
	// Init timer
	g_fInvFrequency = std::chrono::high_resolution_clock::period::num/double(std::chrono::high_resolution_clock::period::den);
	m_iMicroSecPerFrame = std::chrono::microseconds(unsigned(900000.0 / _fTargetFrameRate));

	// Create game states
	m_ppGameStates[0] = new GSMain();
}

Monolith::~Monolith()
{
	delete m_ppGameStates[0];
}

void Monolith::Run()
{
	if( m_bSingleThreaded )
	{
		std::chrono::high_resolution_clock::time_point StartPoint, EndPoint;
		StartPoint = std::chrono::high_resolution_clock::now();
		while( m_bRunning && !glfwWindowShouldClose(Graphic::Device::GetWindow()) )
		{
			// Calculate time since last frame
			EndPoint = std::chrono::high_resolution_clock::now();
			std::chrono::high_resolution_clock::rep Ticks = (EndPoint - StartPoint).count();
			StartPoint = EndPoint;

			double fDeltaTime = Ticks * g_fInvFrequency;
			m_fTime += fDeltaTime;

			// Call stuff
			m_ppGameStates[m_iCurrentGameState]->Update( m_fTime, fDeltaTime );
			m_ppGameStates[m_iCurrentGameState]->Render( m_fTime, fDeltaTime );

			glfwSwapBuffers(Graphic::Device::GetWindow());
			glfwPollEvents();

			// Limiting to target fps
			std::this_thread::sleep_until( EndPoint + m_iMicroSecPerFrame  );
		}
	}
}