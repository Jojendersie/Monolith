#include "controller.hpp"

using namespace ei;

Controller::Controller(SOHandle _ship)
	:m_autoPilot(false)
{
	if (_ship) Possess(_ship);
}

// *************************************************** //

void Controller::Possess(SOHandle _ship)
{
	m_ship = static_cast<Ship*>(&_ship);
	m_shipHandle = _ship;

	//bind events
	m_ship->SetOnCollision(std::bind(&Controller::EvtCollision, this, std::placeholders::_1));
}

// *************************************************** //

void Controller::FaceDirection(const ei::Vec3& _targetDirection)
{
	m_targetDirection = normalize(_targetDirection);
}

// *************************************************** //

void Controller::UpdateTargetDirection()
{
	auto dirFix = m_targetPosition - m_ship->GetPosition();
	Vec3 direction;
	direction[0] = float(double(dirFix[0]));
	direction[1] = float(double(dirFix[1]));
	direction[2] = float(double(dirFix[2]));

	FaceDirection(direction);
}

// *************************************************** //

void Controller::FlyToPosition(const Math::FixVec3& _pos, float _maxVelocity, bool _stopAtGoal)
{
	m_stopAtGoal = _stopAtGoal;
	m_targetPosition = _pos;

	UpdateTargetDirection();

	m_autoMaxSpeed = _maxVelocity == 0 ? 64.f : _maxVelocity;//FLT_MAX
	m_autoPilot = true;
}

// *************************************************** //

void Controller::Process(float _deltaTime)
{
	if (!m_autoPilot) return;

	//update required rotation, this should probably not happen every frame
	UpdateTargetDirection();

	//adjust rotation
	Vec3 dirSlf = Vec3(0.f, 0.f, 1.f);

	Vec3 targetDirection = m_ship->GetInverseRotationMatrix() * m_targetDirection;
	float angle = acos(dot(targetDirection, dirSlf));
//	if (abs(angle) < 0.02f) m_ship->SetTargetAngularVelocity(Vec3(0.f));
//	else
	//todo pay attention to inertia
	if (abs(angle) > 0.03f)
	{
		Vec3 axis = cross(targetDirection, dirSlf);
		Vec3 temp = cross(axis, dirSlf);
		if (dot(temp, targetDirection) < 0) angle = -angle;
	//	m_ship->SetRotation(Quaternion(Vec3(0.f, 0.f, 1.f), m_targetDirection));
		m_ship->SetTargetAngularVelocity(normalize(axis) *  angle * 1.2f);
	}
	//reached the right rotation -> start accelerating
	else
	{
		//adjust velocity
		m_ship->SetTargetVelocity(Vec3(0.f, 0.f, m_autoMaxSpeed));

		if (lensq(m_targetPosition - m_ship->GetPosition()) < 10.f)//some randomly choosen constant
		{
			//goal is reached
			m_autoPilot = false;
			if (m_stopAtGoal)
			{
				m_ship->SetTargetVelocity(Vec3(0.f));
				m_ship->SetTargetAngularVelocity(Vec3(0.f));
			}
		}
	}
}