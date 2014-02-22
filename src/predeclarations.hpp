/// \file predeclarations.hpp // TODO: test doxygen
/// \brief A list of predeclarations for all classes in alphabetical order.

#pragma once

#include <cstdint>

// External things
struct GLFWwindow;

// General classes without scopes
class GSMain;
class IGameState;
typedef IGameState* IGameStateP;
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
	class Plane;
	struct Sphere;
	template<int n, class Data>	class Vector;
	typedef Vector<2,int> IVec2;
	typedef Vector<3,int> IVec3;
	typedef Vector<4,int> IVec4;
	typedef Vector<2,float> Vec2;
	typedef Vector<3,float> Vec3;
	typedef Vector<4,float> Vec4;
} // namespace Math

namespace Utils {
	class Color32F;
	class Color8U;
} // namespace Utils


namespace Voxel {
	class Chunk;
	class Model;
	enum struct VoxelType: uint8_t;
	struct VoxelTypeInfo;
	struct VoxelVertex;
} // namespace Voxel
