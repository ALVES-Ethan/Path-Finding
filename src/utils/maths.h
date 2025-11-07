#pragma once

template <typename T>
class Vector2 {
public:
	Vector2(const T& _x = T(), const T& _y = T());

	T getX() const;
	T getY() const;

	void setX(const T& _x);
	void setY(const T& _y);

	T manhattan(const Vector2<T>& _other) const;
	T magnitude(bool _squared = true) const;

	Vector2<T> operator+(const Vector2<T>& _other) const;
	Vector2<T> operator-(const Vector2<T>& _other) const;

	void operator+=(const Vector2<T>& _other);

	bool operator==(const Vector2<T>& _other) const;
	bool operator!=(const Vector2<T>& _other) const;

	bool operator<(const Vector2<T>& _other) const;
	bool operator>(const Vector2<T>& _other) const;

	static Vector2<T> zero;
	static Vector2<T> one;
private:
	T m_x;
	T m_y;
};

using Vector2I = Vector2<int>;

#include "maths.inl"