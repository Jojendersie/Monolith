#include "color.hpp"

namespace Utils {

	Color32F::Color32F(const HSV& _hsv)
	{
		float hue = _hsv[0] * 6.0f;
		float c = _hsv[1] * _hsv[2];
		float X = c * (1.0f - fabs(fmod(hue, 2.0f) - 1.0f));
		float m = _hsv[2] - c;

		A() = 1.0f;	// Alpha channel

		if(hue < 1.0f)			{ R() = c + m; G() = X + m; B() = m; }
		else if(hue < 2.0f)		{ R() = X + m; G() = c + m; B() = m; }
		else if(hue < 3.0f)		{ R() = m; G() = c + m; B() = X + m; }
		else if(hue < 4.0f)		{ R() = m; G() = X + m; B() = c + m; }
		else if(hue < 5.0f)		{ R() = X + m; G() = m; B() = c + m; }
		else					{ R() = c + m; G() = m; B() = X + m; }
	}

	Color32F::Color32F(const HSL& _hsl)
	{
		float hue = _hsl[0] * 6.0f;
		float c = (1.0f - fabs(2.0f * _hsl[2] - 1.0f)) * _hsl[1];
		float X = c * (1.0f - fabs(fmod(hue, 2.0f) - 1.0f));
		float m = _hsl[2] - 0.5f * c;

		A() = 1.0f;	// Alpha channel

		if(hue < 1.0f)			{ R() = c + m; G() = X + m; B() = m; }
		else if(hue < 2.0f)		{ R() = X + m; G() = c + m; B() = m; }
		else if(hue < 3.0f)		{ R() = m; G() = c + m; B() = X + m; }
		else if(hue < 4.0f)		{ R() = m; G() = X + m; B() = c + m; }
		else if(hue < 5.0f)		{ R() = X + m; G() = m; B() = c + m; }
		else					{ R() = c + m; G() = m; B() = X + m; }
	}

	Color32F::Color32F(const sRGB& _srgb)
	{
		R() = _srgb[0] <= 0.04045f ? (_srgb[0] / 12.92f) : (pow((_srgb[0] + 0.055f) / 1.055f, 2.4f));
		G() = _srgb[1] <= 0.04045f ? (_srgb[1] / 12.92f) : (pow((_srgb[1] + 0.055f) / 1.055f, 2.4f));
		B() = _srgb[2] <= 0.04045f ? (_srgb[2] / 12.92f) : (pow((_srgb[2] + 0.055f) / 1.055f, 2.4f));
		A() = 1.0f;	// Alpha channel
	}

	Color32F::Color32F(const YPbPr& _ybr)
	{
		R() = _ybr[0] +                          1.402f * _ybr[2];
		G() = _ybr[0] - 0.344136f * _ybr[1] - 0.714136f * _ybr[2];
		B() = _ybr[0] +    1.772f * _ybr[1];
		A() = 1.0f;	// Alpha channel
	}


	HSV Color32F::ToHSV() const
	{
		HSV hsv;

		// Value
		hsv[2] = Math::max(R(), Math::max(G(), B()));
		// Black?
		if(fabs(hsv[2]) < Math::EPSILON) {hsv[0] = hsv[1] = 0.0f; return hsv;}

		// Compute non-normalized saturation
		float cmin = Math::min(R(), Math::min(G(), B()));
		hsv[1] = hsv[2] - cmin;
		if(fabs(hsv[1]) < Math::EPSILON) {hsv[0] = 0.0f; return hsv;}

		// Normalize saturation
		hsv[1] /= hsv[2];

		// Compute hue
		if (hsv[2] == R())
			hsv[0] = 1.0f + (G() - B()) / (6.0f * (hsv[2] - cmin));
		else if (hsv[2] == G())
			hsv[0] = 1.0f/3.0f + (B() - R()) / (6.0f * (hsv[2] - cmin));
		else
			hsv[0] = 2.0f/3.0f + (R() - G()) / (6.0f * (hsv[2] - cmin));

		// Angle modulo one (period)
		hsv[0] = hsv[0] - (int)hsv[0];

		return hsv;
	}

	HSL Color32F::ToHSL() const
	{
		HSL hsl;

		// Lightness
		float cmax = Math::max(R(), Math::max(G(), B()));
		float cmin = Math::min(R(), Math::min(G(), B()));
		hsl[2] = 0.5f * (cmin + cmax);
		// Black?
		if(fabs(cmax) < Math::EPSILON) {hsl[0] = hsl[1] = 0.0f; return hsl;}

		// Compute non-normalized saturation
		hsl[1] = cmax - cmin;
		if(fabs(hsl[1]) < Math::EPSILON) {hsl[0] = 0.0f; return hsl;}

		// Normalize saturation
		hsl[1] /= 1.0f - fabs(2.0f * hsl[2] - 1.0f);

		// Compute hue
		if (cmax == R())
			hsl[0] = 1.0f + (G() - B()) / (6.0f * (cmax - cmin));
		else if (cmax == G())
			hsl[0] = 1.0f/3.0f + (B() - R()) / (6.0f * (cmax - cmin));
		else
			hsl[0] = 2.0f/3.0f + (R() - G()) / (6.0f * (cmax - cmin));

		// Angle modulo one (period)
		hsl[0] = hsl[0] - (int)hsl[0];

		return hsl;
	}

	sRGB Color32F::ToSRGB() const
	{
		sRGB srgb;
		srgb[0] = R() <= 0.0031308f ? (12.92f * R()) : (1.055f * pow(R(), 1.0f / 2.4f) - 0.055f);
		srgb[1] = G() <= 0.0031308f ? (12.92f * G()) : (1.055f * pow(G(), 1.0f / 2.4f) - 0.055f);
		srgb[2] = B() <= 0.0031308f ? (12.92f * B()) : (1.055f * pow(B(), 1.0f / 2.4f) - 0.055f);
		return srgb;
	}

	Utils::YPbPr Color32F::ToYPbPr() const
	{
		YPbPr ybr;
		ybr[0] =      0.299f * R() +    0.587f * G() +    0.114f * B();
		ybr[1] = - 0.168736f * R() - 0.331264f * G() +      0.5f * B();
		ybr[2] =        0.5f * R() - 0.418688f * G() - 0.081312f * B();
		return ybr;
	}

} // namespace Utils