/*
 *
 *   Implementation (function definitions) of public API (snake.h)
 *   and helper functions
 *
 *  This is how the snake-pointers looks
 *
 *
 *               snake_t      seg_t       seg_t      seg_t
 *             |-------|    |-------|   |-------|   |-------|
 *             | head -|--->|       |   |       |   |       |
 *    snake--> |       | |--|-prev  |<--|-prev  |<--|- prev |
 *             |       |    |_______|   |_______|   |_______|<---|
 *             | last -|-----------------------------------------|
 *             |_______|
 *
 *
 *    NOTE: No error checks here
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include "snake.h"  // Include to check declaration/definition the same

static char *to_str(snake_t *the_snake);

// gcc `pkg-config gtk+-3.0 --cflags` snake.c main.c -o out_main_2 `pkg-config gtk+-3.0 --libs`


// -----------------------  Segment functions ------------------

static seg_t *segment_new(int x, int y, seg_t *prev) {
    seg_t *seg = malloc(sizeof(seg_t));
    if (seg == NULL) {
        perror("Couldn't create segment, giving up");
        exit(EXIT_FAILURE);
    }
    seg->x = x;
    seg->y = y;
    seg->prev = prev;
    return seg;
}

//--------------- Snake functions  --------------------------

snake_t *snake_new(int x, int y, int width, int height) {

    // TODO Create a snake with head and tail (length = 2) and direction right

    snake_t *snek = malloc(sizeof(snake_t));
    seg_t *seg = segment_new(x, y, NULL);

    seg_t *tail = segment_new(x-width, y, seg);

    // SETTING SNAKE VALUES
    snek->width = width;
    snek->height = height;
    snek->dir = RIGHT;
    snek->head = seg;
    snek->last = tail;
    snek->length = 2;

    

    // return 0;   // 0 for now should be pointer to snake;
    return snek;
}

// static char *to_str(snake_t *the_snake);

// NOTE: The position of the appended segment doesn't
// matter. The new segment will get a positions at next
// move.
void snake_append_segment(snake_t *the_snake) {


    int tail_x = the_snake->last->x; //Get Tail x and y
    int tail_y = the_snake->last->y;

    // Create new segment with tail coordinates and snake tail as prev;
    seg_t *new_segment = segment_new(tail_x,tail_y, the_snake->last);
    the_snake->last = new_segment;

    // Add length to snake
    the_snake->length = the_snake->length + 1;

 
}

/*
 *  Alt1: Shift all positions (values) from head towards last, set new position for head
 *  Alt2: Move last seg to head. Set new positions for head
 */

static char *seg_to_str(seg_t *segment);

static void shift(snake_t *the_snake, int new_x_head, int new_y_head) {

    int snake_length = the_snake->length;

    // GENERATING ARRAY SIZE OF SNAKE LENGTH
    seg_t *snake_segments[the_snake->length];

    // ADDING EACH SNAKE SEGMENT TO SEGMENT ARRAY
    // IN REVERSE DIRECTION
    seg_t *segment = the_snake->last;
    for (int i = snake_length -1; i >= 0; i--) {
        snake_segments[i] = segment;
        segment = segment->prev;
    }

    int previous_x;
    int previous_y;

    int next_x;
    int next_y;

    // Loop over the snake updating the position of each segment
    for (int i = 0; i < snake_length ; i++) {
        // IF SEGMENT IS SNAKE HEAD
        if (i == 0) {
            previous_x = snake_segments[i]->x;
            previous_y = snake_segments[i]->y;

            snake_segments[i]->x = new_x_head;
            snake_segments[i]->y = new_y_head;
        }
        else {
            // ELSE SET NEXT POS TO PREVIOUS POS
            next_x = previous_x;
            next_y = previous_y;

            previous_x = snake_segments[i]->x;
            previous_y = snake_segments[i]->y;

            snake_segments[i]->x = next_x;
            snake_segments[i]->y = next_y;

        }
    }


}

void snake_move(snake_t *the_snake) {

    int new_x_head = the_snake->head->x;
    int new_y_head = the_snake->head->y;


    // CHANGING THE POSITION OF HEAD
    if (the_snake->dir == RIGHT) new_x_head = the_snake->head->x + the_snake->width;
    else if (the_snake->dir == LEFT) new_x_head = the_snake->head->x - the_snake->width;
    else if (the_snake->dir == DOWN) new_y_head = the_snake->head->y + the_snake->height;
    else if (the_snake->dir == UP) new_y_head= the_snake->head->y - the_snake->height;


    // SHIFTING THE SNAKE
    shift(the_snake, new_x_head, new_y_head);

}



void snake_turn(snake_t *the_snake, dir_t dir) {

    // CHANGE SNAKE DIRECTION
    // the_snake->dir = dir;

    // SETTING SO U CANNOT KILL YOURSELF
    // BY GOING BACKWARDS
    if (the_snake->length>2) {
        if (the_snake->dir == RIGHT && dir == LEFT){
            // DONT TURN
        } else if (the_snake->dir == LEFT && dir == RIGHT) {
            // DONT TURN
        }else if (the_snake->dir == UP && dir == DOWN){
            // DONT TURN
        } else if (the_snake->dir == DOWN && dir == UP) {
            // DONT TURN
        } else {
           the_snake->dir = dir; 
        }
    } else {
        the_snake->dir = dir;
    }

}

/*
 *  If length 2 snake may take opposite direction without colliding
 *  Else if length > 2 it will collide. Default length is 2.
 */
bool snake_hit_self(snake_t *the_snake) {

    //FROM REDUNTANT. PROB.
    bool there_is_segment = true;
    int length = 0;

    seg_t *segment = the_snake->last;

    seg_t *head = the_snake->head;


    bool hit_self = false;

    // LOOPING OVER SNAKE; EXCEPT FOR HEAD
    for (int i = 1; i < the_snake->length; i++) {
        // PROB REDUNTANT CODE.
        if (!segment) {
            there_is_segment = false;
            break;
        }
        // IF SEGMENT POSITION SAME AS SNAKE HEAD.
        if (segment->x == head->x && segment->y == head->y) {

            printf("SEG x: %d\n", segment->x);
            printf("HEAD x: %d\n", head->x);

            hit_self = true;
            break;
        }

        segment = segment->prev;

        if (length > 500) break;
        length++;
    }

    //return false;   // false for now
    return hit_self;
}



bool snake_hit_wall(snake_t *the_snake) {
   // TODO

    bool hit_wall = false;

    // SETTING LOWER X AND Y WALL
    int min_x = 0 - the_snake->width;
    int min_y = 0 - the_snake->height;

    //CHECKING IF SNAKE HEAD IT X WALL.
    if (the_snake->head->x == GAME_MAX_X || the_snake->head->x == min_x) {
        hit_wall = true;
    } 
    //CHECKING IF SNAKE HEAD IT Y WALL.
    else if (the_snake->head->y == GAME_MAX_Y || the_snake->head->y == min_y) {
        hit_wall = true;
    }

    return hit_wall;
}

bool snake_hit_apple(snake_t *the_snake, apple_t *the_apple) {
    return abs(the_snake->head->x - the_apple->x) < SEGMENT_WIDTH &&
           abs(the_snake->head->y - the_apple->y) < SEGMENT_HEIGHT;
}

// ------------ Apple function  --------------------------

apple_t *apple_new(snake_t *the_snake) {
    apple_t *a_apple = malloc(sizeof(apple_t));
    if (a_apple == NULL) {
        perror("Couldn't create apple, giving up ...");
        exit(EXIT_FAILURE);
    }

    // Make x and y coincide to grid
    int x = SEGMENT_WIDTH * (rand() % (GAME_MAX_X / 10));
    int y = SEGMENT_HEIGHT * (rand() % (GAME_MAX_Y / 10));

    int snake_intersect = apple_snake_intersect(the_snake, x, y); // Check if Apple location hits Snake location

    int max = 0;
    while (snake_intersect) { // If intersect, create new position until there is no intersect
        x = SEGMENT_WIDTH * (rand() % (GAME_MAX_X / 10)); 
        y = SEGMENT_HEIGHT * (rand() % (GAME_MAX_Y / 10));

        snake_intersect = apple_snake_intersect(the_snake, x, y); // Check if Apple location hits Snake location

        max++;
        if (max > 10000) break; // Redundancy
    }

    a_apple->x = x;
    a_apple->y = y;
    a_apple->width = SEGMENT_WIDTH;
    a_apple->height = SEGMENT_HEIGHT;

    return a_apple;
}

void apple_destroy(apple_t *the_apple) {
    free(the_apple);
}

int apple_snake_intersect(snake_t *the_snake, int x, int y) {

    // SET SNAKE INTERSECT TO FALSE
    // int snake_intersect = 0;
    seg_t *snake_segment = the_snake->last;

    // CLIMB SNAKE TO SEE IF SEGMENT INTERSECT
    int max = 0;
    for (int i = 0; i < the_snake->length; i++) {

        if (snake_segment->x == x && snake_segment->y == y) return 1;

        if (snake_segment->prev) {
            snake_segment = snake_segment->prev;
        }
            
        max++;
        if (max > 100000) break; // Redundancy
    }

    return 0;

}


#define BUF_SIZE 32

static char *seg_to_str(seg_t *segment) {
    char buffer[BUF_SIZE];
    char x_str[12];
    sprintf(x_str, "%d", segment->x);
    char y_str[12];
    sprintf(y_str, "%d", segment->y);
    sprintf(buffer, "(%s, %s)", x_str, y_str);
    char *str = malloc(sizeof(char) * strlen(buffer) + 1);
    strcpy(str, buffer);
    return str;
}


static char *to_str(snake_t *the_snake) {
    char *seg_str[BUF_SIZE] = {0};
    int seg_str_index = 0;
    seg_t *pos = the_snake->last;
    while (pos != NULL) {
        seg_str[seg_str_index++] = seg_to_str(pos);
        pos = pos->prev;
    }
    char buffer[BUF_SIZE * BUF_SIZE] = {0};
    int i = seg_str_index - 1;
    strcat(buffer, "h");
    strcat(buffer, seg_str[i--]);
    strcat(buffer, " ");
    while (i > 0) {
        strcat(buffer, "s");
        strcat(buffer, seg_str[i--]);
        strcat(buffer, " ");
    }
    strcat(buffer, "t");
    strcat(buffer, seg_str[i]);
    char *str = malloc(sizeof(char) * strlen(buffer) + 1);
    strcpy(str, buffer);
    return str;
}



