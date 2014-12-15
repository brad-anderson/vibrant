#pragma once
#ifndef VIBRANT_MOUSE_HPP

#include "entityx/entityx.h"

#include "vibrant/vector.hpp"

namespace vibrant {

enum class MouseState
{
	Hover,
	LeftDownOn,
	//LeftUpOn,
	//LeftDrag,
	//LeftDrop,
	MiddleDownOn,
	//MiddleUpOn,
	//MiddleDrag,
	//MiddleDrop,
	RightDownOn,
	//RightUpOn,
	//RightDrag,
	//RightDrop
};

struct Mouseable : entityx::Component<Mouseable>
{
	Mouseable() : state(0) { }

	bool hover() const        { return state[(unsigned)MouseState::Hover]; }
	bool leftDownOn() const   { return state[(unsigned)MouseState::LeftDownOn]; }
	//bool leftUpOn() const     { return state[(unsigned)MouseState::LeftUpOn]; }
	bool middleDownOn() const { return state[(unsigned)MouseState::MiddleDownOn]; }
	//bool middleUpOn() const   { return state[(unsigned)MouseState::MiddleUpOn]; }
	bool rightDownOn() const  { return state[(unsigned)MouseState::RightDownOn]; }
	//bool rightUpOn() const    { return state[(unsigned)MouseState::RightUpOn]; }

	void hover(bool new_state)        { state[(unsigned)MouseState::Hover] = new_state; }
	void leftDownOn(bool new_state)   { state[(unsigned)MouseState::LeftDownOn] = new_state; }
	//void leftUpOn(bool new_state)     { state[(unsigned)MouseState::LeftUpOn] = new_state; }
	void middleDownOn(bool new_state) { state[(unsigned)MouseState::MiddleDownOn] =  new_state; }
	//void middleUpOn(bool new_state)   { state[(unsigned)MouseState::MiddleUpOn] = new_state; }
	void rightDownOn(bool new_state)  { state[(unsigned)MouseState::RightDownOn] = new_state; }
	//void rightUpOn(bool new_state)    { state[(unsigned)MouseState::RightUpOn] = new_state; }
	
	std::bitset<32> state;
};

enum class ButtonState
{
	Pressed,
	Released,
	Down,
	DoubleClicked
};

struct MouseUpdate
{
	Vector2d position;
	ButtonState left;
	ButtonState middle;
	ButtonState right;
};


struct MouseEnter : public entityx::Event<MouseEnter>
{
	MouseEnter(entityx::Entity entity) : entity(entity) { }

	entityx::Entity entity;
};

struct MouseLeave : public entityx::Event<MouseLeave>
{
	MouseLeave(entityx::Entity entity) : entity(entity) { }

	entityx::Entity entity;
};

// A "click" is only emitted if the button went down inside the entity and later went up while
// inside the entity. The mouse can leave and re-enter the entity to handle cases where
// a user wants to abort a click by releasing the button outside of the entity. 
struct LeftClick : public entityx::Event<LeftClick>
{
	LeftClick(entityx::Entity entity) : entity(entity) { }

	entityx::Entity entity;
};

struct MiddleClick : public entityx::Event<MiddleClick>
{
	MiddleClick(entityx::Entity entity) : entity(entity) { }

	entityx::Entity entity;
};

struct RightClick : public entityx::Event<RightClick>
{
	RightClick(entityx::Entity entity) : entity(entity) { }

	entityx::Entity entity;
};


class MouseSystem : public entityx::System < MouseSystem >
{
public:
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override { }
	void update(entityx::EntityManager &es, entityx::EventManager &events, MouseUpdate mouse_update);
};

} // namespace vibrant

#endif // VIBRANT_MOUSE_HPP