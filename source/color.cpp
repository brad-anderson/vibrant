#include "pch.hpp"

#include "vibrant/color.hpp"

namespace vibrant {

Rgb::operator Hsl() const
{
	assert(!(r < 0 || r > 1) && !(g < 0 || g > 1) && !(b < 0 || b > 1));

	double min = std::min(r, std::min(g, b));
	double max = std::max(r, std::max(g, b));
	double delta = max - min;

	Hsl color{ 0, 0, (max + min) / 2 };

	if (fabs(delta) < 0.000001)
	{
		color.h = 0;
		color.s = 0;
	}
	else
	{
		if (color.l < 0.5)
			color.s = delta / (max + min);
		else
			color.s = delta / (2.0 - max - min);

		double delta_r = (((max - r) / 6.0) + (delta / 2.0)) / delta;
		double delta_g = (((max - g) / 6.0) + (delta / 2.0)) / delta;
		double delta_b = (((max - b) / 6.0) + (delta / 2.0)) / delta;

		if (r == max)
			color.h = delta_b - delta_g;
		else if (g == max)
			color.h = (1 / 3.0) + delta_r - delta_b;
		else if (b == max)
			color.h = (2 / 3.0) + delta_g - delta_r;

		if (color.h < 0)
			color.h += 1;
		if (color.h > 1)
			color.h -= 1;
	}

	return color;
}

namespace { double hue_to_rgb(double v1, double v2, double h)
{
	if( h < 0 )
		h += 1;
	if( h > 1 )
		h -= 1;
	if( 6 * h < 1 )
		return v1 + ( v2 - v1 ) * 6 * h;
	if( 2 * h < 1 )
		return v2;
	if( 3 * h < 2 )
		return v1 + ( v2 - v1 ) * ( ( 2 / 3.0 ) - h ) * 6;
	
	return v1;
} }

Rgb::operator Hsv() const
{
	assert(!(r < 0 || r > 1) &&
		   !(g < 0 || g > 1) &&
		   !(b < 0 || b > 1));

	double min = std::min(r, std::min(g, b));
	double max = std::max(r, std::max(g, b));
	double delta = max - min;

	Hsv color{ 0, 0, max };

	if(fabs(delta) < 0.000001)
	{
		color.h = 0;
		color.s = 0;
	}
	else
	{
		color.s = delta / max;

		double delta_r = (((max - r) / 6.0) + (delta / 2.0)) / delta;
		double delta_g = (((max - g) / 6.0) + (delta / 2.0)) / delta;
		double delta_b = (((max - b) / 6.0) + (delta / 2.0)) / delta;

		if(r == max)
			color.h = delta_b - delta_g;
		else if(g == max)
			color.h = (1 / 3.0) + delta_r - delta_b;
		else if(b == max)
			color.h = (2 / 3.0) + delta_g - delta_r;

		if(color.h < 0)
			color.h += 1;
		if(color.h > 1)
			color.h -= 1;
	}

	return color;
}

Hsl::operator Rgb() const
{
	assert(!(h < 0 || h > 1) &&
		   !(s < 0 || s > 1) &&
		   !(l < 0 || l > 1));

	Rgb color{ 0, 0, 0 };

	if(fabs(s) < 0.000001)
	{
		color.r = l;
		color.g = l;
		color.b = l;
	}
	else
	{
		double v2;
		if(l < 0.5)
			v2 = l * (1 + s);
		else
			v2 = (l + s) - (s * l);

		double v1 = 2*l - v2;

		color.r = hue_to_rgb(v1, v2, h + (1 / 3.0));
		color.g = hue_to_rgb(v1, v2, h);
		color.b = hue_to_rgb(v1, v2, h - (1 / 3.0));
	}

	return color;
}

// TODO: Hsl::operator Hsv() const { }

Hsv::operator Rgb() const
{
	assert(!(h < 0 || h > 1) &&
		   !(s < 0 || s > 1) &&
		   !(v < 0 || v > 1));

	Rgb color{ 0, 0, 0 };

	if(fabs(s) < 0.000001)
	{
		color.r = v;
		color.g = v;
		color.b = v;
	}
	else
	{
		double var_h = h * 6;
		if(fabs(var_h - 6) < 0.000001)
			var_h = 0;

		int var_i = floor(var_h);
		double var_1 = v * (1 - s);
		double var_2 = v * (1 - s * (var_h - var_i));
		double var_3 = v * (1 - s * (1 - (var_h - var_i)));

		if(var_i == 0)
		{
			color.r = v;
			color.g = var_3;
			color.b = var_1;
		}
		else if(var_i == 1)
		{
			color.r = var_2;
			color.g = v;
			color.b = var_1;
		}
		else if(var_i == 2)
		{
			color.r = var_1;
			color.g = v;
			color.b = var_3;
		}
		else if(var_i == 3)
		{
			color.r = var_1;
			color.g = var_2;
			color.b = v;
		}
		else if(var_i == 4)
		{
			color.r = var_3;
			color.g = var_1;
			color.b = v;
		}
		else
		{
			color.r = v;
			color.g = var_1;
			color.b = var_2;
		}
	}

	return color;
}

// TODO: Hsv::operator Hsl() const { }

}