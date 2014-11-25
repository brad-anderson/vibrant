#include "pch.hpp"

#include "vibrant/cairo/render.hpp"

#include "vibrant/renderable.hpp"
#include "vibrant/body.hpp"

namespace vibrant {

class render_visitor : public boost::static_visitor<>
{
public:
	render_visitor(cairo_t* context, Body::Handle body) : context(context), body(body) { }

    void operator()(Line line) const
    {
		cairo_save(context);


		cairo_translate(context, body->position.x, body->position.y);
		cairo_rotate(context, body->rotation);
		cairo_move_to(context, 0, 0);

		double line_length = std::max(body->size.x, body->size.y);

		cairo_line_to(context, line_length, 0);

		Rgb color = line.stroke.color;
		cairo_set_source_rgb(context, color.r, color.g, color.b);
		cairo_set_line_width(context, line.stroke.width);
		cairo_stroke(context);

		cairo_restore(context);
    }
    
    void operator()(Rectangle rect) const
    {
		cairo_save(context);

		cairo_translate(context, body->position.x + body->size.x / 2, body->position.y + body->size.y / 2);
		cairo_rotate(context, body->rotation);
		cairo_rectangle(context,
						-body->size.x / 2, -body->size.y / 2,
						body->size.x, body->size.y);

		Rgb fill_color = rect.fill.color;
		cairo_set_source_rgb(context, fill_color.r, fill_color.g, fill_color.b);
		cairo_fill_preserve(context);

		cairo_set_line_width(context, rect.stroke.width);
		Rgb stroke_color = rect.stroke.color;
		cairo_set_source_rgb(context, stroke_color.r, stroke_color.g, stroke_color.b);

		cairo_stroke(context);


		cairo_restore(context);
    }

private:
	cairo_t* context;
	Body::Handle body;
};

void CairoRenderSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)
{
	assert(context);
	cairo_save(context);

	Body::Handle body;
	Renderable::Handle renderable;

	for (entityx::Entity entity : es.entities_with_components(body, renderable))
	{
		boost::apply_visitor(render_visitor(context, body), renderable->primitive);
	}
	/*last_update += dt;
	if (last_update >= 0.5) {
		std::ostringstream out;
		out << es.size() << " entities (" << static_cast<int>(1.0 / dt) << " fps)";
		text.setString(out.str());
		last_update = 0.0;
	}
	target.draw(text);*/

	cairo_restore(context);
}

}