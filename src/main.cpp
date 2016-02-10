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

#include "graphic/highlevel/particlesystem.hpp"
using namespace Graphic;


/// \brief Entry point.
int main()
{
	ParticleSystem sys;
//	ParticleSystem::SubSystems<(uint)ParticleSystem::Comp::POSITION>::Get().AddParticle<1>(ei::Vec3(0.0f));
	sys.AddParticle<(uint)ParticleSystem::Comp::POSITION>(ei::Vec3(0.0f));
	sys.AddParticle<(uint)ParticleSystem::Comp::POSITION | (uint)ParticleSystem::Comp::VELOCITY>(ei::Vec3(0.0f), ei::Vec3(1.0f));
	//sys.AddParticle<(uint)ParticleSystem::Comp::POSITION>(ei::Vec3(0.0f), ei::Vec3(1.0f));
	//sys.AddParticle<(uint)ParticleSystem::Comp::POSITION | (uint)ParticleSystem::Comp::VELOCITY>(ei::Vec3(0.0f));

#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 18228 );
	// There is one known leak with a size of 44 Bytes in multithreading
	// it comes from an internal static variable of the thread implementation.
#endif

	Jo::Logger::g_logger.Initialize( new Jo::Logger::FilePolicy( "run.log" ) );
	atexit(exitprogram);

	// Create and run game
	{
		Monolith Game( true );
		Game.Run();
	}
}