#include "path.h"

#include "buffer.h"

Path::Path(Buffer& _buffer) : Drawable(_buffer) {
}

void Path::addPoint(const Vector2I& _point) {
	m_data.push_back(_point);
}
void Path::removePoint(const Vector2I& _point) {
	m_data.erase(std::remove(m_data.begin(), m_data.end(), _point), m_data.end());
}
void Path::removeLastPoint() {
	m_data.pop_back();
}

void Path::clearPoints() {
	m_data.clear();
}

void Path::draw() const {
	for (const Vector2I& position : m_data) {
		if (m_buffer->getValue(position) == '.') {
			m_buffer->setValue(position, 'Z');
		}
	}
}