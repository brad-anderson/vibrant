#include "pch.hpp"

#include "vibrant/layout.hpp"
#include "vibrant/body.hpp"

namespace vibrant
{
LayoutSystem::LayoutSystem()
    : left_limit{0},
      right_limit{},
      top_limit{0},
      bottom_limit{},
      right_limit_stay{right_limit == 1280},
      bottom_limit_stay{bottom_limit == 720}
{
  solver.add_constraint(left_limit == 0);
  solver.add_constraint(top_limit == 0);
  solver.add_constraint(right_limit_stay);
  solver.add_constraint(bottom_limit_stay);
}

void LayoutSystem::update(entityx::EntityManager& es, entityx::EventManager& events,
                          entityx::TimeDelta dt)
{
  Layout::Handle layout;
  Body::Handle body;

  for (entityx::Entity entity : es.entities_with_components(layout, body))
  {
    assert(!layout->x.is_nil());
    assert(!layout->width.is_nil());
    assert(!layout->y.is_nil());
    assert(!layout->height.is_nil());

    body->position.x = layout->x.value();
    body->position.y = layout->y.value();
    body->size.x = layout->width.value();
    body->size.y = layout->height.value();
  }
}

void LayoutSystem::setSize(Vector2u size)
{
  solver.remove_constraint(right_limit_stay);
  right_limit_stay = (right_limit == size.x);
  solver.add_constraint(right_limit_stay);

  solver.remove_constraint(bottom_limit_stay);
  bottom_limit_stay = (bottom_limit == size.y);
  solver.add_constraint(bottom_limit_stay);
}

}