#include "game.hpp"
#include "graphic/device.hpp"
#include "utilities/color.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif


void exitprogram()
{
	// Close device
	Graphic::Device::Exit();
}

/// \brief Entry point.
int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	atexit(exitprogram);

	// Create a device with a window
	Graphic::Device::Initialize( 1024, 768, false );

	// Create and run game
	{
		Monolith Game( 60.0f );
		Game.Run();
	}
}