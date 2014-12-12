#pragma once
#ifndef VIBRANT_VECTOR_HPP

namespace vibrant {

template <class Ty>
struct Vector2;

template<typename Ty>
Vector2<Ty> operator+(const Vector2<Ty>& lhs, const Vector2<Ty>& rhs)
{
	return Vector2<Ty>(lhs.x+rhs.x, lhs.y+rhs.y);
}

template<typename Ty>
Vector2<Ty> operator-(const Vector2<Ty>& lhs, const Vector2<Ty>& rhs)
{
	return Vector2<Ty>(lhs.x-rhs.x, lhs.y-rhs.y);
}


template<typename Ty>
Vector2<Ty> operator*(const Vector2<Ty>& lhs, double rhs)
{
	return Vector2<Ty>(lhs.x*rhs, lhs.y*rhs);
}

template<typename Ty>
Vector2<Ty> operator+(const Vector2<Ty>& lhs, double rhs)
{
	return Vector2<Ty>(lhs.x+rhs, lhs.y+rhs);
}

template<typename Ty>
struct Vector2
{
	Vector2() : x(0), y(0) { }
	Vector2(Ty arg_x, Ty arg_y) : x(arg_x), y(arg_y) { }

	Ty x;
	Ty y;
};


typedef Vector2<double>			Vector2d;
typedef Vector2<float>			Vector2f;
typedef Vector2<int>			Vector2i;
typedef Vector2<unsigned int>	Vector2u;

} // namespace vibrant

#endif // VIBRANT_VECTOR_HPP