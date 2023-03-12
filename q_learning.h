#include "snake.h"

#ifndef QLEARNING_H
#define QLEARNING_H



struct List {
    int snake;
    int wall;
    int apple;
};

typedef enum Obsticles {
    GAME, SNAKE, WALL, APPLE
} obst;

#define EPSILON 0.8
#define WIND 0.01
#define GAMMA 0.95
#define ALPHA 0.65

void add_snake_to_grid(int **grid, int size_grid_x, int size_grid_y, snake_t *the_snake, apple_t *apple);
void learningQ(double ***Q_grid, int **game_grid, int nr_of_games, snake_t *the_snake);
void updateQGrid(double ***Q_grid, int **game_grid, snake_t *the_snake, apple_t *apple, int nr_of_games);
double ***generateNewQGrid();
int **generateNewGameGrid(snake_t *the_snake, apple_t* apple);

void free_2d_matrix(double **matrix, int rows);
int create_move(double ***q_grid, snake_t *the_snake);
void reset_Q_matrix(double ***Q, int size_grid_x, int size_grid_y);
int create_moves0(double ***q_grid, int **game_grid, snake_t *the_snake, apple_t *the_apple);
void free_arr(int *arr);


#endif