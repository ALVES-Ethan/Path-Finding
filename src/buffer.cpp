#include "buffer.h"

#include "utils/colors.h"

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

Buffer::Buffer(const Vector2I& _size) : m_size(_size) {
	const int total = m_size.getX() * m_size.getY();
	m_data = new char[total];
	m_oldData = new char[total];

	std::fill_n(m_data, total, '.');
	std::fill_n(m_oldData, total, '\0');
}

Buffer::~Buffer() {
	delete[] m_data;
	delete[] m_oldData;
}

char Buffer::getValue(const Vector2I& _position) const {
	if (_position.getX() < 0 || _position.getX() >= m_size.getX() || _position.getY() < 0 || _position.getY() >= m_size.getY()) {
		return '\0';
	}
	return m_data[_position.getY() * m_size.getX() + _position.getX()];
}

void Buffer::setValue(const Vector2I& _position, char _char) {
	if (_position.getX() < 0 || _position.getX() >= m_size.getX() || _position.getY() < 0 || _position.getY() >= m_size.getY()) {
		return;
	}
	m_data[_position.getY() * m_size.getX() + _position.getX()] = _char;
}

const Vector2I& Buffer::getPosition(int _index) const {
	static Vector2I position;
	if(_index < 0 || _index >= m_size.getX() * m_size.getY()) {
		position = { -1, -1 };
		return position;
	}
	int x = _index % m_size.getX();
	int y = _index / m_size.getX();
	position = { x, y };
	return position;
}

int Buffer::getIndex(const Vector2I& _position) const {
	if(_position.getX() < 0 || _position.getX() >= m_size.getX() || _position.getY() < 0 || _position.getY() >= m_size.getY()) {
		return -1;
	}
	return _position.getY() * m_size.getX() + _position.getX();
}

const Vector2I& Buffer::getSize() const {
	return m_size;
}

void Buffer::clear(char _char) {
	const int total = m_size.getX() * m_size.getY();
	std::fill_n(m_data, total, _char);
}

void Buffer::present() const {

	int width = m_size.getX();
	int height = m_size.getY();

	bool changed = false;
	for (int i = 0; i < width * height; i++) {
		if (m_data[i] != m_oldData[i]) {
			changed = true;
			break;
		}
	}
	if (!changed) {
		return;
	}

	std::string out;
	out.reserve(width * height * 4);

	for (int y = 0; y < height; y++) {
		int x = 0;
		while (x < width) {
			const int idx = y * width + x;
			const char newC = m_data[idx];
			const char oldC = m_oldData[idx];

			if (newC == oldC) {
				x++;
				continue;
			}

			int start = x;
			int run = 1;
			x++;
			while (x < width && m_data[y * width + x] == newC) {
				run++;
				x++;
			}

			int row = y + 1;
			int col = start * 2 + 1;

			out += "\033[";
			{
				char buf[16];
				std::snprintf(buf, sizeof(buf), "%d;", row);
				out += buf;
			}
			{
				char buf[16];
				std::snprintf(buf, sizeof(buf), "%dH", col);
				out += buf;
			}

			switch (newC) {
			case '.':
				out += BGREEN;
				break;
			case 'O':
				out += BBLUE;
				break;
			case 'P':
				out += BRED;
				break;
			case 'X':
				out += BYELLOW;
				break;
			case 'Z':
				out += BMAGENTA;
				break;
			default:
				out += BDEFAULT;
				break;
			}

			for (int i = 0; i < run; ++i) {
				out += "  ";
			}
		}
	}

	out += BDEFAULT;
	out += "\n";

	std::cout << out;
	std::cout.flush();

	std::memcpy(const_cast<char*>(m_oldData), m_data, width * height * sizeof(char));
}