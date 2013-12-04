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
}

/// \brief Entry point.
int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	atexit(exitprogram);

	float d = Math::Mat4x4::Scaling(Math::PI).Det();
	Math::Mat4x4 m = Math::Mat4x4::RotationX(Math::PI) * Math::Mat4x4::Translation(1.0f, 3.0f, 5.0f);
	Math::Vec3 v(1.0f);
	v = m.Transform(v);
	v = m.Invert().Transform(v);
	m = Math::Mat4x4::Translation(1.0f, 3.0f, 5.0f).Transpose();
	int i=sizeof(m);



	// Create a device with a window
	Graphic::Device::Initialize( 1024, 768, false );

	// Create and run game
	{
		Monolith Game( 60.0f );
		Game.Run();
	}
}