#include "sceneobject.hpp"
#include "math/transformation.hpp"
#include "predeclarations.hpp"
#include "utilities/color.hpp"

/// \brief Information required by shader to draw a single star
struct StarVertex
{
	ei::Vec3 position; ///< position in viewspac
	uint32 color;
	float size;
};

/// \brief A combination of stars and planets

class StarSystem : public Math::Transformation
{
public:
	StarSystem(const Math::FixVec3& _position, int _temperature, 
		float _size = 1.f, const ei::Quaternion& _rotation = ei::Quaternion());

	StarVertex& ComputeVertex(const Input::Camera& _camera);

	/// \brief Converts a given temperature in kelvin into a coresponding
	/// RGB-color.
	/// \details derivation and code found here: http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
	static Utils::Color8U TemperatureToRGB(int _temperature);
private:

	StarVertex m_vertex;
};