#pragma once
#ifndef VIBRANT_LAYOUT_HPP

#include "vibrant/vector.hpp"
#include "entityx/entityx.h"
#include "rhea/simplex_solver.hpp"

namespace vibrant
{

struct Layout : entityx::Component<Layout>
{
  Layout(rhea::variable x, rhea::variable y, rhea::variable width, rhea::variable height)
      : x(x), y(y), width(width), height(height)
  {
  }

  rhea::variable x;
  rhea::variable y;
  rhea::variable width;
  rhea::variable height;

};

class LayoutSystem : public entityx::System<LayoutSystem>
{
 public:
  LayoutSystem();

  void update(entityx::EntityManager& es, entityx::EventManager& events,
              entityx::TimeDelta dt) override;

  void setSize(Vector2u size);

  rhea::simplex_solver solver;
  rhea::variable left_limit;
  rhea::variable right_limit;
  rhea::variable top_limit;
  rhea::variable bottom_limit;
  rhea::constraint right_limit_stay;
  rhea::constraint bottom_limit_stay;

};

}  // namespace vibrant

#endif  // VIBRANT_LAYOUT_HPP