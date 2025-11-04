#define WIDTH	16
#define HEIGHT	16

#include "buffer.h"
#include "player.h"
#include "goal.h"
#include "path.h"

#include <thread>

#include <stdio.h>
#include <conio.h>

void sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
void generateRandomGoal(Buffer* buffer, Vector2I& goal) {
	buffer->setValue(goal, '.');

	Vector2I generated_pos = { rand() % WIDTH, rand() % HEIGHT };

	while (buffer->getValue(generated_pos) == 'P') {
		generated_pos = { rand() % WIDTH, rand() % HEIGHT };
	}

	goal = generated_pos;

	buffer->setValue(goal, 'X');
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

#define NOMINMAX
#include <Windows.h>

#include <iostream>
#include <vector>

#define HIDE_CURSOR std::cout<<"\033[?25l";
#define SHOW_CURSOR std::cout<<"\033[?25h";

namespace pathfinding {
	Path basic_algorithm(Player& player, Buffer& buffer, Goal& goal) {
		
		Vector2I goal_position = goal.getPosition();
		Vector2I player_position = player.getPosition();
		
		Vector2I current_position = player_position;

		Path path(buffer);

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

		return path;
	}
	Path advanced_algorithm(Player& player, Buffer& buffer, Goal& goal) {
		Vector2I goal_position = goal.getPosition();
		Vector2I player_position = player.getPosition();

		Vector2I current_position = player_position;

		Path path(buffer);

		std::vector<int> score_container;

		while (current_position != goal_position) {
			// check wich cells are clear arround us
			std::vector<Vector2I> clear_cells;

			if(buffer.getValue(current_position + Vector2I{1, 0}) == '.') {
				clear_cells.push_back(current_position + Vector2I{1, 0});
			}
			if(buffer.getValue(current_position + Vector2I{-1, 0}) == '.') {
				clear_cells.push_back(current_position + Vector2I{-1, 0});
			}
			if(buffer.getValue(current_position + Vector2I{0, 1}) == '.') {
				clear_cells.push_back(current_position + Vector2I{0, 1});
			}
			if(buffer.getValue(current_position + Vector2I{0, -1}) == '.') {
				clear_cells.push_back(current_position + Vector2I{0, -1});
			}

			// choose the cell that is closest to the goal
			Vector2I best_cell = current_position;
			int best_distance = INT_MAX;
			for(const Vector2I& cell : clear_cells) {
				int distance = (cell - goal_position).magnitudeSquared();
				if(distance < best_distance) {
					best_distance = distance;
					best_cell = cell;
				}
			}

			// if stuck, break
			if(best_cell == current_position) {
				__debugbreak();
			}

			// if already visited, break
			if(std::find(visited_positions.begin(), visited_positions.end(), best_cell) != visited_positions.end()) {
				__debugbreak();
			}

			// move to the best cell
			current_position = best_cell;
			visited_positions.push_back(current_position);
		}

		return path;
	}
}

int main() {
	Buffer buffer({ WIDTH, HEIGHT });

	Player player(buffer);
	Goal goal(buffer, { WIDTH - 1, HEIGHT - 1 });

	Path path = pathfinding::advanced_algorithm(player, buffer, goal);

	HIDE_CURSOR

	while (true) {
		buffer.clear();

		Vector2I input = handleInputs();
		if(input != Vector2I::zero) {
			goal.move(input);
			path = pathfinding::advanced_algorithm(player, buffer, goal);
		}

		player.setPosition(player.getPosition());

		path.draw();

		player.draw();
		goal.draw();

		buffer.present();
	}

	return 0;
}