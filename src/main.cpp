#define WIDTH	32
#define HEIGHT	32

#include "buffer.h"
#include "player.h"

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
		case 'w':
			return { 0, -1 };
		case 's':
			return { 0, 1 };
		case 'a':
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
	void balec_algorithm(Player& player, Buffer& buffer, Vector2I goal) {
		Vector2I position = player.getPosition();

		if (position.getX() < goal.getX())
			player.move({ 1, 0 });
		else if (position.getX() > goal.getX())
			player.move({ -1, 0 });
		else if (position.getY() < goal.getY())
			player.move({ 0, 1 });
		else if (position.getY() > goal.getY())
			player.move({ 0, -1 });
	}

	void gambling_algorithm(Player& player, Buffer& buffer, Vector2I goal) {
		Vector2I position = player.getPosition();

		int randX = rand() % 3 - 1;
		int randY = rand() % 3 - 1;

		bool is_empty = buffer.getValue(position + Vector2I(randX, randY)) == '.';
		bool is_goal = buffer.getValue(position + Vector2I(randX, randY)) == 'X';

		if (is_empty || is_goal) {
			player.move({ randX, randY });
			return;
		}
	}

	void less_gambling_algorithm(Player& player, Buffer& buffer, Vector2I goal) {
		Vector2I position = player.getPosition();

		bool could_move = false;

		if (position.getX() < goal.getX() && !could_move) {
			if (buffer.getValue(position + Vector2I(1, 0)) == '.' || buffer.getValue(position + Vector2I(1, 0)) == 'X') {
				player.move({ 1, 0 });
				could_move = true;
			}
		}
		if (position.getX() > goal.getX() && !could_move) {
			if (buffer.getValue(position + Vector2I(-1, 0)) == '.' || buffer.getValue(position + Vector2I(-1, 0)) == 'X') {
				player.move({ -1, 0 });
				could_move = true;
			}
		}
		if (position.getY() < goal.getY() && !could_move) {
			if (buffer.getValue(position + Vector2I(0, 1)) == '.' || buffer.getValue(position + Vector2I(0, 1)) == 'X') {
				player.move({ 0, 1 });
				could_move = true;
			}
		}
		if (position.getY() > goal.getY() && !could_move) {
			if (buffer.getValue(position + Vector2I(0, -1)) == '.' || buffer.getValue(position + Vector2I(0, -1)) == 'X') {
				player.move({ 0, -1 });
				could_move = true;
			}
		}

		if (!could_move) {
			int randX = rand() % 3 - 1;
			int randY = rand() % 3 - 1;

			bool is_empty = buffer.getValue(position + Vector2I(randX, randY)) == '.';
			bool is_goal = buffer.getValue(position + Vector2I(randX, randY)) == 'X';

			if (is_empty || is_goal) {
				player.move({ randX, randY });
				return;
			}
		}
	}

	void almost_no_gambling_algorithm(Player& player, Buffer& buffer, Vector2I goal) {
		static std::vector<int> visited(WIDTH * HEIGHT, 0);
		static Vector2I last_pos = { -1, -1 };

		Vector2I position = player.getPosition();
		int position_index = buffer.getIndex(position);
		if (position_index >= 0 && position_index < WIDTH * HEIGHT)
			visited[position_index]++;

		auto in_bounds_and_free = [&](const Vector2I& p) -> bool {
			if (p.getX() < 0 || p.getX() >= WIDTH || p.getY() < 0 || p.getY() >= HEIGHT)
				return false;
			char v = buffer.getValue(p);
			return v == '.' || v == 'X';
			};

		auto manhattan = [&](const Vector2I& a, const Vector2I& b) -> int {
			return std::abs(a.getX() - b.getX()) + std::abs(a.getY() - b.getY());
			};

		const std::vector<Vector2I> dirs = { {1,0}, {-1,0}, {0,1}, {0,-1} };
		const int VISIT_WEIGHT = 100; // poids pour privilégier cellules peu visitées

		// Première passe : privilégier directions valides et éviter backtracking si possible
		int best_score = INT_MAX;
		Vector2I best_dir = { 0, 0 };
		for (const auto& d : dirs) {
			Vector2I np = position + d;
			if (!in_bounds_and_free(np)) continue;
			if (np == last_pos) continue; // éviter de revenir immédiatement si on a d'autres options

			int idx = buffer.getIndex(np);
			int visits = (idx >= 0 && idx < WIDTH * HEIGHT) ? visited[idx] : INT_MAX / 2;
			int score = visits * VISIT_WEIGHT + manhattan(np, goal);

			if (score < best_score) {
				best_score = score;
				best_dir = d;
			}
		}

		// Si trouvé une direction non-backtrack, l'utiliser
		if (!(best_dir.getX() == 0 && best_dir.getY() == 0)) {
			player.move(best_dir);
			last_pos = position;
			return;
		}

		// Deuxième passe : autoriser le backtrack si nécessaire
		best_score = INT_MAX;
		best_dir = { 0, 0 };
		for (const auto& d : dirs) {
			Vector2I np = position + d;
			if (!in_bounds_and_free(np)) continue;

			int idx = buffer.getIndex(np);
			int visits = (idx >= 0 && idx < WIDTH * HEIGHT) ? visited[idx] : INT_MAX / 2;
			int score = visits * VISIT_WEIGHT + manhattan(np, goal);

			if (score < best_score) {
				best_score = score;
				best_dir = d;
			}
		}

		if (!(best_dir.getX() == 0 && best_dir.getY() == 0)) {
			player.move(best_dir);
			last_pos = position;
			return;
		}

		// Dernier recours : choisir aléatoirement parmi voisins valides
		std::vector<Vector2I> candidates;
		for (const auto& d : dirs) {
			Vector2I np = position + d;
			if (in_bounds_and_free(np))
				candidates.push_back(d);
		}

		if (!candidates.empty()) {
			Vector2I pick = candidates[std::rand() % candidates.size()];
			player.move(pick);
			last_pos = position;
			return;
		}

		// Aucune action possible : rester sur place
		return;
	}
}

int main() {
	Buffer buffer({ WIDTH, HEIGHT });
	buffer.fill('.');

	Player player(&buffer);

	std::srand(2);

	for (int i = 0; i < (WIDTH * HEIGHT) / 4; i++) {
		Vector2I obstacle_pos = { rand() % WIDTH, rand() % HEIGHT };
		while (buffer.getValue(obstacle_pos) != '.') {
			obstacle_pos = { rand() % WIDTH, rand() % HEIGHT };
		}
		buffer.setValue(obstacle_pos, 'P');
	}

	Vector2I goal = { 0 };
	generateRandomGoal(&buffer, goal);

	HIDE_CURSOR

	while (true) {
		pathfinding::almost_no_gambling_algorithm(player, buffer, goal);

		if (player.getPosition() == goal)
			generateRandomGoal(&buffer, goal);

		Vector2I input = handleInputs();
		if (input.getX() != 0 || input.getY() != 0) {
			buffer.setValue(goal, '.');

			goal += input;

			if (goal.getX() < 0) goal.setX(0);
			if (goal.getX() >= WIDTH) goal.setX(WIDTH - 1);
			if (goal.getY() < 0) goal.setY(0);
			if (goal.getY() >= HEIGHT) goal.setY(HEIGHT - 1);

			buffer.setValue(goal, 'X');
		}

		buffer.draw();

		sleep(2);
	}

	return 0;
}