#pragma once

class Buffer;

class Drawable {
public:
	Drawable(Buffer& _buffer);

	virtual void draw() const = 0;

protected:
	Buffer* m_buffer;
};