#include "controller.hpp"

Controller::Controller(Ship* _ship)
{
	if (_ship != nullptr) Possess(*_ship);
}

void Controller::Possess(Ship& _ship)
{
	m_ship = &_ship;

	//bind events
}