#include <fstream>

#include "playercontroller.hpp"
#include "input/input.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"
#include "utilities/scriptengineinst.hpp"

using namespace ei;

PlayerController::PlayerController(SOHandle _ship, Input::Camera* _camera)
	: Controller(_ship),
	m_mouseRotationEnabled(false),
	m_camera(_camera)
{
}

void PlayerController::Possess(SOHandle _ship)
{
	Controller::Possess(_ship);

	NaReTi::Module* mod = g_scriptEngine.getModule("shipfunctions");
	mod->linkExternal("setFiring", &Mechanics::WeaponSystem::_SetFiring);

	std::ofstream file("scripts/availablesystems.nrt", std::ofstream::out & std::ofstream::trunc);
	file << "use shipfunctions" << std::endl << std::endl;
	m_ship->GetPrimarySystem().exportSystems(file);

	//make sure that the module is loaded
	NaReTi::Module* inputMod = g_scriptEngine.getModule("input");
	m_keyDownHndl = g_scriptEngine.getFuncHndl("keyDown");
	m_keyReleaseHndl = g_scriptEngine.getFuncHndl("keyRelease");
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
	g_scriptEngine.call<void, int, int>(m_keyDownHndl, _key, _modifiers);
}

void PlayerController::KeyRelease(int _key)
{
	g_scriptEngine.call<void, int>(m_keyReleaseHndl, _key);

	if (_key == GLFW_KEY_V)
	{
		m_camera->SetRelativePosition(m_ship->GetNextView() - Vec3(0.f,0.f,5.0f)); // no idea why the z offset has to be that high...
	}
}


void PlayerController::Process(float _deltaTime)
{
	// Movement
	// Velocity in Z is increased/decreased while a key is pressed.
	// In other directions stop pushing resets the velocity
	m_velocity[0] = 0.0f;
	// Do not increase the target velocity much further than the real velocity.
	float realVelZ = (m_ship->GetInverseRotationMatrix() * m_ship->GetVelocity())[2];
	if( realVelZ > 0.0f ) m_velocity[2] = max(0.0f, min(m_velocity[2], realVelZ));
	else m_velocity[2] = min(0.0f, max(m_velocity[2], realVelZ));
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

	// Let the camera slide back to the main ship direction
	if(m_mouseRotationEnabled)
	{
		m_camera->Rotate(-m_camera->GetXRotation() * 0.05f, -m_camera->GetYRotation() * 0.05f);
	}

	// Rotation
	// Stop pushing the rotation keys always resets to zero.
	Vec3 angularVel(0.0f);
	if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_E))
		angularVel += Vec3(0.0f, 0.0f, 0.5f);
	else if(glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_Q))
		angularVel += Vec3(0.0f, 0.0f, -0.5f);
	// Rotate towards mouse cursor
	if( m_mouseRotationEnabled )
	{
		Vec2 cursor = Input::Manager::GetCursorPosScreenSpace();
		//cursor = Vec2(sgn(cursor[0]), sgn(cursor[1])) * max(Vec2(0.0f), abs(cursor) - 0.1f);
		cursor = Vec2(sgn(cursor[0]), sgn(cursor[1])) * cursor * cursor;
		angularVel += Vec3(-cursor[1], cursor[0], 0.0f);
		//angularVel += m_ship->GetInverseRotationMatrix().YAxis() * -cursor[0];
		//angularVel += m_ship->GetInverseRotationMatrix().XAxis() * cursor[1];
	}
	m_ship->SetTargetAngularVelocity(angularVel);
}


void PlayerController::EvtCollision(Voxel::Model& _other)
{
	//velocity is set to the current value to prevent any further actions
	//before the player can react
	m_ship->SetTargetVelocity(m_ship->GetVelocity());
}