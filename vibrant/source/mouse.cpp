#include "pch.hpp"

#include "vibrant/mouse.hpp"
#include "vibrant/body.hpp"

namespace vibrant
{
using namespace entityx;

void MouseSystem::update(EntityManager &es, EventManager &events, MouseUpdate mouse)
{
  Body::Handle body;
  Mouseable::Handle mouseable;

  for (entityx::Entity entity : es.entities_with_components(body, mouseable))
  {
    Mouseable previous_state = *mouseable.get();

    bool hover = mouse.position.intersects(body->position, body->size, body->rotation);

    mouseable->hover(hover);

    bool left_up_on = false;
    switch (mouse.left)
    {
      case ButtonState::Pressed:
        mouseable->leftDownOn(hover);
        break;
      case ButtonState::Released:
        mouseable->leftDownOn(false);
        left_up_on = hover;
        break;
        /*
        case ButtonState::Down:
                break;
        case ButtonState::DoubleClicked:
                break;
        */
    }

    // emit events for changes in state
    if (mouseable->hover() && !previous_state.hover()) events.emit<MouseEnter>(entity);
    if (!mouseable->hover() && previous_state.hover()) events.emit<MouseLeave>(entity);
    if (previous_state.leftDownOn() && left_up_on) events.emit<LeftClick>(entity);
  }
}

}  // namespace vibrant
