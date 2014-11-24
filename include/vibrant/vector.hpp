#pragma once
#ifndef VIBRANT_VECTOR_HPP

namespace vibrant {

template<typename Ty>
struct Vector2
{
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