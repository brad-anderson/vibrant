#pragma once
#ifndef VIBRANT_RENDERABLE_HPP

namespace vibrant
{

struct Hsl;
struct Hsv;

struct Rgb
{
	Rgb(double r, double g, double b) : r(r), g(g), b(b) { }

	operator Hsl() const;
	operator Hsv() const;

	double r, g, b;
};

struct Hsl
{
	Hsl(double h, double s, double l) : h(h), s(s), l(l) { }

	operator Rgb() const;
	operator Hsv() const;

	double h, s, l;
};

struct Hsv
{
	Hsv(double h, double s, double v) : h(h), s(s), v(v) { }

	operator Rgb() const;
	operator Hsl() const;

	double h, s, v;
};

}

#endif // VIBRANT_RENDERABLE_HPP