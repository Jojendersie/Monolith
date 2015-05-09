#include "playercontroller.hpp"
#include "input/input.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"

using namespace Math;

PlayerController::PlayerController(SOHandle _ship)
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


void PlayerController::Process(float _deltaTime)
{
	// Movement
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
		m_ship->SetTargetVelocity(Vec3(0.0f, 0.0f, 1.0f));
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
		m_ship->SetTargetVelocity(Vec3(0.0f, 0.0f, -1.0f));

	// Rotation
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_E))
		m_ship->SetTargetAngularVelocity((~m_ship->GetRotation()).ZAxis() * 0.5f);
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_Q))
		m_ship->SetTargetAngularVelocity((~m_ship->GetRotation()).ZAxis() * -0.5f);
}