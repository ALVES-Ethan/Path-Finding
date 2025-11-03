#include "maths.h"

template<typename T>
inline Vector2<T>::Vector2(const T& _x, const T& _y) : m_x(_x), m_y(_y) {
}

template<typename T>
inline T Vector2<T>::getX() const {
	return m_x;
}

template<typename T>
inline T Vector2<T>::getY() const {
	return m_y;
}

template<typename T>
inline void Vector2<T>::setX(const T& _x) {
	m_x = _x;
}

template<typename T>
inline void Vector2<T>::setY(const T& _y) {
	m_y = _y;
}

template<typename T>
inline Vector2<T> Vector2<T>::operator+(const Vector2<T>& _other) const {
	return Vector2<T>(m_x + _other.m_x, m_y + _other.m_y);
}

template<typename T>
inline void Vector2<T>::operator+=(const Vector2<T>& _other) {
	m_x += _other.m_x;
	m_y += _other.m_y;
}

template<typename T>
inline bool Vector2<T>::operator==(const Vector2<T>& _other) const {
	return (m_x == _other.m_x) && (m_y == _other.m_y);
}