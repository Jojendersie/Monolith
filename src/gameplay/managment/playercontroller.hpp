#include "controller.hpp"
#include "input\camera.hpp"

class PlayerController : public Controller
{
public:
	PlayerController(SOHandle _ship = nullptr, Input::Camera* _camera = nullptr);

	void MouseMove(double _dx, double _dy);
	void Scroll(double _dx, double _dy);
	void KeyDown(int _key, int _modifiers);
	void KeyUp(int _key, int _modifiers);

	void SetMouseRotation(bool _enable) { m_mouseRotationEnabled = _enable; }
	void SetTarget(Voxel::Model* _newTarget) { m_target = _newTarget; };

	virtual void Process(float _deltaTime) override;
private:
	virtual void EvtCollision(Voxel::Model& _other) override;

	bool m_mouseRotationEnabled;
	ei::Vec3 m_velocity; ///< Ship relative velocity

	Voxel::Model* m_target; ///< selected target

	Input::Camera* m_camera;
};