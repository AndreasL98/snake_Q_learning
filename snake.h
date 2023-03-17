/*
 *
 *    *** NOTHING TO DO ****
 * 
 *    The public API for the snake module (function declarations)
 *    Used by main.c (the GUI) in functions init and update
 *
 */
#include <stdbool.h>

#ifndef SNAKE_H
#define SNAKE_H

#define GAME_MAX_X 300
#define GAME_MAX_Y 300

#define SEGMENT_WIDTH 10
#define SEGMENT_HEIGHT 10

// The direction of the snake head
// NOTE: In computer graphics y-axis points down
typedef enum Dir {
    UP, RIGHT, DOWN, LEFT
} dir_t;

// A segment of the snake
typedef struct Segment {
    int x;                 // Top left corner
    int y;
    struct Segment *prev;  // Previous segment
} seg_t;

// The full snake
typedef struct Snake {
    int width;         // Width for all segments (when rendering)
    int height;        // Height for all segments (when rendering)
    dir_t dir;         // Direction of snake
    int length;        // Length of snake. Used for points for player (default 2)
    seg_t *head;       // Pointer to head. NOTE: This should never change!
    seg_t *last;       // Pointer to last segment
} snake_t;

// The apple
typedef struct Apple {
    int x;              // Top
    int y;              // Left
    int width;
    int height;
} apple_t;

// -------- Snake functions (used in main.c) ---------------

// Create a new dynamically allocated snake (with length 2 and direction RIGHT)
snake_t *snake_new(int x, int y, int width, int height);

// Move snake one step (distance == snake->width)
void snake_move(snake_t *the_snake);

// Will create a segment and append it (add it last in snake)
void snake_append_segment(snake_t *the_snake);

void snake_turn(snake_t *the_snake, dir_t);

bool snake_hit_self(snake_t *the_snake);

bool snake_hit_apple(snake_t *the_snake, apple_t *the_apple);

bool snake_hit_wall(snake_t *the_snake);

// -------- Apple functions---------------

// Get a new dynamically allocated apple with random position
apple_t *apple_new();

// Check is Apple is intersecting with snake
int apple_snake_intersect(snake_t *the_snake, int x, int y);

// Deallocate apple
void apple_destroy(apple_t *the_apple);

#endif //SNAKE_H
