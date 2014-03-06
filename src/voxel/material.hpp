#pragma once

#include <stdint.h>
#include <cassert>
#include "voxel.hpp"

namespace Voxel {

	/// \brief A densely packed material information.
	/// \details the information are packed in an order which maximizes 8 bit
	///		alignment.
	struct Material
	{
		uint32_t y: 8;			///< The y from YPbPr.
		uint32_t specular: 4;	///< Exponent of specularity
		uint32_t shininess: 4;	///< Amplitude of specularity
		uint32_t transparent: 1;///< If 1 yVar has to be interpreted as alpha channel.
		uint32_t emissive: 1;	///< This is a light source or not
		uint32_t pb: 5;			///< Discretized Pb color component
		uint32_t pr: 5;			///< Discretized Pr color component
		uint32_t yVar: 4;		///< Amplitude of rotating Y intensity or transparency.

		/// \brief Standard constructor creates total black
		Material()	{ *(uint32_t*)this = 0x07BC0000; }

		/// \brief Allow casting from uint32_t to material
		Material& operator = (uint32_t _value)	{ *(uint32_t*)this = _value; return *this; }
		Material(uint32_t _value)				{ *(uint32_t*)this = _value; }

		/// \brief Full transparent black
		static const Material UNDEFINED;

		/// \brief Constructor to define a material
		/// \param [in] _r Red color channel - will be converted internally.
		/// \param [in] _g Green color channel - will be converted internally.
		/// \param [in] _b Blue color channel - will be converted internally.
		/// \param [in] _specular Amplitude of specular reflections. Interval [0,15].
		/// \param [in] _shininess Exponent of specularity. Interval [0,15]
		///		~ square root of the exponent*2 (exponents are: 0, 0.5, 2, 4.5, 8, ...)
		/// \param [in] _emissive This is a light source.
		/// \param [in] _variance Relative fluctuation of the luminance from the
		///		color. Interval [0,15]: 0%, 6.7%, 13.3%, ....
		Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _specular, uint8_t _shininess, bool _emissive, uint8_t _variance );

		Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a, uint8_t _specular, uint8_t _shininess, bool _emissive );

		/// \brief Compute the average of _num materials.
		/// \param [in] _materials An array of _num materials.
		/// \param [in] _num Size of the given material array.
		Material( const Material* _materials, int _num );

		/// \brief Binary check for equality
		bool operator == (const Material& _mat) const		{ return *(uint32_t*)this == *(uint32_t*)&_mat; }
		bool operator != (const Material& _mat) const		{ return *(uint32_t*)this != *(uint32_t*)&_mat; }
	};


	/// \brief Models are represented by a material and voxel-type tree in one.
#	pragma pack(push, 1)
	struct Component
	{
		Material material;		///< Graphical representation
		VoxelType type;			///< The type of the voxel
		uint8_t dirty: 1;		///< Somebody changed a child or this node
		uint8_t solid: 1;		///< This node and all its children are defined
		uint8_t surface: 6;	///< One flag for each direction if there is no solid neighborhood

		/// \brief Standard constructor creates undefined element
		Component() : material(Material::UNDEFINED), type(VoxelType::UNDEFINED), dirty(0), solid(0), surface(0)	{}

		/// \brief Construct a component with a defined type and undefined material
		Component(VoxelType _type) : material(VOXEL_INFO[(int)_type].material), type(_type), dirty(1), solid(IsSolid(_type)?1:0), surface(0) {}

		/// \brief Mark this component as outdated (it is set to undefined)
		void Touch()			{ dirty = 1; }
		bool IsDirty() const	{ return dirty == 1; }

		/// \brief Undefined material and type.
		static const Component UNDEFINED;

		/// \brief Checks if type is equal
		bool operator == (const Component& _mat) const		{ return type == _mat.type; }
		/// \brief Checks if type is not equal
		bool operator != (const Component& _mat) const		{ return type != _mat.type; }
	};
#	pragma pack(pop)

} // namespace Voxel