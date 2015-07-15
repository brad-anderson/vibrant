#pragma once
#ifndef VIBRANT_BODY_HPP

#include "entityx/entityx.h"

#include "vibrant/vector.hpp"

namespace vibrant
{
typedef double Radians;

struct Body : entityx::Component<Body>
{
  Body(Vector2d position, Vector2d size, Radians rotation = 0)
      : position(position), size(size), rotation(rotation)
  {
  }

  Vector2d position;
  Vector2d size;
  Radians rotation;
};
}

#endif  // VIBRANT_BODY_HPP