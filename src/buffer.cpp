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

void Buffer::present(bool _flush) const {
	if(_flush) {
		std::cout << "\033[2J"; // Clear screen
		std::cout << "\033[H";  // Move cursor to home position

		// redraw entire buffer
		for (int y = 0; y < m_size.getY(); y++) {
			for (int x = 0; x < m_size.getX(); x++) {
				const int idx = y * m_size.getX() + x;
				const char newC = m_data[idx];
				switch (newC) {
				case '.': // empty cell
					std::cout << BGREEN << "  ";
					break;
				case 'O': // player cell
					std::cout << BBLUE << "  ";
					break;
				case 'P': // obstacle cell
					std::cout << BRED << "  ";
					break;
				case 'X': // goal cell
					std::cout << BYELLOW << "  ";
					break;
				case 'Z': // path cell
					std::cout << BMAGENTA << "  ";
					break;
				case 'W': // visited cell
					std::cout << BCYAN << "  ";
					break;
				default:
					std::cout << BDEFAULT << "  ";
					break;
				}
			}
			std::cout << BDEFAULT << "\n";
		}
	}

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
			case '.': // empty cell
				out += BGREEN;
				break;
			case 'O': // player cell
				out += BBLUE;
				break;
			case 'P': // obstacle cell
				out += BRED;
				break;
			case 'X': // goal cell
				out += BYELLOW;
				break;
			case 'Z': // path cell
				out += BMAGENTA;
				break;
			case 'W': // visited cell
				out += BCYAN;
				break;
			default:
				out += BDEFAULT;
				break;
			}

			for (int i = 0; i < run; ++i) {
				// check if a score exist for current position
				// TEMPORARY
				if (m_scores) {
					int buffer_index = y * width + (start + i);
					int score = (*m_scores)[buffer_index];
					if (score > 0) {
						if (score <= 99) {
							if (score < 10) {
								out += " " + std::to_string(score);
							}
							else {
								out += std::to_string(score);
							}
						}
						else {
							out += "##";
						}

						continue;
					}
				}
				// END TEMPORARY

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

void Buffer::drawTextBellowBuffer(const std::string& _text) const
{
	int row = m_size.getY() + 1;
	int col = 1;
	std::cout << "\033[" << row << ";" << col << "H";
	std::cout << _text;
	std::cout.flush();
}