#pragma once

#include "utilities/assert.hpp"
#include <ei/elementarytypes.hpp>

namespace Voxel {

	/// \brief A densely packed material information.
	/// \details the information are packed in an order which maximizes 8 bit
	///		alignment.
	struct Material
	{
		uint32 code;
		/// Set the y from YPbPr.
		void SetY(uint32 _y)					{ code = (code & 0xffffff00) | (_y & 0xff); }
		/// Get the y from YPbPr.
		uint32 GetY() const						{ return code & 0xff; }
		// Set exponent of specularity
		void SetSpecular(uint32 _specular)		{ code = (code & 0xfffff0ff) | ((_specular & 0xf) << 8); }
		/// Get exponent of specularity
		uint32 GetSpecular() const				{ return (code >> 8) & 0xf; }
		/// Set amplitude of specularity
		void SetShininess(uint32 _shininess)	{ code = (code & 0xffff0fff) | ((_shininess & 0xf) << 12); }
		/// Get amplitude of specularity
		uint32 GetShininess() const				{ return (code >> 12) & 0xf; }
		/// If 1 Var has to be interpreted as alpha channel.
		void SetTransparent(bool _transparent)	{ code = (code & 0xfffeffff) | (_transparent ? 0x00010000 : 0); }
		/// Is there an animation or transparents value? (Var)
		bool IsTransparent() const				{ return (code >> 16) & 0x1; }
		/// Set this is a light source or not
		void SetEmissive(bool _emissive)		{ code = (code & 0xfffdffff) | (_emissive ? 0x00020000 : 0); }
		/// Is this a light source or not?
		bool IsEmissive() const					{ return (code >> 17) & 0x1; }
		/// Set discretized Pb color component
		void SetPb(uint32 _pb)					{ code = (code & 0xff83ffff) | ((_pb & 0x1f) << 18); }
		/// Get discretized Pb color component
		uint32 GetPb() const					{ return (code >> 18) & 0x1f; }
		/// Set discretized Pr color component
		void SetPr(uint32 _pr)					{ code = (code & 0xf07fffff) | ((_pr & 0x1f) << 23); }
		/// Get discretized Pr color component
		uint32 GetPr() const					{ return (code >> 23) & 0x1f; }
		/// Get amplitude of rotating Y intensity or transparency
		void SetVar(uint32 _var)				{ code = (code & 0x0fffffff) | ((_var & 0xf) << 28); }
		/// Set amplitude of rotating Y intensity or transparency
		uint32 GetVar() const					{ return (code >> 28) & 0xf; }



		/// \brief Standard constructor creates total black
		Material()	{ *(uint32*)this = 0x07BC0000; }

		/// \brief Allow casting from uint32 to material
		Material& operator = (uint32 _value)	{ *(uint32*)this = _value; return *this; }
		Material(uint32 _value)				{ *(uint32*)this = _value; }

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
		Material( uint8 _r, uint8 _g, uint8 _b, uint8 _specular, uint8 _shininess, bool _emissive, uint8 _variance );

		Material( uint8 _r, uint8 _g, uint8 _b, uint8 _a, uint8 _specular, uint8 _shininess, bool _emissive );

		/// \brief Compute the average of _num materials.
		/// \param [in] _materials An array of _num materials.
		/// \param [in] _num Size of the given material array.
		Material( const Material* _materials, int _num );

		/// \brief Binary check for equality
		bool operator == (const Material& _mat) const		{ return *(uint32*)this == *(uint32*)&_mat; }
		bool operator != (const Material& _mat) const		{ return *(uint32*)this != *(uint32*)&_mat; }
	};

} // namespace Voxel