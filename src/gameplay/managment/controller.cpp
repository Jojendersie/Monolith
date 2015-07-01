#include "controller.hpp"

Controller::Controller(SOHandle _ship)
{
	if (_ship) Possess(_ship);
}

void Controller::Possess(SOHandle _ship)
{
	m_ship = static_cast<Ship*>(&_ship);
	m_shipHandle = _ship;

	//bind events
	m_ship->SetOnCollision(std::bind(&Controller::EvtCollision, this, std::placeholders::_1));
}