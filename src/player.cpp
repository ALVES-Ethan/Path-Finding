#include "player.h"

#include "buffer.h"

#include <algorithm>

Player::Player(Buffer& _buffer, const Vector2I& _position) : Drawable(_buffer) {
	setPosition(_position);
}

void Player::setPosition(const Vector2I& _position) {
	m_position = _position;

	Vector2I size = m_buffer->getSize();
	m_position.setX(std::max(0, std::min(size.getX() - 1, m_position.getX())));
	m_position.setY(std::max(0, std::min(size.getY() - 1, m_position.getY())));
}

void Player::move(const Vector2I& _delta) {
	m_position += _delta;

	Vector2I size = m_buffer->getSize();
	m_position.setX(std::max(0, std::min(size.getX() - 1, m_position.getX())));
	m_position.setY(std::max(0, std::min(size.getY() - 1, m_position.getY())));
}

const Vector2I& Player::getPosition() const {
	return m_position;
}

void Player::draw() const {
	m_buffer->setValue(m_position, 'O');
}