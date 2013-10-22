#include <Windows.h>
#include <thread>
#include "game.hpp"

static double g_fInvFrequency;

Monolith::Monolith( float _fTargetFrameRate ) :
	m_bSingleThreaded( true ),
	m_bRunning( true ),
	m_fTime( 0.0 ),
	m_fTargetRenderFRInv( 1.0/_fTargetFrameRate ),
	m_iCurrentGameState( 0 )
{
	// Init timer
	uint64_t iFreq;
	QueryPerformanceFrequency( (LARGE_INTEGER*)&iFreq );
	g_fInvFrequency = 1.0 / iFreq;

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
		uint64_t iOldTime, iNewTime;
		QueryPerformanceCounter( (LARGE_INTEGER*)&iOldTime );
		while( m_bRunning )
		{
			// Calculate time scince last frame
			QueryPerformanceCounter( (LARGE_INTEGER*)&iNewTime );
			double fDeltaTime = ( iNewTime - iOldTime ) * g_fInvFrequency;
			// Limiting to target fps
			if( fDeltaTime > m_fTargetRenderFRInv )
			{
				iOldTime = iNewTime;
				m_fTime += fDeltaTime;

				// Call stuff
				m_ppGameStates[m_iCurrentGameState]->Update( m_fTime, fDeltaTime );
				m_ppGameStates[m_iCurrentGameState]->Render( m_fTime, fDeltaTime );
			}
		}
	}
}