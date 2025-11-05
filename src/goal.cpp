#include "goal.h"

#include "buffer.h"

#include <algorithm>

Goal::Goal(Buffer& _buffer, const Vector2I& _position) : Drawable(_buffer) {
	setPosition(_position);
}

void Goal::setPosition(const Vector2I& _position) {
	if(m_buffer->getValue(_position) == 'P')
		return;

	m_position = _position;

	Vector2I size = m_buffer->getSize();
	m_position.setX(std::max(0, std::min(size.getX() - 1, m_position.getX())));
	m_position.setY(std::max(0, std::min(size.getY() - 1, m_position.getY())));
}

void Goal::move(const Vector2I& _delta) {
	if (m_buffer->getValue(m_position + _delta) == 'P')
		return;

	m_position += _delta;

	Vector2I size = m_buffer->getSize();
	m_position.setX(std::max(0, std::min(size.getX() - 1, m_position.getX())));
	m_position.setY(std::max(0, std::min(size.getY() - 1, m_position.getY())));
}

const Vector2I& Goal::getPosition() const {
	return m_position;
}

void Goal::draw() const {
	m_buffer->setValue(m_position, 'X');
}