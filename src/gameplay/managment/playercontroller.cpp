#include "playercontroller.hpp"
#include "input/input.hpp"

PlayerController::PlayerController(Ship* _ship)
	: Controller(_ship)
{

}

void PlayerController::MouseMove(double _dx, double _dy)
{
	if (!m_ship) return;
}

void PlayerController::Scroll(double _dx, double _dy)
{
	
}
void PlayerController::KeyDown(int _key, int _modifiers)
{
	if (Input::Manager::IsVirtualKey(_key, Input::VirtualKey::ACCELERATE_FORWARD));
}
void PlayerController::KeyUp(int _key, int _modifiers)
{

}
