/*
 * @Author: WHURS-THC
 * @Date: 2023-07-17 14:26:01
 * @LastEditTime: 2023-07-17 15:29:29
 * @Description: 
 */
#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <string>

class Position;
class Player;
class Monster;
class Trap;
class Wall;

class Position {
public:
    Position();
    Position(int row, int col);
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    int getRow() const;
    int getCol() const;


    int row;
    int col;
};

class Maze {
public:
    Maze();
    void initialize(int level);
    void print() const;
    void movePlayer(char direction, bool& gameOver, bool& levelCompleted);
    void moveMonsters();
    bool isCompleted() const;
    bool isGameOver() const;

public:
    const int ROWS;
    const int COLS;
    const int MAX_TRAPS;
    const int NUM_LEVELS;

    std::vector<std::vector<char>> grid;
    Position start;
    Position goal;
    Player* player;
    std::vector<Monster*> monsters;
    std::vector<Trap*> traps;
    std::vector<Wall*> walls;

    void setWalls();
    void setStartAndGoal();
    void generateRandomWalls(int numWalls);
    void generateRandomTraps(int numTraps);
};



class Player {
public:
    Player();
    void setPosition(const Position& position);
    Position getPosition() const;

    Position position;
};

class Monster {
public:
    Monster();
    Monster(const Position& position);
    void setPosition(const Position& position);
    Position getPosition() const;

    Position position;
};

class Trap {
public:
    Trap();
    Trap(const Position& position);
    void setPosition(const Position& position);
    Position getPosition() const;

    Position position;
};

class Wall {
public:
    Wall();
    Wall(const Position& position);
    void setPosition(const Position& position);
    Position getPosition() const;

    Position position;
};

#endif  // MAZE_H
