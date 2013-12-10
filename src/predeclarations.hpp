/// \file predeclarations.hpp // TODO: test doxygen
/// \brief A list of predeclarations for all classes in alphabetical order.

#pragma once

// External things
struct GLFWwindow;

// General classes without scopes
class GSMain;
class IGameState;
class Monolith;

namespace Generators {
	class Asteroid;
} // namespace Generators

namespace Graphic {
	class Effect;
	class Font;
	class TextRender;
	class SamplerState;
	class Texture;
	class UniformBuffer;
	struct Content;
} // namespace Graphic

namespace Input {
	class Camera;
	class Manager;

	enum struct Keys;
} // namespace Input

namespace Math {
	class Mat4x4;
	class Mat3x3;
	class Quaternion;
	struct Sphere;
	class Vec2;
	class Vec3;
	class Vec4;
} // namespace Math