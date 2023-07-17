#include "maze.h"
#include <iostream>
using namespace std;
int main() {
    char input;
    int completedLevel = 0;

    while (true) {
        int level;
        while (true) {
            Maze maze;
            level = maze.selectLevel();

            if (level <= completedLevel) {
                cout << "You have already completed this level. Please select a higher level." << endl;
                continue;
            }

            maze.initializeMaze(level);

            Position player;
            Position monster;

            // Ensure the monster's initial position is not the same as the player's initial position
            do {
                player = maze.start;
                monster = {rand() % (maze.ROWS - 2) + 1, rand() % (maze.COLS - 2) + 1};
            } while (player.row == monster.row && player.col == monster.col);

            maze.grid[player.row][player.col] = maze.PLAYER;
            maze.grid[monster.row][monster.col] = maze.MONSTER;

            bool gameOver = false;
            bool levelCompleted = false;

            while (true) {
                maze.clearScreen();
                maze.printMaze();

                cout << "Use WASD keys to move (W: up, A: left, S: down, D: right)" << endl;

                input = maze.getInput(); // Get input from the keyboard

                maze.movePlayer(input, gameOver, levelCompleted);
                maze.moveMonster();

                if (gameOver) {
                    cout << "You were killed by a trap or a monster! Game over." << endl;
                    break; // Game over, return to level selection
                }

                if (levelCompleted) {
                    cout << "Congratulations! You successfully completed the level!" << endl;
                    completedLevel = level; // Update the completed level
                    break; // Level completed, break out of the loop
                }
            }

            cout << "Press Enter to continue..." << endl;
            cin.ignore();
            cin.get();

            break; // Return to level selection
        }
    }

    return 0;
}
