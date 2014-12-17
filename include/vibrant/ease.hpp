#pragma once
#ifndef VIBRANT_EASE_HPP

#include "entityx/entityx.h"

#include "vibrant/vector.hpp"
#include "vibrant/body.hpp"
#include "vibrant/color.hpp"
#include "vibrant/renderable.hpp"

namespace vibrant {

template<typename Time, typename Value, typename Comp>
struct FastEase : entityx::Component<FastEase<Time, Value, Comp>>
{
	typedef std::function<Value(Time, Value, Value, Time)> EasingFunction;
	typedef std::function<Value&(typename Comp::Handle)> ValueFunction;

	FastEase(ValueFunction value_function, Value beginning, Value final, Time total_time,
		 EasingFunction easing_function)
		: value_function(value_function), beginning(beginning), change(final-beginning), current(0), total_time(total_time),
		  easing_function(easing_function) { }

	ValueFunction value_function;
	Value beginning;
	Value change;
	Time current;
	Time total_time;
	EasingFunction easing_function;
};

template<typename Time, typename Value, typename Comp>
class FastEasingSystem : public entityx::System<FastEasingSystem<Time, Value, Comp>>
{
public:
	FastEasingSystem() { }

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override
	{
		FastEase<Time, Value, Comp>::Handle ease;
		typename Comp::Handle component;

		for (entityx::Entity entity : es.entities_with_components(ease, component))
		{
			ease->current += dt;
			if (ease->current < ease->total_time)
			{
				ease->value_function(component) = ease->easing_function(ease->current, ease->beginning, ease->change, ease->total_time);
			}
			else
			{
				ease->value_function(component) = ease->beginning + ease->change;
				entity.remove<FastEase<Time, Value, Comp>>();
			}
		}
	}
};



// Penner's Easing Equations
// -------------------------
//
// Arguments:
// - t: Current time. Any units (millseconds, frames, etc). Units must just match those of d.
// - b: Beginning value
// - c: Change in value
// - d: Total time
//
// Bootstrapped from jesusgollonet/ofpennereasing.
//
// Use http://easings.net/ for reference.
//
// Available:
// - Back
// - Bounce
// - Circ
// - Cubic
// - Elastic
// - Expo
// - Linear
// - Quad
// - Quart
// - Quint
// - Sine

// Back
// ----

template<typename Time, typename Value>
Value ease_in_back(Time t, Value b, Value c, Time d)
{
	double s = 1.70158;
	Time postFix = t/=d;
	return c*(postFix)*t*((s+1)*t - s) + b;
}

template<typename Time, typename Value>
Value ease_out_back(Time t, Value b, Value c, Time d)
{	
	double s = 1.70158;
	return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

template<typename Time, typename Value>
Value ease_inout_back(Time t, Value b, Value c, Time d)
{
	double s = 1.70158;
	if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
	double postFix = t-=2;
	return c/2*((postFix)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
}


// Bounce
// ------

template<typename Time, typename Value>
Value ease_out_bounce(Time t, Value b, Value c, Time d) {
	if ((t/=d) < (1/2.75)) {
		return c*(7.5625*t*t) + b;
	} else if (t < (2/2.75)) {
		double postFix = t-=(1.5/2.75);
		return c*(7.5625*(postFix)*t + 0.75) + b;
	} else if (t < (2.5/2.75)) {
			double postFix = t-=(2.25/2.75);
		return c*(7.5625*(postFix)*t + 0.9375) + b;
	} else {
		double postFix = t-=(2.625/2.75);
		return c*(7.5625*(postFix)*t + 0.984375) + b;
	}
}

template<typename Time, typename Value>
Value ease_in_bounce(Time t, Value b, Value c, Time d) {
	return c - ease_out_bounce<Time, Value>(d-t, Value(), c, d) + b;
}


template<typename Time, typename Value>
Value ease_inout_bounce(Time t, Value b, Value c, Time d) {
	if (t < d/2) return ease_in_bounce<Time, Value>(t*2, Value(), c, d) * 0.5 + b;
	else return ease_out_bounce<Time, Value>(t*2-d, Value(), c, d) * 0.5 + c*0.5 + b;
}


// Circ
// ----

template<typename Time, typename Value>
Value ease_in_circ(Time t, Value b, Value c, Time d) {
	return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
}
template<typename Time, typename Value>
Value ease_out_circ(Time t, Value b, Value c, Time d) {
	return c * sqrt(1 - (t=t/d-1)*t) + b;
}

template<typename Time, typename Value>
Value ease_inout_circ(Time t, Value b, Value c, Time d) {
	if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
	return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
}


// Cubic
// ------

template<typename Time, typename Value>
Value ease_in_cubic(Time t, Value b, Value c, Time d) {
	return c*(t/=d)*t*t + b;
}
template<typename Time, typename Value>
Value ease_out_cubic(Time t, Value b, Value c, Time d) {
	return c*((t=t/d-1)*t*t + 1) + b;
}

template<typename Time, typename Value>
Value ease_inout_cubic(Time t, Value b, Value c, Time d) {
	if ((t/=d/2) < 1) return c/2*t*t*t + b;
	return c/2*((t-=2)*t*t + 2) + b;	
}

// Elastic
// ------

template<typename Time, typename Value>
Value ease_in_elastic(Time t, Value b, Value c, Time d) {
	if (t==0) return b;  if ((t/=d)==1) return b+c;  
	double p=d*0.3;
	Value a=c; 
	double s=p/4;
	Value postFix =a*pow(2,10*(t-=1));
	return -(postFix * sin((t*d-s)*M_TAU/p )) + b;
}

template<typename Time, typename Value>
Value ease_out_elastic(Time t, Value b, Value c, Time d) {
	if (t==0) return b;  if ((t/=d)==1) return b+c;  
	double p=d*0.3;
	Value a=c; 
	double s=p/4;
	return (a*pow(2,-10*t) * sin( (t*d-s)*M_TAU/p ) + c + b);	
}

template<typename Time, typename Value>
Value ease_inout_elastic(Time t, Value b, Value c, Time d) {
	if (t==0) return b;  if ((t/=d/2)==2) return b+c; 
	double p=d*(0.3*1.5);
	Value a=c; 
	double s=p/4;
	 
	if (t < 1) {
		Value postFix =a*pow(2,10*(t-=1));
		return -0.5*(postFix* sin( (t*d-s)*M_TAU/p )) + b;
	} 
	Value postFix =  a*pow(2,-10*(t-=1));
	return postFix * sin( (t*d-s)*M_TAU/p )*0.5 + c + b;
}


// Expo
// ----

template<typename Time, typename Value>
Value ease_in_expo(Time t, Value b, Value c, Time d) {
	return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
}
template<typename Time, typename Value>
Value ease_out_expo(Time t, Value b, Value c, Time d) {
	return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;	
}

template<typename Time, typename Value>
Value ease_inout_expo(Time t, Value b, Value c, Time d) {
	if (t==0) return b;
	if (t==d) return b+c;
	if ((t/=d/2) < 1) return c/2 * pow(2, 10 * (t - 1)) + b;
	return c/2 * (-pow(2, -10 * --t) + 2) + b;
}

// Linear
// ------

template<typename Time, typename Value>
Value ease_in_linear(Time t, Value b, Value c, Time d) {
	return c*t/d + b;
}
template<typename Time, typename Value>
Value ease_out_linear(Time t, Value b, Value c, Time d) {	
	return c*t/d + b;
}

template<typename Time, typename Value>
Value ease_inout_linear(Time t, Value b, Value c, Time d) {
	return c*t/d + b;
}


// Quad
// ----

template<typename Time, typename Value>
Value ease_in_quad(Time t, Value b, Value c, Time d) {
	return c*(t/=d)*t + b;
}
template<typename Time, typename Value>
Value ease_out_quad(Time t, Value b, Value c, Time d) {
	return -c *(t/=d)*(t-2) + b;
}

template<typename Time, typename Value>
Value ease_inout_quad(Time t, Value b, Value c, Time d) {
	if ((t/=d/2) < 1) return ((c/2)*(t*t)) + b;
	return -c/2 * (((t-2)*(--t)) - 1) + b;
}


// Quart
// ------

template<typename Time, typename Value>
Value ease_in_quart(Time t, Value b, Value c, Time d) {
	return c*(t/=d)*t*t*t + b;
}
template<typename Time, typename Value>
Value ease_out_quart(Time t, Value b, Value c, Time d) {
	return -c * ((t=t/d-1)*t*t*t - 1) + b;
}

template<typename Time, typename Value>
Value ease_inout_quart(Time t, Value b, Value c, Time d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
	return -c/2 * ((t-=2)*t*t*t - 2) + b;
}


// Quint
// ------

template<typename Time, typename Value>
Value ease_in_quint(Time t, Value b, Value c, Time d) {
	return c*(t/=d)*t*t*t*t + b;
}
template<typename Time, typename Value>
Value ease_out_quint(Time t, Value b, Value c, Time d) {
	return c*((t=t/d-1)*t*t*t*t + 1) + b;
}

template<typename Time, typename Value>
Value ease_inout_quint(Time t, Value b, Value c, Time d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
	return c/2*((t-=2)*t*t*t*t + 2) + b;
}

// Sine
// ----

template<typename Time, typename Value>
Value ease_in_sine(Time t, Value b, Value c, Time d) {
	return -c * cos(t/d * (M_TAU/4)) + c + b;
}
template<typename Time, typename Value>
Value ease_out_sine(Time t, Value b, Value c, Time d) {	
	return c * sin(t/d * (M_TAU/4)) + b;	
}

template<typename Time, typename Value>
Value ease_inout_sine(Time t, Value b, Value c, Time d) {
	return -c/2 * (cos(M_TAU/2*t/d) - 1) + b;
}



enum class Ease
{
	InBack,
	OutBack,
	InOutBack,
	InBounce,
	OutBounce,
	InOutBounce,
	InCirc,
	OutCirc,
	InOutCirc,
	InCubic,
	OutCubic,
	InOutCubic,
	InElastic,
	OutElastic,
	InOutElastic,
	InExpo,
	OutExpo,
	InOutExpo,
	InLinear,
	OutLinear,
	InOutLinear,
	InQuad,
	OutQuad,
	InOutQuad,
	InQuart,
	OutQuart,
	InOutQuart,
	InQuint,
	OutQuint,
	InOutQuint,
	InSine,
	OutSine,
	InOutSine
};

template<typename Value>
std::function<Value(double, Value, Value, double)> ease_to_function(Ease ease)
{
	switch (ease)
	{
	case Ease::InBack: return &ease_in_back<double, Value>;
	case Ease::OutBack: return &ease_out_back<double, Value>;
	case Ease::InOutBack: return &ease_inout_back<double, Value>;
	case Ease::InBounce: return &ease_in_bounce<double, Value>;
	case Ease::OutBounce: return &ease_out_bounce<double, Value>;
	case Ease::InOutBounce: return &ease_inout_bounce<double, Value>;
	case Ease::InCirc: return &ease_in_circ<double, Value>;
	case Ease::OutCirc: return &ease_out_circ<double, Value>;
	case Ease::InOutCirc: return &ease_inout_circ<double, Value>;
	case Ease::InCubic: return &ease_in_cubic<double, Value>;
	case Ease::OutCubic: return &ease_out_cubic<double, Value>;
	case Ease::InOutCubic: return &ease_inout_cubic<double, Value>;
	case Ease::InElastic: return &ease_in_elastic<double, Value>;
	case Ease::OutElastic: return &ease_out_elastic<double, Value>;
	case Ease::InOutElastic: return &ease_inout_elastic<double, Value>;
	case Ease::InExpo: return &ease_in_expo<double, Value>;
	case Ease::OutExpo: return &ease_out_expo<double, Value>;
	case Ease::InOutExpo: return &ease_inout_expo<double, Value>;
	case Ease::InLinear: return &ease_in_linear<double, Value>;
	case Ease::OutLinear: return &ease_out_linear<double, Value>;
	case Ease::InOutLinear: return &ease_inout_linear<double, Value>;
	case Ease::InQuad: return &ease_in_quad<double, Value>;
	case Ease::OutQuad: return &ease_out_quad<double, Value>;
	case Ease::InOutQuad: return &ease_inout_quad<double, Value>;
	case Ease::InQuart: return &ease_in_quart<double, Value>;
	case Ease::OutQuart: return &ease_out_quart<double, Value>;
	case Ease::InOutQuart: return &ease_inout_quart<double, Value>;
	case Ease::InQuint: return &ease_in_quint<double, Value>;
	case Ease::OutQuint: return &ease_out_quint<double, Value>;
	case Ease::InOutQuint: return &ease_inout_quint<double, Value>;
	case Ease::InSine: return &ease_in_sine<double, Value>;
	case Ease::OutSine: return &ease_out_sine<double, Value>;
	case Ease::InOutSine: return &ease_inout_sine<double, Value>;
	default: assert(false); return &ease_inout_sine<double, Value>;
	}
}

void move_to(entityx::Entity entity, Vector2d new_position, double time, Ease ease, double delay=0);
void resize_to(entityx::Entity entity, Vector2d new_size, double time, Ease ease, double delay=0);
void rotate_to(entityx::Entity entity, Radians new_rotation, double time, Ease ease, double delay=0);

void stroke_width_to(entityx::Entity entity, double new_width, double time, Ease ease, double delay=0);
void stroke_color_to(entityx::Entity entity, Rgb new_color, double time, Ease ease, double delay=0);
void fill_color_to(entityx::Entity entity, Rgb new_color, double time, Ease ease, double delay=0);
// would be neat but very sophisticated
// void reshape(entityx::Entity entity, RenderPrimitive new_primitive, double time, Ease ease) 


template<typename Value>
struct Easing
{
	Easing() : active(false), beginning(), change(), current(0), total_time(0) { }
	Easing(bool active, Value beginning, Value change, double current, double total_time,
		std::function<Value(double, Value, Value, double)> easing_function)
		: active(active), beginning(beginning), change(change), current(current),
		  total_time(total_time), easing_function(easing_function) { }

	bool active = false;
	Value beginning;
	Value change;
	double current;
	double total_time;
	std::function<Value(double, Value, Value, double)> easing_function;
};

template<typename TargetComponent> struct Easings { };

template<>
struct Easings<Body> : entityx::Component<Easings<Body>>
{
	bool apply(Body::Handle body, double delta)
	{
		// Position
		if (position_easing.active)
		{
			position_easing.current += delta;
			if (position_easing.current < 0) {} // in delay
			else if (position_easing.current < position_easing.total_time)
			{
				body->position =
					position_easing.easing_function(position_easing.current,
													position_easing.beginning,
													position_easing.change,
													position_easing.total_time);
			}
			else
			{
				body->position = position_easing.beginning + position_easing.change;
				position_easing.active = false;
			}
		}

		// Size
		if (size_easing.active)
		{
			size_easing.current += delta;
			if (size_easing.current < 0) {} // in delay
			else if (size_easing.current < size_easing.total_time)
			{
				body->size =
					size_easing.easing_function(size_easing.current,
												size_easing.beginning,
												size_easing.change,
												size_easing.total_time);
			}
			else
			{
				body->size = size_easing.beginning + size_easing.change;
				size_easing.active = false;
			}
		}

		// Radians
		if (rotation_easing.active)
		{
			rotation_easing.current += delta;
			if (rotation_easing.current < 0) {} // in delay
			else if (rotation_easing.current < rotation_easing.total_time)
			{
				body->rotation =
					rotation_easing.easing_function(rotation_easing.current,
													rotation_easing.beginning,
													rotation_easing.change,
													rotation_easing.total_time);
			}
			else
			{
				body->rotation = rotation_easing.beginning + rotation_easing.change;
				rotation_easing.active = false;
			}
		}

		return position_easing.active || size_easing.active || rotation_easing.active;
	}

	Easing<Vector2d> position_easing;
	Easing<Vector2d> size_easing;
	Easing<Radians> rotation_easing;
};


class stroke_width_visitor : public boost::static_visitor<double&>
{
public:
    double& operator()(Line& line) const { return line.stroke.width; }
	double& operator()(Rectangle& rect) const { return rect.stroke.width; }
};

class stroke_color_visitor : public boost::static_visitor<Rgb&>
{
public:
    Rgb& operator()(Line& line) const { return line.stroke.color; }
	Rgb& operator()(Rectangle& rect) const { return rect.stroke.color; }
};

class set_fill_color_visitor : public boost::static_visitor<>
{
public:
	set_fill_color_visitor(Rgb new_color) : new_color(new_color) { }
    void operator()(Line& line) const { }
	void operator()(Rectangle& rect) const { rect.fill.color = new_color; }

	Rgb new_color;
};

class get_fill_color_visitor : public boost::static_visitor<Rgb>
{
public:
	Rgb operator()(Line& line) const { return Rgb(0, 0, 0); }
	Rgb operator()(Rectangle& rect) const { return rect.fill.color; }
};


template<>
struct Easings<Renderable> : entityx::Component<Easings<Renderable>>
{
	bool apply(Renderable::Handle renderable, double delta)
	{
		// Stroke Width
		if (stroke_width_easing.active)
		{
			stroke_width_easing.current += delta;
			if (stroke_width_easing.current < 0) {} // in delay
			else if (stroke_width_easing.current < stroke_width_easing.total_time)
			{
				boost::apply_visitor(stroke_width_visitor(), renderable->primitive) =
					stroke_width_easing.easing_function(stroke_width_easing.current,
													stroke_width_easing.beginning,
													stroke_width_easing.change,
													stroke_width_easing.total_time);
			}
			else
			{
				boost::apply_visitor(stroke_width_visitor(), renderable->primitive) =
					stroke_width_easing.beginning + stroke_width_easing.change;
				stroke_width_easing.active = false;
			}
		}

		// Stroke Color
		if (stroke_color_easing.active)
		{
			stroke_color_easing.current += delta;
			if (stroke_color_easing.current < 0) {} // in delay
			else if (stroke_color_easing.current < stroke_color_easing.total_time)
			{
				boost::apply_visitor(stroke_color_visitor(), renderable->primitive) =
					stroke_color_easing.easing_function(stroke_color_easing.current,
												stroke_color_easing.beginning,
												stroke_color_easing.change,
												stroke_color_easing.total_time);
			}
			else
			{
				boost::apply_visitor(stroke_color_visitor(), renderable->primitive) =
					stroke_color_easing.beginning + stroke_color_easing.change;
				stroke_color_easing.active = false;
			}
		}

		// Fill Color
		if (fill_color_easing.active)
		{
			fill_color_easing.current += delta;
			if (fill_color_easing.current < 0) {} // in delay
			else if (fill_color_easing.current < fill_color_easing.total_time)
			{
				auto new_color =
					fill_color_easing.easing_function(fill_color_easing.current,
													fill_color_easing.beginning,
													fill_color_easing.change,
													fill_color_easing.total_time);
				boost::apply_visitor(set_fill_color_visitor(new_color), renderable->primitive);
			}
			else
			{
				boost::apply_visitor(set_fill_color_visitor(fill_color_easing.beginning + fill_color_easing.change),
					renderable->primitive);
				fill_color_easing.active = false;
			}
		}

		return stroke_width_easing.active || stroke_color_easing.active || fill_color_easing.active;
	}

	Easing<double> stroke_width_easing;
	Easing<Rgb> stroke_color_easing;
	Easing<Rgb> fill_color_easing;
};


template<typename TargetComponent>
class EasingSystem : public entityx::System<EasingSystem<TargetComponent>>
{
public:
	EasingSystem() { }

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override
	{
		Easings<TargetComponent>::Handle easing;
		typename TargetComponent::Handle target;

		for (entityx::Entity entity : es.entities_with_components(easing, target))
		{
			// TODO: add events
			bool alive = easing->apply(target, dt);
			if (!alive)
				entity.remove<Easings<TargetComponent>>();
		}
	}
};


} // namespace vibrant

#endif // VIBRANT_EASE_HPP
