/// \file predeclarations.hpp // TODO: test doxygen
/// \brief A list of predeclarations for all classes in alphabetical order.

#pragma once

#include "ei/elementarytypes.hpp"

#include "utilities/logger.hpp"

// External things
struct GLFWwindow;

// General classes without scopes
class GSMainMenu;
class GSPlay;
class GSEditor;
class GSEditorChoice;
class GSGameplayOpt;
class GSGraphicOpt;
class GSInputOpt;
class GSSoundOpt;
class IGameState;
typedef IGameState* IGameStateP;
class Monolith;

//searching for a home(namespace) :-(
class Galaxy;
class Ship;

namespace Mechanics {
	class ComponentSystem;
	class ComputerSystem;
	class ReactorSystem;
	class BatterySystem;
	class ShieldSystem;
	class StorageSystem;
	class SensorSystem;
	class DriveSystem;
}

namespace Generators {
	class Asteroid;
} // namespace Generators

namespace Graphic {
	class Effect;
	class Hud;
	class SamplerState;
	class Texture;
	class UniformBuffer;
	class Font;
	class Framebuffer;
	class Texture;
	class PostProcessing;
	class SingleComponentRenderer;

	namespace Marker {
		class Grid;
		class Box;
		class SphericalFunction;
	}
} // namespace Graphic

namespace Input {
	class Camera;
	class Manager;

	enum struct VirtualKey;
} // namespace Input

namespace ei {
	template<typename T, uint M, uint N>
	class Matrix;
	typedef Matrix<int32, 2, 1> IVec2;
	typedef Matrix<int32, 3, 1> IVec3;
	typedef Matrix<int32, 4, 1> IVec4;
	typedef Matrix<float, 2, 1> Vec2;
	typedef Matrix<float, 3, 1> Vec3;
	typedef Matrix<float, 4, 1> Vec4;
	typedef Matrix<float, 4, 4> Mat4x4;
	typedef Matrix<float, 3, 3> Mat3x3;

	template<typename T>
	class TQuaternion;
	typedef TQuaternion<float> Quaternion;
	struct Sphere;
	struct Ray;
} // namespace

namespace Math {
	class Plane;
	struct WorldRay;

//	typedef Vector<3,FixedPoint<30>> FixVec3;
	class Transformation;
	template<int N, typename T> class CubeMap;
	/// A typedef to have global control about the tessellation refinement
	typedef CubeMap<3, ei::Vec4> SphericalFunction;
} // namespace Math

namespace Utils {
	class Color32F;
	class Color8U;
} // namespace Utils


namespace Voxel {
	class Chunk;
	class Model;
	enum struct ComponentType: uint16;
	class VoxelVertex;
	struct Material;
	struct Voxel;
} // namespace Voxel
