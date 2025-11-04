#pragma once

#include "utils/drawable.h"

#include "utils/maths.h"

class Buffer;

#define DEFAULT_GOAL_POSITION Vector2I(0, 0)

class Goal : public Drawable {
public:
	Goal(Buffer& _buffer, const Vector2I& _position = DEFAULT_GOAL_POSITION);

	void setPosition(const Vector2I& _position);

	void move(const Vector2I& _delta);

	const Vector2I& getPosition() const;

	void draw() const override;

private:
	Vector2I m_position;
};