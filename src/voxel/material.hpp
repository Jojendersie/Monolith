#pragma once

#include <stdint.h>

namespace Voxel {

	/// \brief A densely packed material information.
	/// \details the information are packed in an order which maximizes 8 bit
	///		alignment.
	struct Material
	{
		uint32_t y: 8;			///< The y from YPbPr.
		uint32_t specular: 4;	///< Amplitude of specularity
		uint32_t shininess: 4;	///< Exponent of specularity
		uint32_t transparent: 1;///< If 1 yVar has to be interpreted as alpha channel. Octree related: DIRTY: The material requires an update from the hierarchy.
		uint32_t emissive: 1;	///< This is a light source or not
		uint32_t pb: 5;			///< Discretized Pb color component
		uint32_t pr: 5;			///< Discretized Pr color component
		uint32_t yVar: 4;		///< Amplitude of rotating Y intensity.

		/// \brief Standard constructor creates total black (all 0)
		Material()	{ *(uint32_t*)this = 0; }

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
	};




	// Inline implementations


	Material::Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _specular, uint8_t _shininess, bool _emissive, uint8_t _variance )
	{
		transparent = 0;
		emissive = _emissive ? 1 : 0;
		specular = _specular;			assert(_specular < 16);
		shininess = _shininess;			assert(_shininess < 16);
		yVar = _variance;				assert(_variance < 16);

		// Encode color to YPbPr:855 discretization.
		y = uint8_t((_r * 77 + _g * 150 + _b * 29) / 256);
		pb = uint8_t((- _r * 43 - _g * 85 + _b * 128 + 32640) / 2040);
		pr = uint8_t((_r * 128 - _g * 107 - _b * 21 + 32640) / 2040);
	}





	Material::Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a, uint8_t _specular, uint8_t _shininess, bool _emissive )
	{
		transparent = 1;
		emissive = _emissive ? 1 : 0;
		specular = _specular;			assert(_specular < 16);
		shininess = _shininess;			assert(_shininess < 16);
		yVar = _a;						assert(_a < 16);

		// Encode color to YPbPr:855 discretization.
		y = uint8_t((_r * 77 + _g * 150 + _b * 29) / 256);
		pb = uint8_t((- _r * 43 - _g * 85 + _b * 128 + 32640) / 2040);
		pr = uint8_t((_r * 128 - _g * 107 - _b * 21 + 32640) / 2040);
	}





	Material::Material( const Material* _materials, int _num )
	{
		// Sum up all things component wise and then divide afterwards.
		// For division use real rounding instead of integer div.
		unsigned nh = _num / 2;
		unsigned tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].y;
		y = (tmp + nh) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].specular;
		specular = (tmp + nh) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].shininess;
		shininess = (tmp + nh) / _num;

		transparent = 1;
		for( int i=0; i<_num; ++i ) transparent &= _materials[i].transparent;

		tmp = 0;
		for( int i=0; i<_num; ++i ) emissive += _materials[i].specular;
		emissive = (tmp + nh) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].pb;
		pb = (tmp + nh) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].pr;
		pr = (tmp + nh) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].yVar;
		yVar = (tmp + nh) / _num;
	}

} // namespace Voxel