#include "maze.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Maze::Maze()
    : ROWS(12), COLS(22), MAX_TRAPS(15), NUM_LEVELS(10) {}

void Maze::initialize(int level) {
    grid = std::vector<std::vector<char>>(ROWS, std::vector<char>(COLS, ' '));
    setWalls();
    setStartAndGoal();
    generateRandomWalls(level * 2);
    generateRandomTraps(std::min(level, MAX_TRAPS));
    player = new Player();
    player->setPosition(start);
    monsters.clear();

    srand(time(nullptr) + level);
    int numMonsters = std::min(level, 3);
    for (int i = 0; i < numMonsters; ++i) {
        Position monsterPos;
        do {
            monsterPos = {rand() % (ROWS - 2) + 1, rand() % (COLS - 2) + 1};
        } while (monsterPos == player->getPosition());

        monsters.push_back(new Monster(monsterPos));
    }
}

void Maze::print() const {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            char cell = ' ';
            if (player->getPosition() == Position(row, col)) {
                cell = 'P';
            } else if (std::find_if(monsters.begin(), monsters.end(), [row, col](const Monster* m) {
                           return m->getPosition() == Position(row, col);
                       }) != monsters.end()) {
                cell = 'M';
            } else if (std::find_if(traps.begin(), traps.end(), [row, col](const Trap* t) {
                           return t->getPosition() == Position(row, col);
                       }) != traps.end()) {
                cell = 'T';
            } else if (std::find_if(walls.begin(), walls.end(), [row, col](const Wall* w) {
                           return w->getPosition() == Position(row, col);
                       }) != walls.end()) {
                cell = '#';
            } else if (row == start.getRow() && col == start.getCol()) {
                cell = 'S';
            } else if (row == goal.getRow() && col == goal.getCol()) {
                cell = 'G';
            }
            std::cout << cell << ' ';
        }
        std::cout << std::endl;
    }
}

void Maze::movePlayer(char direction, bool& gameOver, bool& levelCompleted) {
    Position newPosition = player->getPosition();

    switch (direction) {
        case 'w':
            newPosition = Position(newPosition.getRow() - 1, newPosition.getCol());
            break;
        case 's':
            newPosition = Position(newPosition.getRow() + 1, newPosition.getCol());
            break;
        case 'a':
            newPosition = Position(newPosition.getRow(), newPosition.getCol() - 1);
            break;
        case 'd':
            newPosition = Position(newPosition.getRow(), newPosition.getCol() + 1);
            break;
        default:
            return;
    }

    char newPositionCell = grid[newPosition.getRow()][newPosition.getCol()];
    if (newPositionCell == '#') {
        return;
    } else if (newPositionCell == 'T' || newPositionCell == 'M') {
        gameOver = true;
        return;
    } else if (newPosition == goal) {
        levelCompleted = true;
        return;
    }

    grid[player->getPosition().getRow()][player->getPosition().getCol()] = ' ';
    grid[newPosition.getRow()][newPosition.getCol()] = 'P';
    player->setPosition(newPosition);
}

void Maze::moveMonsters() {
    for (Monster* monster : monsters) {
        Position newPosition = monster->getPosition();

        int direction = rand() % 4;

        switch (direction) {
            case 0:
                newPosition = Position(newPosition.getRow() - 1, newPosition.getCol());
                break;
            case 1:
                newPosition = Position(newPosition.getRow() + 1, newPosition.getCol());
                break;
            case 2:
                newPosition = Position(newPosition.getRow(), newPosition.getCol() - 1);
                break;
            case 3:
                newPosition = Position(newPosition.getRow(), newPosition.getCol() + 1);
                break;
            default:
                continue;
        }

        char newPositionCell = grid[newPosition.getRow()][newPosition.getCol()];
        if (newPositionCell == '#' || newPositionCell == 'T' || newPositionCell == 'P') {
            continue;
        }

        grid[monster->getPosition().getRow()][monster->getPosition().getCol()] = ' ';
        grid[newPosition.getRow()][newPosition.getCol()] = 'M';
        monster->setPosition(newPosition);
    }
}

bool Maze::isCompleted() const {
    return player->getPosition() == goal;
}

bool Maze::isGameOver() const {
    for (Monster* monster : monsters) {
        if (monster->getPosition() == player->getPosition()) {
            return true;
        }
    }

    for (Trap* trap : traps) {
        if (trap->getPosition() == player->getPosition()) {
            return true;
        }
    }

    return false;
}

void Maze::setWalls() {
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (row == 0 || row == ROWS - 1 || col == 0 || col == COLS - 1) {
                walls.push_back(new Wall(Position(row, col)));
            }
        }
    }
}

void Maze::setStartAndGoal() {
    start = Position(1, 1);
    goal = Position(ROWS - 2, COLS - 2);
}

void Maze::generateRandomWalls(int numWalls) {
    srand(time(nullptr));
    int count = 0;

    while (count < numWalls) {
        int row = rand() % (ROWS - 2) + 1;
        int col = rand() % (COLS - 2) + 1;

        Position pos(row, col);
        if (pos != start && pos != goal &&
            std::find_if(walls.begin(), walls.end(), [&pos](const Wall* w) {
                return w->getPosition() == pos;
            }) == walls.end()) {
            walls.push_back(new Wall(pos));
            count++;
        }
    }
}

void Maze::generateRandomTraps(int numTraps) {
    srand(time(nullptr));
    int count = 0;

    while (count < numTraps) {
        int row = rand() % (ROWS - 2) + 1;
        int col = rand() % (COLS - 2) + 1;

        Position pos(row, col);
        if (pos != start && pos != goal &&
            std::find_if(walls.begin(), walls.end(), [&pos](const Wall* w) {
                return w->getPosition() == pos;
            }) == walls.end() &&
            std::find_if(traps.begin(), traps.end(), [&pos](const Trap* t) {
                return t->getPosition() == pos;
            }) == traps.end()) {
            traps.push_back(new Trap(pos));
            count++;
        }
    }
}

Position::Position() : row(0), col(0) {}

Position::Position(int row, int col) : row(row), col(col) {}

bool Position::operator==(const Position& other) const {
    return row == other.row && col == other.col;
}

int Position::getRow() const {
    return row;
}

int Position::getCol() const {
    return col;
}

Player::Player() {}

void Player::setPosition(const Position& position) {
    this->position = position;
}

Position Player::getPosition() const {
    return position;
}

Monster::Monster() {}

Monster::Monster(const Position& position) : position(position) {}

void Monster::setPosition(const Position& position) {
    this->position = position;
}

Position Monster::getPosition() const {
    return position;
}

Trap::Trap() {}

Trap::Trap(const Position& position) : position(position) {}

void Trap::setPosition(const Position& position) {
    this->position = position;
}

Position Trap::getPosition() const {
    return position;
}

Wall::Wall() {}

Wall::Wall(const Position& position) : position(position) {}

void Wall::setPosition(const Position& position) {
    this->position = position;
}

Position Wall::getPosition() const {
    return position;
}
