#pragma once

#include "utils/maths.h"

#define DEFAULT_BUFFER_WIDTH	16
#define DEFAULT_BUFFER_HEIGHT	16

#define DEFAULT_BUFFER_SIZE	Vector2I(DEFAULT_BUFFER_WIDTH, DEFAULT_BUFFER_HEIGHT)

class Buffer {
public:
	Buffer(const Vector2I& _size = DEFAULT_BUFFER_SIZE);
	~Buffer();

	char getValue(const Vector2I& _position) const;
	void setValue(const Vector2I& _position, char _char);

	int getIndex(const Vector2I& _position) const;
	const Vector2I& getSize() const;

	void clear(char _char = '.');
	void present() const;

private:
	Vector2I m_size;
	char* m_data;
	char* m_oldData;
};