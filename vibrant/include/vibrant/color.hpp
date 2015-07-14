#pragma once
#ifndef VIBRANT_RENDERABLE_HPP

namespace vibrant
{

struct Hsl;
struct Hsv;

struct Rgb
{
	Rgb() : r(0), g(0), b(0), a(1) { }
	Rgb(double r, double g, double b, double a=1.0) : r(r), g(g), b(b), a(a) { }

	operator Hsl() const;
	operator Hsv() const;

	double r, g, b, a;
};

// So easing functions work
Rgb operator+(Rgb lhs, Rgb rhs);
Rgb operator-(Rgb lhs, Rgb rhs);
Rgb operator*(Rgb lhs, Rgb rhs);

Rgb operator*(Rgb lhs, double rhs);
Rgb operator/(Rgb lhs, double rhs);
Rgb operator+(Rgb lhs, double rhs);

Rgb operator*(double lhs, Rgb rhs);
Rgb operator/(double lhs, Rgb rhs);
Rgb operator+(double lhs, Rgb rhs);

Rgb operator-(const Rgb& rhs);

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