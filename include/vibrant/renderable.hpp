#pragma once
#ifndef VIBRANT_RENDERABLE_HPP

#include "entityx/entityx.h"
#include "boost/variant.hpp"

#include "vibrant/vector.hpp"

namespace vibrant {

struct Line
{
	Line(Vector2d point1, Vector2d point2) : point1(point1), point2(point2) { }

	Vector2d point1;
	Vector2d point2;
};

struct Rectangle
{
	Rectangle(Vector2d point, Vector2d size) : point(point), size(size) { }

	Vector2d point;
	Vector2d size;
};


typedef boost::variant<Line, Rectangle> RenderPrimitive;

struct Renderable : entityx::Component<Renderable>
{
	Renderable(RenderPrimitive primitive) : primitive(primitive) { }

	RenderPrimitive primitive;
};

}

#endif // VIBRANT_RENDERABLE_HPP
