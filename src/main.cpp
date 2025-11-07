#define WIDTH	50
#define HEIGHT	50

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

char get_input() {
	if (_kbhit()) {
		char input = _getch();
		return input;
	}
	return '\0';
}
Vector2I handleInputs(char _input) {
	switch (_input) {
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

#define HIDE_CURSOR std::cout<<"\033[?25l";
#define SHOW_CURSOR std::cout<<"\033[?25h";

std::vector<Vector2I> g_visited_points;
std::vector<int> g_scores;
int queue_calls = 0;

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

		score_container[buffer.getIndex(current_position)] = (current_position - goal_position).magnitude(true);
		
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
				float magnitude_squared = (cell - goal_position).magnitude(true);
				int cell_index = buffer.getIndex(cell);
				score_container[cell_index] = magnitude_squared;
			}

			path.addPoint(current_position);
		}
	}
	void a_star_algorithm(Path& path, const Player& player, const Buffer& buffer, const Goal& goal) {
		g_visited_points.clear();
		g_scores.clear();
		
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
				grid[index].dist_to_player = std::numeric_limits<int>::max(); // g
				grid[index].dist_to_goal = grid[index].position.manhattan(goal_position); // h
				grid[index].from = nullptr;
				g_scores.push_back(0);
			}
		}

		std::priority_queue<Point*, std::vector<Point*>, PointComparator> queue;

		Point* start = &grid[buffer.getIndex(player_position)];
		start->dist_to_player = 0;
		start->dist_to_goal = start->position.manhattan(goal_position);

		queue_calls++;
		queue.push(start);

		while (!queue.empty()) {
			Point* current_point = queue.top();
			queue.pop();

			if (current_point->visited)
				continue;

			g_visited_points.push_back(current_point->position);

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

				if (buffer.getValue(neighbor_position) != '.')
					continue;

				Point* neighbor = &grid[neighbor_index];

				if (neighbor->visited)
					continue;

				int new_neighbor_dist_to_player = current_point->dist_to_player + 1;

				if (new_neighbor_dist_to_player < neighbor->dist_to_player) {
					neighbor->from = current_point;
					neighbor->dist_to_player = new_neighbor_dist_to_player;
					neighbor->dist_to_goal = neighbor->position.manhattan(goal_position);

					queue.push(neighbor);
					queue_calls++;
					g_scores[neighbor_index] = neighbor->dist_to_goal + neighbor->dist_to_player;
					g_visited_points.push_back(neighbor->position);
				}
			}
		}
	}
}

int main() {
	Buffer buffer({ WIDTH, HEIGHT });
	g_scores.reserve(WIDTH * HEIGHT);
	buffer.setScoresContainer(&g_scores);

	Player player(buffer);
	player.setPosition({ 8, 8 });

	Goal goal(buffer, { WIDTH - 1, HEIGHT - 1 });

	Path path(buffer);

	std::vector<Vector2I> obstacles;
	for (int i = 0; i < (WIDTH * HEIGHT) / 8; i++) {
		Vector2I obstacle_position;
		do {
			obstacle_position = Vector2I(rand() % WIDTH, rand() % HEIGHT);
		} while (obstacle_position == player.getPosition() || obstacle_position == goal.getPosition() ||
			buffer.getValue(obstacle_position) == 'X');
		obstacles.push_back(obstacle_position);
		buffer.setValue(obstacle_position, 'X');
	}

	pathfinding::a_star_algorithm(path, player, buffer, goal);

	HIDE_CURSOR

	buffer.present();

	//std::cout << "Press WASD to move the goal\n";

	for (const Vector2I& obstacle : obstacles) {
		buffer.setValue(obstacle, 'P');
	}

	float time_accumulator = 0.0f;

	while (true) {
		buffer.clear();

		time_accumulator += 0.0001f;

		for (const Vector2I& obstacle : obstacles) {
			buffer.setValue(obstacle, 'P');
		}

		char input = get_input();

		bool flush = false;
		if(input == 'r') {
			flush = true;
			path.clearPoints();
			queue_calls = 0;
			pathfinding::a_star_algorithm(path, player, buffer, goal);
		}

		if (input == ' ') {
			obstacles.clear();
			for (int i = 0; i < (WIDTH * HEIGHT) / 8; i++) {
				Vector2I obstacle_position;
				do {
					obstacle_position = Vector2I(rand() % WIDTH, rand() % HEIGHT);
				} while (obstacle_position == player.getPosition() || obstacle_position == goal.getPosition() ||
					buffer.getValue(obstacle_position) == 'X');
				obstacles.push_back(obstacle_position);
				buffer.setValue(obstacle_position, 'X');
			}

			flush = true;
			path.clearPoints();
			queue_calls = 0;
			pathfinding::a_star_algorithm(path, player, buffer, goal);
		}

		Vector2I direction = handleInputs(input);
		if(direction != Vector2I::zero) {
			goal.move(direction);
			path.clearPoints();
			queue_calls = 0;
			pathfinding::a_star_algorithm(path, player, buffer, goal);
		}

		player.setPosition(player.getPosition());

		path.draw();
		player.draw();
		goal.draw();

		for (const Vector2I& visited_point : g_visited_points) {
			if (buffer.getValue(visited_point) == '.')
				buffer.setValue(visited_point, 'W');
		}

		buffer.drawTextBellowBuffer("A* Algorithm: " + std::to_string(queue_calls) + " queue calls.");

		buffer.present(flush);
	}

	return 0;
}