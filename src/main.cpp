#include "game.hpp"
#include "graphic/core/device.hpp"
//#include "predeclarations.hpp"
#include "utilities/loggerinit.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif


void exitprogram()
{
	// Close device
	Graphic::Device::Exit();
}

#include "utilities/pathutils.hpp"

/// \brief Entry point.
int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 458 );
#endif

	Jo::Logger::g_logger.Initialize( new Jo::Logger::FilePolicy( "run.log" ) );
	atexit(exitprogram);

	// Create and run game
	{
		Monolith Game( 60.0f );
		Game.Run();
	}
}