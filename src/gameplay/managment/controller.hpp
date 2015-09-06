#include "..\ship.hpp"

class Controller
{
public:
	Controller(SOHandle _ship = nullptr);

	///\brief Takes control of the target ship
	///\details Any former ownership of this controller is discarded
	void Possess(SOHandle _ship);

	virtual void Process(float _deltaTime) = 0;

	Ship* GetShip() { return m_ship; };
protected:
	Ship* m_ship;
	
	virtual void EvtCollision(Voxel::Model& _other) {}; //not pure virtual because implemantation should be necesary
private:
	SOHandle m_shipHandle;
	//events
};