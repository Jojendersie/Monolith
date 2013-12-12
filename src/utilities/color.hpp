#pragma once

#include "../math/vector3.hpp"
#include "../math/vector4.hpp"
#include "../predeclarations.hpp"
#include <cstdint>

namespace Utils {

	struct HSV: public Math::Vec3 {};
	struct HSL: public Math::Vec3 {};
	struct sRGB: public Math::Vec3 {};
	struct YPbPr: public Math::Vec3 {};

	/// \brief Main RGBA color class used for all computations.
	/// \brief Inherits many operators from 4D vectors.
	class Color32F: public Math::Vec4
	{
	public:
		/// \brief Default: Uninitialized for speed
		Color32F()												{}

		/// \brief From other color class for implicit casting
		Color32F(const Color8U& _color)							{*this = static_cast<Color32F>(_color); }

		/// \brief From single bytes
		Color32F(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255)	{ x = r*(1.0f/255.0f); y = r*(1.0f/255.0f); z = r*(1.0f/255.0f); w = r*(1.0f/255.0f); }

		/// \brief From single floats
		Color32F(float r, float g, float b, float a=1.0f)		{ x = r; y = g; z = b; w = a; }

		/// \brief From HSV color space
		Color32F(const HSV& _hsv);

		/// \brief From HSL color space
		Color32F(const HSL& _hsl);

		/// \brief From non-linear sRGB color space
		Color32F(const sRGB& _srgb);

		/// \brief From YPbPr color space
		Color32F(const YPbPr& _ybr);

		/// \brief Returns if all 4 channels are in the Range [0,1].
		bool IsNormalized() const								{ return x <= 1.0f && y <= 1.0f && z <= 1.0f && w <= 1.0f && x >= 0.0f && y >= 0.0f && z >= 0.0f && w >= 0.0f; }

		/// TODO
		const Color32F&	Normalize();

		/// \brief Access single color component: Red
		float R() const										{ return x; }
		float& R()											{ return x; }

		/// \brief Access single color component: Green
		float G() const										{ return y; }
		float& G()											{ return y; }

		/// \brief Access single color component: Blue
		float B() const										{ return z; }
		float& B()											{ return z; }

		/// \brief Access single color component: Alpha
		float A() const										{ return w; }
		float& A()											{ return w; }

		/// \brief Convert color to HSV color space.
		/// \details Values are given as:
		///		* Hue: angle in [0,1)
		///		* Saturation: [0,1]
		///		* Value: [0,1] = max(r,g,b)
		///		http://en.wikipedia.org/wiki/HSV_color_space
		///		http://en.literateprograms.org/RGB_to_HSV_color_space_conversion_%28C%29
		HSV ToHSV() const;

		/// \brief Convert color to HSL color space.
		/// \details Values are given as:
		///		* Hue: angle in [0,1)
		///		* Saturation: [0,1]
		///		* Lightness: [0,1] = (max(r,g,b)+min(r,g,b))/2
		HSL ToHSL() const;

		/// \brief Convert linear rgb to sRGB color space.
		sRGB ToSRGB() const;

		/// \brief Convert to YPbPr color space (ITU-R BT.601).
		/// \details All values are in [0,1]. This is the difference to YCbCr
		///		where values are discretized for the 8 bit range.
		///		
		///		The conversion from YPbPr to YCbCr is:
		///		YPbPr * Vec3(219,224,224) + Vec3(16,128,128)
		YPbPr ToYPbPr() const;

		/// \brief Compute relative luminance (ITU-R BT.709).
		/// http://en.wikipedia.org/wiki/Luminance_%28relative%29
		float Luminance() const		{ return 0.2126f * x + 0.7152f * y + 0.0722f * z; }
	};




	/// \brief 32Bit color class for storage.
	/// \details This class is for read only purposes only. For any operation
	///		on colors or to get derived attributes convert it to Color32F (cast
	///		is implemented implicit).
	class Color8U
	{
	public:
		/// \brief From main color class
		Color8U(const Color32F& _color)							{ m_color = 0; for(int i=0; i<4; ++i) { m_color<<=8; m_color |= uint8_t(Math::saturate(_color[i]) * 255); } }

		/// \brief From single RGBA 32 bit integer
		Color8U(const uint32_t& _color) : m_color(_color)		{}

		/// \brief Default: Uninitialized for speed
		Color8U()												{}

		/// \brief From single bytes
		Color8U(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255)	{ m_color = (r<<24) | (g<<16) | (b<<8) | a; }

		/// \brief From single floats
		Color8U(float r, float g, float b, float a=1.0f)		{ m_color = (uint8_t(Math::saturate(r)*255.0f)<<24) | (uint8_t(Math::saturate(g)*255.0f)<<16) | (uint8_t(Math::saturate(b)*255.0f)<<8) | uint8_t(Math::saturate(a)*255.0f); }

		/// \brief Conversion to main color class
		operator Color32F() const								{ return Color32F(R(), G(), B(), A()); }

		/// \brief Access single color component: Red
		uint8_t R() const										{ return m_color >> 24; }

		/// \brief Access single color component: Green
		uint8_t G() const										{ return m_color >> 16; }

		/// \brief Access single color component: Blue
		uint8_t B() const										{ return m_color >> 8; }

		/// \brief Access single color component: Alpha
		uint8_t A() const										{ return m_color; }

		/// \brief Returns true, if both colors are identical in all components.
		bool operator==( const Color8U& _c) const				{ return m_color == _c.m_color; }

		/// \brief Returns true, if both colors are not identical in all components.
		bool operator!=( const Color8U& _c) const				{ return m_color != _c.m_color; }

	protected:
		uint32_t m_color;		///< The RGBA bytes
	};

} // namespace Utils 