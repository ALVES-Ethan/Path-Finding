#pragma once

#include "utils/drawable.h"
#include "utils/maths.h"

#include <vector>

struct Point {
public:
	Vector2I position;
	int index;

	int dist_to_player;
	int dist_to_goal;

	bool visited;
	Point* from;
};

class PointComparator {
public:
	bool operator()(Point* _a, Point* _b) {
		int score_a = _a->dist_to_player + _a->dist_to_goal;
		int score_b = _b->dist_to_player + _b->dist_to_goal;
		return score_a > score_b;
	}
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