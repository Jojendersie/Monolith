#include <ei/vector.hpp>
#include "../core/device.hpp"

namespace Graphic{

	//describes a global position on the screen
	struct PixelCoord : public ei::IVec2
	{
		template<typename... _Args>
		PixelCoord(_Args&&... _args):
			ei::IVec2(std::forward< _Args >(_args)...){}

		static PixelCoord FromScreenPos(const ei::Vec2& _pos) {}

		operator ei::Vec2() const { return *this * 2.f / Device::GetBackbufferSize() - ei::Vec2(1.f,1.f); }
	};

	//use this for pixel offsets and sizes
	struct PixelOffset : public ei::IVec2
	{
		template<typename... _Args>
		PixelOffset(_Args&&... _args) :
			ei::IVec2(std::forward< _Args >(_args)...){}

		operator ei::Vec2() const { return *this * 2.f / Device::GetBackbufferSize(); }
	};

}	