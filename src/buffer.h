#pragma once

#include "utils/maths.h"

#include <vector> // TEMPORARY
#include <string> // TEMPORARY

#define DEFAULT_BUFFER_WIDTH	16
#define DEFAULT_BUFFER_HEIGHT	16

#define DEFAULT_BUFFER_SIZE	Vector2I(DEFAULT_BUFFER_WIDTH, DEFAULT_BUFFER_HEIGHT)

class Buffer {
public:
	Buffer(const Vector2I& _size = DEFAULT_BUFFER_SIZE);
	~Buffer();

	char getValue(const Vector2I& _position) const;
	void setValue(const Vector2I& _position, char _char);

	const Vector2I& getPosition(int _index) const;
	int getIndex(const Vector2I& _position) const;
	const Vector2I& getSize() const;

	void setScoresContainer(std::vector<int>* _scores) { // TEMPORARY
		m_scores = _scores;
	}

	void clear(char _char = '.');
	void present(bool _flush = false) const;

	void drawTextBellowBuffer(const std::string& _text) const;

private:
	Vector2I m_size;

	std::vector<int>* m_scores; // TEMPORARY

	char* m_data;
	char* m_oldData;
};