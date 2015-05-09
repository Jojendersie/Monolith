#include "..\ship.hpp"

class Controller
{
public:
	Controller(SOHandle _ship = nullptr);

	///\brief Takes control of the target ship
	///\details Any former ownership of this controller is discarded
	void Possess(SOHandle _ship);

	virtual void Process(float _deltaTime) = 0;
protected:
	Ship* m_ship;
	
private:
	SOHandle m_shipHandle;
	//events
};