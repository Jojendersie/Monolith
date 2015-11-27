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
	//fuck quaternion magic(for now).
//	m_ship->SetRotation(_targetDirection);
}

// *************************************************** //

void Controller::FlyToPosition(const Math::FixVec3& _pos, float _maxVelocity, bool _stopAtGoal)
{
	m_targetPosition = _pos;

	auto dirFix = _pos - m_ship->GetPosition();
	Vec3 direction;
	direction[0] = float(double(dirFix[0]));
	direction[1] = float(double(dirFix[1]));
	direction[2] = float(double(dirFix[2]));

	FaceDirection(direction);//m_ship->Transform(_pos)

	m_autoPilot = true;

/*	Vec3 slfDir = m_ship->GetRotationMatrix() * Vec3(0.f, 0.f, -1.f);//-m_ship->GetRotation().ZAxis();
	float angle = acos(dot(m_targetDirection, slfDir));
	if (abs(angle) > 0.02f)
	{
		Vec3 axis = cross(m_targetDirection, slfDir);
		Vec3 temp = cross(axis, slfDir);
		if (dot(temp, m_targetDirection) < 0) angle = -angle;
//		m_ship->SetRotation(Quaternion(normalize(axis), angle) * m_ship->GetRotation());
	//	m_ship->Rotate(normalize(axis), angle);
		slfDir = Mat3x3::Rotation(Quaternion(normalize(axis), angle)) * slfDir;
		m_ship->SetTargetAngularVelocity(angle * 0.5f * normalize(axis));
	}
	Vec3 test = m_ship->GetRotationMatrix() * Vec3(0.f, 0.f, -1.f);*/
	//	float a
//	m_ship->SetTargetVelocity(Vec3(0.f, 0.f, 2.f));
}

// *************************************************** //

void Controller::Process(float _deltaTime)
{
	if (!m_autoPilot) return;

	Vec3 dirSlf = zaxis(m_ship->GetRotation());//m_ship->GetRotationMatrix() * Vec3(0.f, 0.f, -1.f);//-m_ship->GetRotation().ZAxis();

	float angle = acos(dot(m_targetDirection, dirSlf));
	if (abs(angle) > 0.02f)
	{
		Vec3 axis = cross(m_targetDirection, dirSlf);
		Vec3 temp = cross(axis, dirSlf);
		if (dot(temp, m_targetDirection) < 0) angle = -angle;
		m_ship->Rotate(Quaternion(dirSlf, m_targetDirection));
	//	m_ship->SetRotation(Quaternion(normalize(axis), angle) * m_ship->GetRotation());
	//	m_ship->Rotate(normalize(axis), angle);
	//	m_ship->SetTargetAngularVelocity(angle * 0.5f * normalize(axis));
	}
//	float angularDist = length(direction - m_ship->GetRotation().ZAxis());
/*	Vec3 axis = cross(m_targetDirection, dirSlf);
	Vec3 temp = cross(axis, dirSlf);
	if (dot(temp, m_targetDirection) < 0) angle = -angle;*/

}