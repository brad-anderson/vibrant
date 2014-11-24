#include "pch.hpp"

#include "vibrant/cairo/render.hpp"

#include "vibrant/renderable.hpp"

namespace vibrant {

class render_visitor : public boost::static_visitor<>
{
public:
	render_visitor(cairo_t* context) : context(context) { }

    void operator()(Line line) const
    {
		cairo_save(context);

		// temporary
		cairo_set_line_width(context, 2);
		cairo_set_source_rgb(context, 0, 0, 0);

		cairo_move_to(context, line.point1.x, line.point1.y);
		cairo_line_to(context, line.point2.x, line.point2.y);
		cairo_stroke(context);

		cairo_restore(context);
    }
    
    void operator()(Rectangle rect) const
    {
		cairo_save(context);

		// temporary
		cairo_set_line_width(context, 2);
		cairo_set_source_rgb(context, 0, 0, 0);

		cairo_rectangle(context, rect.point.x, rect.point.y, rect.size.x, rect.size.y);
		cairo_stroke(context);

		cairo_restore(context);
    }

private:
	cairo_t* context;
};

void CairoRenderSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)
{
	assert(context);

	Renderable::Handle renderable;

	for (entityx::Entity entity : es.entities_with_components(renderable))
	{
		boost::apply_visitor(render_visitor(context), renderable->primitive);
	}
	/*last_update += dt;
	if (last_update >= 0.5) {
		std::ostringstream out;
		out << es.size() << " entities (" << static_cast<int>(1.0 / dt) << " fps)";
		text.setString(out.str());
		last_update = 0.0;
	}
	target.draw(text);*/
}

}