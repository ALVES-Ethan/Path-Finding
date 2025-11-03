#pragma once

#include "utils/maths.h"

class Buffer;

#define DEFAULT_PLAYER_POSITION	Vector2I(0, 0)

class Player {
public:
	Player(Buffer* _buffer, const Vector2I& _position = DEFAULT_PLAYER_POSITION);

	void setPosition(const Vector2I& _position);

	void move(const Vector2I& _delta);

	const Vector2I& getPosition() const;

private:
	Vector2I m_position;
	Buffer* m_buffer;
};