#pragma once
#ifndef VIBRANT_VECTOR_HPP

#ifndef M_TAU
#define M_TAU 6.28318530717958647693
#endif

namespace vibrant
{
template <class Ty>
struct Vector2;

template <typename Ty>
Vector2<Ty> operator+(Vector2<Ty> lhs, Vector2<Ty> rhs)
{
  return Vector2<Ty>(lhs.x + rhs.x, lhs.y + rhs.y);
}
template <typename Ty>
Vector2<Ty> operator-(Vector2<Ty> lhs, Vector2<Ty> rhs)
{
  return Vector2<Ty>(lhs.x - rhs.x, lhs.y - rhs.y);
}
template <typename Ty>
Vector2<Ty> operator*(Vector2<Ty> lhs, Vector2<Ty> rhs)
{
  return Vector2<Ty>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template <typename Ty>
Vector2<Ty> operator*(Vector2<Ty> lhs, double rhs)
{
  return Vector2<Ty>(lhs.x * rhs, lhs.y * rhs);
}
template <typename Ty>
Vector2<Ty> operator/(Vector2<Ty> lhs, double rhs)
{
  return Vector2<Ty>(lhs.x / rhs, lhs.y / rhs);
}
template <typename Ty>
Vector2<Ty> operator+(Vector2<Ty> lhs, double rhs)
{
  return Vector2<Ty>(lhs.x + rhs, lhs.y + rhs);
}

template <typename Ty>
Vector2<Ty> operator*(double lhs, Vector2<Ty> rhs)
{
  return Vector2<Ty>(lhs * rhs.x, lhs * rhs.y);
}
template <typename Ty>
Vector2<Ty> operator/(double lhs, Vector2<Ty> rhs)
{
  return Vector2<Ty>(lhs / rhs.x, lhs / rhs.y);
}
template <typename Ty>
Vector2<Ty> operator+(double lhs, Vector2<Ty> rhs)
{
  return Vector2<Ty>(lhs + rhs.x, lhs + rhs.y);
}

template <typename Ty>
struct Vector2
{
  Vector2() : x(0), y(0) {}
  Vector2(Ty arg_x, Ty arg_y) : x(arg_x), y(arg_y) {}

  template <typename PosV, typename SizeV, typename Rotation>
  bool intersects(PosV position, SizeV size, Rotation rotation)
  {
    auto x1 = cos(rotation) * -size.x / 2.0 + position.x,
         x2 = cos(rotation) * size.x / 2.0 + position.x,
         y1 = sin(rotation) * -size.y / 2.0 + position.y,
         y2 = sin(rotation) * size.y / 2.0 + position.y;

    auto a = PosV(x1, y1), b = PosV(x2, y1), c = PosV(x1, y2), d = PosV(x2, y2);

    auto triangle_area = [](PosV a, PosV b, PosV c)
    {
      return fabs((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2.0);
    };

    auto total_area = triangle_area(a, *this, d) + triangle_area(d, *this, c) +
                      triangle_area(c, *this, b) + triangle_area(*this, b, a);

    return total_area <= size.x * size.y;
  }

  Vector2<Ty> operator-() const { return Vector2<Ty>(-x, -y); }

  Ty x;
  Ty y;
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;

}  // namespace vibrant

#endif  // VIBRANT_VECTOR_HPP