#pragma once
#ifndef VIBRANT_CAIRO_RENDER_HPP

#include "entityx/entityx.h"
#include "cairo/cairo.h"

namespace vibrant {

class CairoRenderSystem : public entityx::System < CairoRenderSystem >
{
public:
	CairoRenderSystem() { }

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;

	void setContext(cairo_t* arg_context) { context = arg_context; }

private:
	cairo_t* context = nullptr;
	float last_update = 0.0;
};

}

#endif // VIBRANT_CAIRO_RENDER_HPP