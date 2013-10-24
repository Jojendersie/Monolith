#include "game.hpp"
#include "graphic/device.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif


void exitprogram()
{
	// Close device
	Graphic::Device::Exit();

#if defined(DEBUG) || defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}

/// \brief Entry point.
int main()
{
	atexit(exitprogram);

	// Create a device with a window
	Graphic::Device::Initialize( 1024, 768, false );

	// Create and run game
	{
		Monolith Game( 120.0f );
		Game.Run();
	}
}