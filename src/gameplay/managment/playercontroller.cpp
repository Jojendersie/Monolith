#include "playercontroller.hpp"
#include "input/input.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"

using namespace Math;

PlayerController::PlayerController(SOHandle _ship)
	: Controller(_ship),
	m_mouseRotationEnabled(false)
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
}

void PlayerController::KeyUp(int _key, int _modifiers)
{

}


void PlayerController::Process(float _deltaTime)
{
	// Movement
	// Velocity in Z is increased/decreased while a key is pressed.
	// In other directions stop pushing resets the velocity
	m_velocity[0] = 0.0f;
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
		m_velocity[2] += _deltaTime * 50.0f;
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
		m_velocity[2] -= _deltaTime * 50.0f;
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_D))
		m_velocity[0] = _deltaTime * 100.0f;
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_A))
		m_velocity[0] = -_deltaTime * 100.0f;
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_TAB))
		m_velocity = Vec3(0.0f);
	m_ship->SetTargetVelocity(m_velocity);

	// Rotation
	// Stop pushing the rotation keys always resets to zero.
	Vec3 angularVel(0.0f);
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_E))
		angularVel += m_ship->GetInverseRotationMatrix().ZAxis() * 0.5f;
	else if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_Q))
		angularVel += m_ship->GetInverseRotationMatrix().ZAxis() * -0.5f;
	// Rotate towards mouse cursor
	if( m_mouseRotationEnabled )
	{
		Vec2 cursor = Input::Manager::GetCursorPosScreenSpace();
		//cursor = Vec2(sgn(cursor[0]), sgn(cursor[1])) * max(Vec2(0.0f), abs(cursor) - 0.1f);
		cursor = Vec2(sgn(cursor[0]), sgn(cursor[1])) * cursor * cursor;
		angularVel += m_ship->GetInverseRotationMatrix().YAxis() * -cursor[0];
		angularVel += m_ship->GetInverseRotationMatrix().XAxis() * cursor[1];
	}
	m_ship->SetTargetAngularVelocity(angularVel);
}