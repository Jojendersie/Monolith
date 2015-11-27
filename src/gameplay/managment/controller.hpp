#include "..\ship.hpp"

class Controller
{
public:
	Controller(SOHandle _ship = nullptr);

	///\brief Takes control of the target ship
	///\details Any former ownership of this controller is discarded
	void Possess(SOHandle _ship);

	void FaceDirection(const ei::Vec3& _targetDirection);

	void FlyToPosition(const Math::FixVec3& _pos, float _maxVelocity = 0.f, bool _stopAtGoal = true);

	virtual void Process(float _deltaTime);

	Ship* GetShip() { return m_ship; };

	const std::string& GetName() { return m_name; };
protected:
	Ship* m_ship;
	
	virtual void EvtCollision(Voxel::Model& _other) {}; //not pure virtual because implemantation should not be necesary
private:
	SOHandle m_shipHandle;

	std::string m_name; ///< The name of the intelligent entity
	//events

	//intern information
	Math::FixVec3 m_targetPosition;
	ei::Vec3 m_targetDirection;

	bool m_autoPilot;
};