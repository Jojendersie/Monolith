#include "material.hpp"
#include "component.hpp"
#include <cassert>
#include <cmath>
#include "..\math\mathbase.hpp"

namespace Voxel {

	const Material Material::UNDEFINED = 0x0;
	const Component Component::UNDEFINED;

	Material::Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _specular, uint8_t _shininess, bool _emissive, uint8_t _variance )
	{
		transparent = 0;
		emissive = _emissive ? 1 : 0;
		specular = _specular;			assert(_specular < 16);
		shininess = _shininess;			assert(_shininess < 16);
		yVar = _variance;				assert(_variance < 16);

		// Encode color to YPbPr:855 discretization.
		y = uint8_t((_r * 299000 + _g * 587000 + _b * 114000) / 1000000);
		pb = uint8_t((- _r * 20513 - _g * 40271 + _b * 60784 + 15500000) / 1000000);
		pr = uint8_t((_r * 60784 - _g * 50899 - _b * 9885 + 15500000) / 1000000);
	}


	Material::Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a, uint8_t _specular, uint8_t _shininess, bool _emissive )
	{
		transparent = 1;
		emissive = _emissive ? 1 : 0;
		specular = _specular;			assert(_specular < 16);
		shininess = _shininess;			assert(_shininess < 16);
		yVar = _a;						assert(_a < 16);

		// Encode color to YPbPr:855 discretization.
		y = uint8_t((_r * 299000 + _g * 587000 + _b * 114000) / 1000000);
		pb = uint8_t((- _r * 20513 - _g * 40271 + _b * 60784 + 15500000) / 1000000);
		pr = uint8_t((_r * 60784 - _g * 50899 - _b * 9885 + 15500000) / 1000000);
	}


	Material::Material( const Material* _materials, int _num )
	{
		// Sum up all things component wise and then divide afterwards.
		// For division use real rounding instead of integer div.
		unsigned nh = _num / 2;
		unsigned tmp = 0;

		// Emissivity determines how colors are summed
		unsigned numEmissive = 0;
		for( int i=0; i<_num; ++i ) { emissive |= _materials[i].emissive; numEmissive += _materials[i].emissive; }
		//emissive = (tmp) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].y;
		y = (tmp + nh) / _num;
		// If only a part of the voxels is emissive reduce luminance -> reduce y
		if( emissive )
			y = y * (4 + numEmissive) / (4 + _num);

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].specular;
		specular = (tmp + nh) / _num;

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].shininess;
		shininess = (tmp + nh) / _num;

		transparent = 1;
		for( int i=0; i<_num; ++i ) transparent &= _materials[i].transparent;

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