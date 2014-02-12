#include "material.hpp"
#include <assert.h>

namespace Voxel {

	static const Material UNDEFINED = 0x0000800f;

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