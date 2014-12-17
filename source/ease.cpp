#include "pch.hpp"

#include "vibrant/ease.hpp"

namespace vibrant {

void move_to(entityx::Entity entity, Vector2d new_position, double time, Ease ease, double delay)
{
	if (!entity.has_component<Easings<Body>>())
		entity.assign<Easings<Body>>();

	auto beginning = entity.component<Body>()->position;
	entity.component<Easings<Body>>()->position_easing = {
		true,
		beginning,
		new_position - beginning,
		-delay,
		time,
		ease_to_function<Vector2d>(ease) };
}

void resize_to(entityx::Entity entity, Vector2d new_size, double time, Ease ease, double delay)
{
	if (!entity.has_component<Easings<Body>>())
		entity.assign<Easings<Body>>();

	auto beginning = entity.component<Body>()->size;
	entity.component<Easings<Body>>()->size_easing = {
		true,
		beginning,
		new_size - beginning,
		-delay,
		time,
		ease_to_function<Vector2d>(ease) };
}

void rotate_to(entityx::Entity entity, Radians new_rotation, double time, Ease ease, double delay)
{
	if (!entity.has_component<Easings<Body>>())
		entity.assign<Easings<Body>>();

	auto beginning = entity.component<Body>()->rotation;
	entity.component<Easings<Body>>()->rotation_easing = {
		true,
		beginning,
		new_rotation - beginning,
		-delay,
		time,
		ease_to_function<Radians>(ease) };
}




void stroke_width_to(entityx::Entity entity, double new_width, double time, Ease ease, double delay)
{
	if (!entity.has_component<Easings<Renderable>>())
		entity.assign<Easings<Renderable>>();

	auto beginning = boost::apply_visitor(stroke_width_visitor(), entity.component<Renderable>()->primitive);
	entity.component<Easings<Renderable>>()->stroke_width_easing = {
		true,
		beginning,
		new_width - beginning,
		-delay,
		time,
		ease_to_function<double>(ease) };
}
void stroke_color_to(entityx::Entity entity, Rgb new_color, double time, Ease ease, double delay)
{
	if (!entity.has_component<Easings<Renderable>>())
		entity.assign<Easings<Renderable>>();

	auto beginning = boost::apply_visitor(stroke_color_visitor(), entity.component<Renderable>()->primitive);
	entity.component<Easings<Renderable>>()->stroke_color_easing = {
		true,
		beginning,
		new_color - beginning,
		-delay,
		time,
		ease_to_function<Rgb>(ease) };
}
void fill_color_to(entityx::Entity entity, Rgb new_color, double time, Ease ease, double delay)
{
	if (!entity.has_component<Easings<Renderable>>())
		entity.assign<Easings<Renderable>>();

	auto beginning = boost::apply_visitor(get_fill_color_visitor(), entity.component<Renderable>()->primitive);
	entity.component<Easings<Renderable>>()->fill_color_easing = {
		true,
		beginning,
		new_color - beginning,
		-delay,
		time,
		ease_to_function<Rgb>(ease) };
}

} // namespace vibrant