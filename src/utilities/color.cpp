#include "color.hpp"

namespace Utils {

	Color32F::Color32F(const HSV& _hsv)
	{
		float hue = _hsv.x * 6.0f;
		float c = _hsv.y * _hsv.z;
		float X = c * (1.0f - fabs(fmod(hue, 2.0f) - 1.0f));
		float m = _hsv.z - c;

		w = 1.0f;	// Alpha channel

		if(hue < 1.0f)			{ x = c + m; y = X + m; z = m; }
		else if(hue < 2.0f)		{ x = X + m; y = c + m; z = m; }
		else if(hue < 3.0f)		{ x = m; y = c + m; z = X + m; }
		else if(hue < 4.0f)		{ x = m; y = X + m; z = c + m; }
		else if(hue < 5.0f)		{ x = X + m; y = m; z = c + m; }
		else					{ x = c + m; y = m; z = X + m; }
	}

	Color32F::Color32F(const HSL& _hsl)
	{
		float hue = _hsl.x * 6.0f;
		float c = (1.0f - fabs(2.0f * _hsl.z - 1.0f)) * _hsl.y;
		float X = c * (1.0f - fabs(fmod(hue, 2.0f) - 1.0f));
		float m = _hsl.z - 0.5f * c;

		w = 1.0f;	// Alpha channel

		if(hue < 1.0f)			{ x = c + m; y = X + m; z = m; }
		else if(hue < 2.0f)		{ x = X + m; y = c + m; z = m; }
		else if(hue < 3.0f)		{ x = m; y = c + m; z = X + m; }
		else if(hue < 4.0f)		{ x = m; y = X + m; z = c + m; }
		else if(hue < 5.0f)		{ x = X + m; y = m; z = c + m; }
		else					{ x = c + m; y = m; z = X + m; }
	}

	Color32F::Color32F(const sRGB& _srgb)
	{
		x = _srgb.x <= 0.04045f ? (_srgb.x / 12.92f) : (pow((_srgb.x + 0.055f) / 1.055f, 2.4f));
		y = _srgb.y <= 0.04045f ? (_srgb.y / 12.92f) : (pow((_srgb.y + 0.055f) / 1.055f, 2.4f));
		z = _srgb.z <= 0.04045f ? (_srgb.z / 12.92f) : (pow((_srgb.z + 0.055f) / 1.055f, 2.4f));
		w = 1.0f;	// Alpha channel
	}

	Color32F::Color32F(const YPbPr& _ybr)
	{
		x = _ybr.x +                         1.402f * _ybr.z;
		y = _ybr.x - 0.344136f * _ybr.y - 0.714136f * _ybr.z;
		z = _ybr.x +    1.772f * _ybr.y;
		w = 1.0f;	// Alpha channel
	}


	HSV Color32F::ToHSV() const
	{
		HSV hsv;

		// Value
		hsv.z = Math::max(x, Math::max(y, z));
		// Black?
		if(fabs(hsv.z) < Math::EPSILON) {hsv.x = hsv.y = 0.0f; return hsv;}

		// Compute non-normalized saturation
		float cmin = Math::min(x, Math::min(y, z));
		hsv.y = hsv.z - cmin;
		if(fabs(hsv.y) < Math::EPSILON) {hsv.x = 0.0f; return hsv;}

		// Normalize saturation
		hsv.y /= hsv.z;

		// Compute hue
		if (hsv.z == x)
			hsv.x = 1.0f + (y - z) / (6.0f * (hsv.z - cmin));
		else if (hsv.z == y)
			hsv.x = 1.0f/3.0f + (z - x) / (6.0f * (hsv.z - cmin));
		else
			hsv.x = 2.0f/3.0f + (x - y) / (6.0f * (hsv.z - cmin));

		// Angle modulo one (period)
		hsv.x = hsv.x - (int)hsv.x;

		return hsv;
	}

	HSL Color32F::ToHSL() const
	{
		HSL hsl;

		// Lightness
		float cmax = Math::max(x, Math::max(y, z));
		float cmin = Math::min(x, Math::min(y, z));
		hsl.z = 0.5f * (cmin + cmax);
		// Black?
		if(fabs(cmax) < Math::EPSILON) {hsl.x = hsl.y = 0.0f; return hsl;}

		// Compute non-normalized saturation
		hsl.y = cmax - cmin;
		if(fabs(hsl.y) < Math::EPSILON) {hsl.x = 0.0f; return hsl;}

		// Normalize saturation
		hsl.y /= 1.0f - fabs(2.0f * hsl.z - 1.0f);

		// Compute hue
		if (cmax == x)
			hsl.x = 1.0f + (y - z) / (6.0f * (cmax - cmin));
		else if (cmax == y)
			hsl.x = 1.0f/3.0f + (z - x) / (6.0f * (cmax - cmin));
		else
			hsl.x = 2.0f/3.0f + (x - y) / (6.0f * (cmax - cmin));

		// Angle modulo one (period)
		hsl.x = hsl.x - (int)hsl.x;

		return hsl;
	}

	sRGB Color32F::ToSRGB() const
	{
		sRGB srgb;
		srgb.x = x <= 0.0031308f ? (12.92f * x) : (1.055f * pow(x, 1.0f / 2.4f) - 0.055f);
		srgb.y = y <= 0.0031308f ? (12.92f * y) : (1.055f * pow(y, 1.0f / 2.4f) - 0.055f);
		srgb.z = z <= 0.0031308f ? (12.92f * z) : (1.055f * pow(z, 1.0f / 2.4f) - 0.055f);
		return srgb;
	}

	Utils::YPbPr Color32F::ToYPbPr() const
	{
		YPbPr ybr;
		ybr.x =      0.299f * x +    0.587f * y +    0.114f * z;
		ybr.y = - 0.168736f * x - 0.331264f * y +      0.5f * z;
		ybr.z =        0.5f * x - 0.418688f * y - 0.081312f * z;
		return ybr;
	}

} // namespace Utils