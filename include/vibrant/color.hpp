#pragma once
#ifndef VIBRANT_RENDERABLE_HPP

namespace vibrant
{

struct Hsl;
struct Hsv;

struct Rgb
{
	Rgb(double r, double g, double b, double a=1.0) : r(r), g(g), b(b), a(a) { }

	operator Hsl() const;
	operator Hsv() const;

	double r, g, b, a;
};

struct Hsl
{
	Hsl(double h, double s, double l, double a=1.0) : h(h), s(s), l(l), a(a) { }

	operator Rgb() const;
	operator Hsv() const;

	double h, s, l, a;
};

struct Hsv
{
	Hsv(double h, double s, double v, double a=1.0) : h(h), s(s), v(v), a(a) { }

	operator Rgb() const;
	operator Hsl() const;

	double h, s, v, a;
};

}

#endif // VIBRANT_RENDERABLE_HPP