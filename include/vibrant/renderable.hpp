#pragma once
#ifndef VIBRANT_RENDERABLE_HPP

#include "entityx/entityx.h"
#include "boost/variant.hpp"

#include "vibrant/color.hpp"

namespace vibrant {

struct Stroke
{
	double width;
	Rgb color;
	// TODO: Style
};

struct Fill
{
	Rgb color;
	// TODO: Gradients
};


struct Line
{
	// rotates around position of Body

	Line(Stroke stroke) : stroke(stroke) { }

	Stroke stroke;
};

struct Rectangle
{
	// rotates around center of Body

	Rectangle(Stroke stroke, Fill fill) : stroke(stroke), fill(fill) { }

	Stroke stroke;
	Fill fill;
};


typedef boost::variant<Line, Rectangle> RenderPrimitive;

struct Renderable : entityx::Component<Renderable>
{
	Renderable(RenderPrimitive primitive) : primitive(primitive) { }

	RenderPrimitive primitive;
};

}

#endif // VIBRANT_RENDERABLE_HPP
