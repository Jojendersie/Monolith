#include "..\ship.hpp"

class Controller
{
public:
	Controller(Ship* _ship = nullptr);

	///\brief Takes control of the target ship
	///\details Any former ownership of this controller is discarded
	void Possess(Ship& _ship);

	//virtual void Process(float _deltaTime);
protected:
	Ship* m_ship;
private:
	
	//events
};