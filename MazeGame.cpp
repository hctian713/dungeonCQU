#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>

#ifdef _WIN32
#include <conio.h> // For getch() on Windows
#else
#include <termios.h> // For getch() on Unix-based systems
#include <unistd.h> // For getch() on Unix-based systems
#endif

using namespace std;

const int ROWS = 12;         // Maze rows
const int COLS = 22;         // Maze columns
const int MAX_TRAPS = 15;    // Maximum number of traps per level
const int NUM_LEVELS = 10;   // Number of levels

const char EMPTY = ' ';     // Empty space
const char WALL = '#';      // Wall
const char TRAP = 'T';      // Trap
const char MONSTER = 'M';   // Monster
const char PLAYER = 'P';    // Player
const char START = 'S';     // Start
const char GOAL = 'G';      // Goal

struct Position {
    int row;
    int col;
};

struct Maze {
    vector<vector<char>> grid;
    Position start;
    Position goal;
};

class MazeGame {
private:
    Maze maze;
    Position player;
    Position monster;
    int completedLevel;

    void initializeMaze(int level) {
        maze.grid = vector<vector<char>>(ROWS, vector<char>(COLS, EMPTY));

        // Set walls
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                if (row == 0 || row == ROWS - 1 || col == 0 || col == COLS - 1) {
                    maze.grid[row][col] = WALL;
                }
            }
        }

        // Set start and goal positions
        maze.start = {1, 1};
        maze.goal = {ROWS - 2, COLS - 2};
        maze.grid[maze.start.row][maze.start.col] = START;
        maze.grid[maze.goal.row][maze.goal.col] = GOAL;

        // Randomly generate walls
        srand(time(nullptr) + level);
        int numWalls = level * 2;
        for (int i = 0; i < numWalls; ++i) {
            int row = rand() % (ROWS - 2) + 1;
            int col = rand() % (COLS - 2) + 1;
            maze.grid[row][col] = WALL;
        }

        // Randomly generate traps
        int numTraps = min(level, MAX_TRAPS);
        for (int i = 0; i < numTraps; ++i) {
            int row = rand() % (ROWS - 2) + 1;
            int col = rand() % (COLS - 2) + 1;
            maze.grid[row][col] = TRAP;
        }
    }

    void printMaze() const {
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                cout << maze.grid[row][col] << ' ';
            }
            cout << endl;
        }
    }

    char getInput() {
        char input;
#ifdef _WIN32
        input = getch(); // Get input from the keyboard on Windows
#else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        input = getchar(); // Get input from the keyboard on Unix-based systems (e.g., Linux, macOS)
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
        return input;
    }

    void movePlayer(char direction, bool& gameOver, bool& levelCompleted) {
        Position newPosition = player;

        switch (direction) {
            case 'w':
                newPosition.row--;
                break;
            case 's':
                newPosition.row++;
                break;
            case 'a':
                newPosition.col--;
                break;
            case 'd':
                newPosition.col++;
                break;
            default:
                return;
        }

        // Check if the new position is valid
        char newPositionCell = maze.grid[newPosition.row][newPosition.col];
        if (newPositionCell == WALL) {
            return; // Hit a wall, cannot move
        } else if (newPositionCell == TRAP || newPositionCell == MONSTER) {
            gameOver = true;
            return; // Game over, return to level selection
        } else if (newPosition.row == ROWS - 2 && newPosition.col == COLS - 2) {
            levelCompleted = true;
            return; // Level completed, return to level selection
        }

        // Update maze and player position
        maze.grid[player.row][player.col] = EMPTY;
        maze.grid[newPosition.row][newPosition.col] = PLAYER;
        player = newPosition;
    }

    void moveMonster() {
        int direction = rand() % 4;
        Position newPosition = monster;

        switch (direction) {
            case 0:
                newPosition.row--;
                break;
            case 1:
                newPosition.row++;
                break;
            case 2:
                newPosition.col--;
                break;
            case 3:
                newPosition.col++;
                break;
            default:
                return;
        }

        // Check if the new position is valid
        char newPositionCell = maze.grid[newPosition.row][newPosition.col];
        if (newPositionCell == WALL || newPositionCell == TRAP || newPositionCell == PLAYER) {
            return; // Hit a wall, trap, or player, cannot move
        }

        // Update maze and monster position
        maze.grid[monster.row][monster.col] = EMPTY;
        maze.grid[newPosition.row][newPosition.col] = MONSTER;
        monster = newPosition;
    }

    int selectLevel() const {
        int level;
        cout << "Select a level (1-" << NUM_LEVELS << "): ";

        while (!(cin >> level)) {
            cin.clear(); // Clear error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore input buffer
            cout << "Invalid input. Please enter a numeric value: ";
        }

        while (level < 1 || level > NUM_LEVELS) {
            cout << "Invalid level. Please select a level between 1 and " << NUM_LEVELS << ": ";

            while (!(cin >> level)) {
                cin.clear(); // Clear error state
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore input buffer
                cout << "Invalid input. Please enter a numeric value: ";
            }
        }

        return level;
    }

    void clearScreen() {
#ifdef _WIN32
        system("cls"); // Clear the screen on Windows
#else
        cout << "\033[2J\033[1;1H"; // Clear the screen on Unix-based systems (e.g., Linux, macOS)
#endif
    }

public:
    MazeGame() : completedLevel(0) {}

    void playGame() {
        char input;

        while (true) {
            int level;
            while (true) {
                level = selectLevel();

                if (level <= completedLevel) {
                    cout << "You have already completed this level. Please select a higher level." << endl;
                    continue;
                }

                initializeMaze(level);

                // Ensure the monster's initial position is not the same as the player's initial position
                do {
                    player = maze.start;
                    monster = {rand() % (ROWS - 2) + 1, rand() % (COLS - 2) + 1};
                } while (player.row == monster.row && player.col == monster.col);

                maze.grid[player.row][player.col] = PLAYER;
                maze.grid[monster.row][monster.col] = MONSTER;

                bool gameOver = false;
                bool levelCompleted = false;

                while (true) {
                    clearScreen();
                    printMaze();

                    cout << "Use WASD keys to move (W: up, A: left, S: down, D: right)" << endl;

                    input = getInput(); // Get input from the keyboard

                    movePlayer(input, gameOver, levelCompleted);
                    moveMonster();

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
    }
};


