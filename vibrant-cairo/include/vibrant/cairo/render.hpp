#pragma once
#ifndef VIBRANT_CAIRO_RENDER_HPP

#include "entityx/entityx.h"
#include "cairo/cairo.h"

#include "vibrant/renderable.hpp"
#include "vibrant/body.hpp"

namespace vibrant
{
class CairoRenderSystem : public entityx::System<CairoRenderSystem>
{
 public:
  CairoRenderSystem() {}

  void update(entityx::EntityManager& es, entityx::EventManager& events,
              entityx::TimeDelta dt) override;

  void setContext(cairo_t* arg_context) { context = arg_context; }

 private:
  cairo_t* context = nullptr;
  typedef std::tuple<Renderable::Handle, Body::Handle> EntityPack;
  std::vector<EntityPack> m_orderedEntities;
};
}

#endif  // VIBRANT_CAIRO_RENDER_HPP