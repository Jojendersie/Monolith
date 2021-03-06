#include "starsystem.hpp"
#include "input/camera.hpp"
#include "math/fixedpoint.hpp"
#include "../graphic/core/uniformbuffer.hpp"


#include <math.h>

using namespace ei;
using namespace Math;

StarSystem::StarSystem(const FixVec3& _position, int _temperature, float _size, const Quaternion& _rotation)
	:Transformation(_position, _rotation)
{
	m_vertex.color = TemperatureToRGB(_temperature).RGBA();
	m_vertex.size = _size;
}

StarVertex& StarSystem::ComputeVertex(const Input::Camera& _camera)
{
	m_vertex.position = _camera.Transformation().Transform(m_position);
	return m_vertex;
}


Utils::Color8U StarSystem::TemperatureToRGB(int _temperature)
{
	//all calculations need this
	_temperature /= 100;

	int red;
	int green;
	int blue;
	
	//red
	if (_temperature <= 66)
		red = 255;
	else
	{
		red = _temperature - 60;
		red = int(329.698727446 * pow(red, -0.1332047592));
		if (red < 0)  red = 0;
		if (red > 255)  red = 255;
	}

	//green
	if (_temperature <= 66)
	{
		green = _temperature;
		green = int(99.4708025861 * log2(green) - 161.1195681661);
	}
	else
	{
		green = _temperature - 60;
		green = int(288.1221695283 * pow(green, -0.0755148492));
	}
	//check boundries
	if (green < 0)  green = 0;
	else if (green > 255) green = 255;

	//blue
	if (_temperature >= 66)
		blue = 255;
	else if (_temperature <= 19)
		blue = 0;
	else
	{
		blue = _temperature - 10;
		blue = int(138.5177312231 * log2(blue) - 305.0447927307);
		if (blue < 0) blue = 0;
		if (blue > 255) blue = 255;
	}

	return Utils::Color8U((uint8)red, (uint8)green, (uint8)blue);

}