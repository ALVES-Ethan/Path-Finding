#pragma once

template <typename T>
class Vector2 {
public:
	Vector2(const T& _x = T(), const T& _y = T());

	T getX() const;
	T getY() const;

	void setX(const T& _x);
	void setY(const T& _y);

	Vector2<T> operator+(const Vector2<T>& _other) const;
	void operator+=(const Vector2<T>& _other);
	bool operator==(const Vector2<T>& _other) const;
private:
	T m_x;
	T m_y;
};

using Vector2I = Vector2<int>;

#include "maths.inl"