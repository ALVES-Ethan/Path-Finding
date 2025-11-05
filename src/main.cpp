#define WIDTH	16
#define HEIGHT	16

#include "buffer.h"
#include "player.h"
#include "goal.h"
#include "path.h"

#include <thread>

#include <stdio.h>
#include <conio.h>

#include <array>
#include <vector>
#include <queue>

#define NOMINMAX
#include <Windows.h>

#include <iostream>

void sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
Vector2I handleInputs() {
	if (_kbhit()) {
		char input = _getch();

		switch (input) {
		case 'z':
			return { 0, -1 };
		case 's':
			return { 0, 1 };
		case 'q':
			return { -1, 0 };
		case 'd':
			return { 1, 0 };
		default:
			return { 0, 0 };
		}
	}

	return { 0, 0 };
}

#define HIDE_CURSOR std::cout<<"\033[?25l";
#define SHOW_CURSOR std::cout<<"\033[?25h";

namespace pathfinding {
	void basic_algorithm(Path& path, const Player& player, const Buffer& buffer, const Goal& goal) {
		Vector2I goal_position = goal.getPosition();
		Vector2I player_position = player.getPosition();
		
		Vector2I current_position = player_position;

		while (current_position != goal_position) {
			

			if(current_position.getX() < goal_position.getX()) {
				current_position += { 1, 0 };
			} else if(current_position.getX() > goal_position.getX()) {
				current_position += { -1, 0 };
			} else if(current_position.getY() < goal_position.getY()) {
				current_position += { 0, 1 };
			} else if(current_position.getY() > goal_position.getY()) {
				current_position += { 0, -1 };
			}
			path.addPoint(current_position);
		}
	}
	void first_depth_search(Path& path, const Player& player, const Buffer& buffer, const Goal& goal) {
		Vector2I goal_position = goal.getPosition();
		Vector2I player_position = player.getPosition();

		Vector2I current_position = player_position;

		std::array<float, 16 * 16> score_container;
		score_container.fill(std::numeric_limits<float>::max());
		std::array<bool, 16 * 16> visited_container;
		visited_container.fill(false);

		score_container[buffer.getIndex(current_position)] = (current_position - goal_position).magnitudeSquared();
		
		while (current_position != goal_position) {
			float best_score = std::numeric_limits<float>::max();
			int best_index = -1;
			for (int i = 0; i < score_container.size(); i++) {
				if (!visited_container[i] && score_container[i] < best_score) {
					best_score = score_container[i];
					best_index = i;
				}
			}

			// move to best cell
			current_position = buffer.getPosition(best_index);

			// remove used score
			score_container[best_index] = (std::numeric_limits<float>::max());

			// mark it as visited
			visited_container[best_index] = true;

			// check wich cells are clear arround us
			std::vector<Vector2I> clear_cells;

			if(buffer.getValue(current_position + Vector2I{1, 0}) == '.' || buffer.getValue(current_position + Vector2I{ 1, 0 }) == 'X') {
				clear_cells.push_back(current_position + Vector2I{1, 0});
			}
			if(buffer.getValue(current_position + Vector2I{-1, 0}) == '.' || buffer.getValue(current_position + Vector2I{ -1, 0 }) == 'X') {
				clear_cells.push_back(current_position + Vector2I{-1, 0});
			}
			if(buffer.getValue(current_position + Vector2I{0, 1}) == '.' || buffer.getValue(current_position + Vector2I{ 0, 1 }) == 'X') {
				clear_cells.push_back(current_position + Vector2I{0, 1});
			}
			if(buffer.getValue(current_position + Vector2I{0, -1}) == '.' || buffer.getValue(current_position + Vector2I{ 0, -1 }) == 'X') {
				clear_cells.push_back(current_position + Vector2I{0, -1});
			}

			// add cells to container
			for (Vector2I& cell : clear_cells) {
				float magnitude_squared = (cell - goal_position).magnitudeSquared();
				int cell_index = buffer.getIndex(cell);
				score_container[cell_index] = magnitude_squared;
			}

			path.addPoint(current_position);
		}
	}
	void a_star_algorithm(Path& path, const Player& player, const Buffer& buffer, const Goal& goal) {
		Vector2I goal_position = goal.getPosition();
		Vector2I player_position = player.getPosition();
		
		std::vector<Point> grid;
		grid.reserve(WIDTH * HEIGHT);
		grid.resize(WIDTH * HEIGHT);
		
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				int index = y * WIDTH + x;
				grid[index].position = { x, y };
				grid[index].index = index;
				grid[index].visited = false;
				grid[index].dist_to_player = std::numeric_limits<int>::max();
				grid[index].from = nullptr;
			}
		}

		std::priority_queue<Point*, std::vector<Point*>, std::greater<Point*>> queue;

		queue.push(&grid[buffer.getIndex(player_position)]);

		while (!queue.empty()) {
			Point* current_point = queue.top();
			queue.pop();

			if (current_point->visited)
				continue;

			// compute distance to goal
			float dist_to_goal = (current_point->position - goal_position).magnitudeSquared();
			current_point->dist_to_player = dist_to_goal;


			current_point->visited = true;

			if(current_point->position == goal_position) {
				std::vector<Vector2I> path_points;
				Point* point = current_point;
				while (point != nullptr) {
					path_points.push_back(point->position);
					point = point->from;
				}

				std::reverse(path_points.begin(), path_points.end());

				for (const Vector2I& point : path_points)
					path.addPoint(point);

				return;
			}

			Vector2I directions[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

			for (const Vector2I& direction : directions) {
				Vector2I neighbor_position = current_point->position + direction;
				int neighbor_index = buffer.getIndex(neighbor_position);

				if (neighbor_index == -1)
					continue;

				if (buffer.getValue(neighbor_position) != '.' ||
					buffer.getValue(neighbor_position) == 'X')
					continue;

				if (grid[neighbor_index].visited)
					continue;

				Point* neighbor = &grid[neighbor_index];
				neighbor->dist_to_player = (neighbor->position - goal_position).magnitudeSquared();
				neighbor->from = current_point;
				queue.push(neighbor);	
			}
		}
	}
}

int main() {
	Buffer buffer({ WIDTH, HEIGHT });

	Player player(buffer);
	Goal goal(buffer, { WIDTH - 1, HEIGHT - 1 });

	Path path(buffer);

	pathfinding::a_star_algorithm(path, player, buffer, goal);


	HIDE_CURSOR

	while (true) {
		buffer.clear();

		// add obstacles
		buffer.setValue({ 5, 5 }, 'P');
		buffer.setValue({ 5, 6 }, 'P');
		buffer.setValue({ 5, 7 }, 'P');
		buffer.setValue({ 5, 8 }, 'P');

		buffer.setValue({ 10, 3 }, 'P');
		buffer.setValue({ 10, 4 }, 'P');
		buffer.setValue({ 10, 5 }, 'P');

		buffer.setValue({ 8, 5 }, 'P');
		buffer.setValue({ 9, 5 }, 'P');
		buffer.setValue({ 10, 5 }, 'P');

		buffer.setValue({ 10, 12 }, 'P');
		buffer.setValue({ 10, 13 }, 'P');
		buffer.setValue({ 10, 16 }, 'P');

		Vector2I input = handleInputs();
		if(input != Vector2I::zero) {
			goal.move(input);
			path.clearPoints();
			pathfinding::a_star_algorithm(path, player, buffer, goal);
		}

		player.setPosition(player.getPosition());

		path.draw();

		player.draw();
		goal.draw();

		buffer.present();
	}

	return 0;
}