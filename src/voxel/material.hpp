#pragma once

#include <cstdint>
#include <cassert>
#include "../math/math.hpp"

namespace Voxel {

	/// \brief A densely packed material information.
	/// \details the information are packed in an order which maximizes 8 bit
	///		alignment.
	struct Material
	{
		uint32_t code;
		/// Set the y from YPbPr.
		void SetY(uint32_t _y)					{ code = (code & 0xffffff00) | (_y & 0xff); }
		/// Get the y from YPbPr.
		uint32_t GetY() const					{ return code & 0xff; }
		// Set exponent of specularity
		void SetSpecular(uint32_t _specular)	{ code = (code & 0xfffff0ff) | ((_specular & 0xf) << 8); }
		/// Get exponent of specularity
		uint32_t GetSpecular() const			{ return (code >> 8) & 0xf; }
		/// Set amplitude of specularity
		void SetShininess(uint32_t _shininess)	{ code = (code & 0xffff0fff) | ((_shininess & 0xf) << 12); }
		/// Get amplitude of specularity
		uint32_t GetShininess() const			{ return (code >> 12) & 0xf; }
		/// If 1 Var has to be interpreted as alpha channel.
		void SetTransparent(bool _transparent)	{ code = (code & 0xfffeffff) | (_transparent ? 0x00010000 : 0); }
		/// Is there an animation or transparents value? (Var)
		bool IsTransparent() const				{ return (code >> 16) & 0x1; }
		/// Set this is a light source or not
		void SetEmissive(bool _emissive)		{ code = (code & 0xfffdffff) | (_emissive ? 0x00020000 : 0); }
		/// Is this a light source or not?
		bool IsEmissive() const					{ return (code >> 17) & 0x1; }
		/// Set discretized Pb color component
		void SetPb(uint32_t _pb)				{ code = (code & 0xff83ffff) | ((_pb & 0x1f) << 18); }
		/// Get discretized Pb color component
		uint32_t GetPb() const					{ return (code >> 18) & 0x1f; }
		/// Set discretized Pr color component
		void SetPr(uint32_t _pr)				{ code = (code & 0xf07fffff) | ((_pr & 0x1f) << 23); }
		/// Get discretized Pr color component
		uint32_t GetPr() const					{ return (code >> 23) & 0x1f; }
		/// Get amplitude of rotating Y intensity or transparency
		void SetVar(uint32_t _var)				{ code = (code & 0x0fffffff) | ((_var & 0xf) << 28); }
		/// Set amplitude of rotating Y intensity or transparency
		uint32_t GetVar() const					{ return (code >> 28) & 0xf; }



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

} // namespace Voxel