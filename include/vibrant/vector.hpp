#pragma once
#ifndef VIBRANT_VECTOR_HPP

template<typename Ty>
struct Vector2
{
	Vector2(Ty arg_x, Ty arg_y) : _x(arg_x), _y(arg_y) { }

	Ty x;
	Ty y;
};


typedef Vector2<double>			Vector2d;
typedef Vector2<float>			Vector2f;
typedef Vector2<int>			Vector2i;
typedef Vector2<unsigned int>	Vector2u;

#endif // VIBRANT_VECTOR_HPP