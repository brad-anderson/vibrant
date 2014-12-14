#pragma once
#ifndef VIBRANT_EASE_HPP

#include "entityx/entityx.h"

namespace vibrant {

template<typename Time, typename Value, typename Comp>
struct Ease : entityx::Component<Ease<Time, Value, Comp>>
{
	typedef std::function<Value(Time, Value, Value, Time)> EasingFunction;
	typedef std::function<Value&(typename Comp::Handle)> ValueFunction;

	Ease(ValueFunction value_function, Value beginning, Value final, Time total_time,
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
class EasingSystem : public entityx::System<EasingSystem<Time, Value, Comp>>
{
public:
	EasingSystem() { }

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override
	{
		Ease<Time, Value, Comp>::Handle ease;
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
				entity.remove<Ease<Time, Value, Comp>>();
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
Value ease_in_bounce(Time t, Value b, Value c, Time d) {
	return c - ease_out_bounc (d-t, 0, c, d) + b;
}

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
Value ease_inout_bounce(Time t, Value b, Value c, Time d) {
	if (t < d/2) return easeIn (t*2, 0, c, d) * 0.5 + b;
	else return ease_out_bounce (t*2-d, 0, c, d) * 0.5 + c*0.5 + b;
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
	return -(postFix * sin((t*d-s)*(2*M_PI)/p )) + b;
}

template<typename Time, typename Value>
Value ease_out_elastic(Time t, Value b, Value c, Time d) {
	if (t==0) return b;  if ((t/=d)==1) return b+c;  
	double p=d*0.3;
	Value a=c; 
	double s=p/4;
	return (a*pow(2,-10*t) * sin( (t*d-s)*(2*M_PI)/p ) + c + b);	
}

template<typename Time, typename Value>
Value ease_inout_elastic(Time t, Value b, Value c, Time d) {
	if (t==0) return b;  if ((t/=d/2)==2) return b+c; 
	double p=d*(0.3*1.5);
	Value a=c; 
	double s=p/4;
	 
	if (t < 1) {
		Value postFix =a*pow(2,10*(t-=1));
		return -0.5*(postFix* sin( (t*d-s)*(2*M_PI)/p )) + b;
	} 
	Value postFix =  a*pow(2,-10*(t-=1));
	return postFix * sin( (t*d-s)*(2*M_PI)/p )*0.5 + c + b;
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
	return -c * cos(t/d * (M_PI/2)) + c + b;
}
template<typename Time, typename Value>
Value ease_out_sine(Time t, Value b, Value c, Time d) {	
	return c * sin(t/d * (M_PI/2)) + b;	
}

template<typename Time, typename Value>
Value ease_inout_sine(Time t, Value b, Value c, Time d) {
	return -c/2 * (cos(M_PI*t/d) - 1) + b;
}


} // namespace vibrant

#endif // VIBRANT_EASE_HPP
