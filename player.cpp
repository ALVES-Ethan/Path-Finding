#include "player.h"

#include "buffer.h"

#include <algorithm>

Player::Player(Buffer* _buffer, const Vector2I& _position) {
	m_buffer = _buffer;
	setPosition(_position);
}

void Player::setPosition(const Vector2I& _position) {
	// replace the old background color at the old player position
	m_buffer->setValue(m_position, '.');

	// set the new player position
	m_position = _position;

	// clamp the player position to the buffer boundaries
	Vector2I size = m_buffer->getSize();
	m_position.setX(std::max(0, std::min(size.getX() - 1, m_position.getX())));
	m_position.setY(std::max(0, std::min(size.getY() - 1, m_position.getY())));

	// set the new player position in the buffer
	m_buffer->setValue(m_position, 'O');
}

void Player::move(const Vector2I& _delta) {
	// replace the old background color at the old player position
	m_buffer->setValue(m_position, '.');

	// move the player
	m_position += _delta;

	// clamp the player position to the buffer boundaries
	Vector2I size = m_buffer->getSize();
	m_position.setX(std::max(0, std::min(size.getX() - 1, m_position.getX())));
	m_position.setY(std::max(0, std::min(size.getY() - 1, m_position.getY())));

	// set the new player position in the buffer
	m_buffer->setValue(m_position, 'O');
}

const Vector2I& Player::getPosition() const {
	return m_position;
}