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
		SetTransparent(false);
		SetEmissive(_emissive);
		SetSpecular(_specular);			assert(_specular < 16);
		SetShininess(_shininess);		assert(_shininess < 16);
		SetVar(_variance);				assert(_variance < 16);

		// Encode color to YPbPr:855 discretization.
		SetY( uint8_t((_r * 299000 + _g * 587000 + _b * 114000) / 1000000) );
		SetPb( uint8_t((- _r * 20513 - _g * 40271 + _b * 60784 + 15500000) / 1000000) );
		SetPr( uint8_t((_r * 60784 - _g * 50899 - _b * 9885 + 15500000) / 1000000) );
	}


	Material::Material( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a, uint8_t _specular, uint8_t _shininess, bool _emissive )
	{
		SetTransparent(true);
		SetEmissive(_emissive);
		SetSpecular(_specular);			assert(_specular < 16);
		SetShininess(_shininess);		assert(_shininess < 16);
		SetVar(_a);						assert(_a < 16);

		// Encode color to YPbPr:855 discretization.
		SetY( uint8_t((_r * 299000 + _g * 587000 + _b * 114000) / 1000000) );
		SetPb( uint8_t((- _r * 20513 - _g * 40271 + _b * 60784 + 15500000) / 1000000) );
		SetPr( uint8_t((_r * 60784 - _g * 50899 - _b * 9885 + 15500000) / 1000000) );
	}


	Material::Material( const Material* _materials, int _num )
	{
		// Sum up all things component wise and then divide afterwards.
		// For division use real rounding instead of integer div.
		unsigned nh = _num / 2;
		unsigned tmp = 0;

		// Emissivity determines how colors are summed
		unsigned numEmissive = 0;
		for( int i=0; i<_num; ++i ) if(_materials[i].IsEmissive()) ++numEmissive;
		SetEmissive( numEmissive > 0 );

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetY();
		tmp = (tmp + nh) / _num;
		// If only a part of the voxels is emissive reduce luminance -> reduce y
		if( numEmissive > 0 )
			SetY( tmp * (4 + numEmissive) / (4 + _num) );
		else SetY( tmp );

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetSpecular();
		SetSpecular( (tmp + nh) / _num );

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetShininess();
		SetShininess( (tmp + nh) / _num );

		// All must be transparent to inherite
		tmp = 0;
		for( int i=0; i<_num; ++i ) if(_materials[i].IsTransparent()) ++tmp;
		SetTransparent(tmp == _num);

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetPb();
		SetPb( ((tmp + nh) / _num) & 0x1f );

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetPr();
		SetPr( (tmp + nh) / _num );

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetVar();
		SetVar( (tmp + nh) / _num );
	}
} // namespace Voxel