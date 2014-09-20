#pragma once

#include <cstdint>
#include "voxel.hpp"
//#include "../math/math.hpp"

namespace Voxel {

	/// \brief Models are represented by a material and voxel-type tree in one.
#	pragma pack(push, 1)
	struct Component
	{
		Material material;		    ///< Graphical representation
		VoxelType type;			    ///< The type of the voxel
		uint16_t dirty: 1;		    ///< Somebody changed a child or this node (flag is cleared by the render thread)
        uint16_t dirtyPhysics: 1;   ///< Somebody changed a child or this node (flag is cleared by the physics thread)
		uint16_t solid: 1;		    ///< This node and all its children are defined
		uint16_t surface: 6;        ///< One flag for each direction if there is no solid neighborhood

		/// \brief Standard constructor creates undefined element
		Component() : material(Material::UNDEFINED), type(VoxelType::UNDEFINED), dirty(0), solid(0), surface(0)	{}

		/// \brief Construct a component with a defined type and undefined material
		Component(VoxelType _type) : type(_type), dirty(1), solid(TypeInfo::IsSolid(_type)?1:0), surface(0) { uint8_t dummy; TypeInfo::Sample(_type, Math::IVec3(0), 0, 0, material, dummy); }

		/// \brief Mark this component as outdated (it is set to undefined)
		void Touch()			{ dirty = 1; dirtyPhysics = 1; }
		bool IsDirty() const	{ return dirty == 1; }
        bool IsPhysicsDirty() const	{ return dirtyPhysics == 1; }

		/// \brief Undefined material and type.
		static const Component UNDEFINED;

		/// \brief Checks if type is equal
		bool operator == (const Component& _mat) const		{ return type == _mat.type; }
		/// \brief Checks if type is not equal
		bool operator != (const Component& _mat) const		{ return type != _mat.type; }
	};
#	pragma pack(pop)

} // namespace Voxel