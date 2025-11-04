#pragma once

#include "utils/drawable.h"
#include "utils/maths.h"

#include <vector>

class Path : public Drawable {
public:
	Path(Buffer& _buffer);

	void addPoint(const Vector2I& _point);

	void draw() const override;

private:
	std::vector<Vector2I> m_data;
};