#pragma once

#include "utils/drawable.h"
#include "utils/maths.h"

#include <vector>

struct Point {
public:
	Vector2I position;
	int index;

	int dist_to_player;

	bool visited;

	Point* from;
};

class Path : public Drawable {
public:
	Path(Buffer& _buffer);

	void addPoint(const Vector2I& _point);
	void removePoint(const Vector2I& _point);
	void removeLastPoint();
	void clearPoints();

	void draw() const override;

private:
	std::vector<Vector2I> m_data;
};