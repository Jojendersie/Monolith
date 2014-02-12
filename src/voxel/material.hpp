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
		uint32_t specular: 4;	///< Amplitude of specularity
		uint32_t shininess: 4;	///< Exponent of specularity
		uint32_t transparent: 1;///< If 1 yVar has to be interpreted as alpha channel.
		uint32_t emissive: 1;	///< This is a light source or not
		uint32_t pb: 5;			///< Discretized Pb color component
		uint32_t pr: 5;			///< Discretized Pr color component
		uint32_t yVar: 4;		///< Amplitude of rotating Y intensity or transparency.

		/// \brief Standard constructor creates total black (all 0)
		Material()	{ *(uint32_t*)this = 0; }

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


	/// \brief A struct with the same size as the material containing per voxel
	///		information in an octree.
/*	struct Component
	{
		VoxelType type;			///< The type of the voxel
		/// \brief Coding of the rotation of the component
		/// \details There are 24 combinations possible but for better speed
		///		a direct coordinate mapping is encoded (additional support
		///		for mirroring).
		///		ori_x/ori_y/ori_z Index of the coordinate which is mapped
		///		to x/y/z respectively.
		///		ori_xr/... - 0 or 1 if the coordinate should be reflected.
		///		
		///		Example:
		///		This is how the final access position is determined:
		///		´accessX = ori_xr ? max - sampleX[ori_x] : sampleX[ori_x];´
		uint16_t orientation;

		/// \brief Convert materials to components for the case of mixed trees.
		/// \details This operator does no logical conversation!
		Component( const Material& _material )	{ assert(sizeof(Component) == sizeof(Material)); assert(sizeof(Component) == 4); *(uint32_t*)this = *(uint32_t*)&_material; }

		Component() : type(VoxelType::UNDEFINED)	{}
	};

	/// \brief Trees uses Components on leaf level and Materials for internal
	///		nodes.
	/*struct ComponentMaterialElement
	{
		uint32_t data;

		operator Material
		{
			Material material;
			Component component;
		};
	}*/

} // namespace Voxel