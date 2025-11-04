#include "path.h"

#include "buffer.h"

Path::Path(Buffer& _buffer) : Drawable(_buffer) {
}

void Path::addPoint(const Vector2I& _point) {
	m_data.push_back(_point);
}

void Path::draw() const {
	for (const Vector2I& position : m_data) {
		if (m_buffer->getValue(position) == '.') {
			m_buffer->setValue(position, 'Z');
		}
	}
}