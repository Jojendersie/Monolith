#include "material.hpp"
#include "voxel.hpp"
#include "utilities/assert.hpp"
#include <cmath>
#include "ei/elementarytypes.hpp"

namespace Voxel {

	const Material Material::UNDEFINED = 0x0;
	const Voxel Voxel::UNDEFINED;

	Material::Material( uint8 _r, uint8 _g, uint8 _b, uint8 _specular, uint8 _shininess, bool _emissive, uint8 _variance )
	{
		SetTransparent(false);
		SetEmissive(_emissive);
		SetSpecular(_specular);			Assert(_specular < 16, "Specular is out of range.");
		SetShininess(_shininess);		Assert(_shininess < 16, "Shineness is out of range.");
		SetVar(_variance);				Assert(_variance < 16, "Variance is out of range.");

		// Encode color to YPbPr:855 discretization.
		SetY( uint8((_r * 299000 + _g * 587000 + _b * 114000) / 1000000) );
		SetPb( uint8((- _r * 20513 - _g * 40271 + _b * 60784 + 15500000) / 1000000) );
		SetPr( uint8((_r * 60784 - _g * 50899 - _b * 9885 + 15500000) / 1000000) );
	}


	Material::Material( uint8 _r, uint8 _g, uint8 _b, uint8 _a, uint8 _specular, uint8 _shininess, bool _emissive )
	{
		SetTransparent(true);
		SetEmissive(_emissive);
		SetSpecular(_specular);			Assert(_specular < 16, "Specular is out of range.");
		SetShininess(_shininess);		Assert(_shininess < 16, "Shineness is out of range.");
		SetVar(_a);						Assert(_a < 16, "Variance is out of range.");

		// Encode color to YPbPr:855 discretization.
		SetY( uint8((_r * 299000 + _g * 587000 + _b * 114000) / 1000000) );
		SetPb( uint8((- _r * 20513 - _g * 40271 + _b * 60784 + 15500000) / 1000000) );
		SetPr( uint8((_r * 60784 - _g * 50899 - _b * 9885 + 15500000) / 1000000) );
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
		for( int i=0; i<_num; ++i ) tmp = ei::max(tmp, _materials[i].GetSpecular());
		SetSpecular( tmp );

		tmp = 0;
		for( int i=0; i<_num; ++i ) tmp += _materials[i].GetShininess();
		SetShininess( (tmp + nh) / _num );

		// All must be transparent to inherit
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