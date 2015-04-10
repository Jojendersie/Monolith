#include "sceneobject.hpp"
#include "math/transformation.hpp"
#include "predeclarations.hpp"
#include "utilities/color.hpp"

/// \brief Information required by shader to draw a single star
struct StarVertex
{
	Math::Vec3 position;
	uint32_t color;
	float size;

};

/// \brief A combination of stars and planets

class StarSystem : public Math::Transformation
{
public:
	StarSystem(const Math::FixVec3& _position, int _temperature, const Math::Quaternion& _rotation = Math::Quaternion());

	StarVertex& ComputeVertex(const Input::Camera& _camera);
private:
	/// \brief Converts a given temperature in kelvin into a coresponding
	/// RGB-color.
	/// \details derivation and code found here: http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
	static Utils::Color8U TemperatureToRGB(int _temperature);

	StarVertex m_vertex;
};