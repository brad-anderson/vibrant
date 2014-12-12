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
			if (ease->current > ease->total_time)
				ease->value_function(component) = ease->beginning + ease->change;
			else
				ease->value_function(component) = ease->easing_function(ease->current, ease->beginning, ease->change, ease->total_time);
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

template<typename Time, typename Value>
Value ease_in_back(Time t, Value b, Value c, Time d)
{
	double s = 1.70158;
	Time postFix = t/=d;
	return c*(postFix)*t*((s+1)*t - s) + b;
}

template<typename Time, typename Value>
float ease_out_back(Time t, Value b, Value c, Time d)
{	
	double s = 1.70158;
	return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

template<typename Time, typename Value>
float ease_inout_back(Time t, Value b, Value c, Time d)
{
	double s = 1.70158;
	if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
	Value postFix = t-=2;
	return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
}

} // namespace vibrant

#endif // VIBRANT_EASE_HPP
