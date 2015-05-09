#include "controller.hpp"

class PlayerController : public Controller
{
public:
	PlayerController(SOHandle _ship = nullptr);

	void MouseMove(double _dx, double _dy);
	void Scroll(double _dx, double _dy);
	void KeyDown(int _key, int _modifiers);
	void KeyUp(int _key, int _modifiers);

	virtual void Process(float _deltaTime) override;
private:
};