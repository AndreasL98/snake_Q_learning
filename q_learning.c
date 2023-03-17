
//#include "snake.h"

// gcc `pkg-config gtk+-3.0 --cflags` q_learning.c snake.c main.c -o Q_test `pkg-config gtk+-3.0 --libs`

#include <stdlib.h>
#include <stdio.h>

#include <time.h>

#include "q_learning.h"



int **generateNewGameGrid(snake_t *the_snake, apple_t* apple) {
    int size_grid_x = GAME_MAX_X / SEGMENT_WIDTH;
    int size_grid_y = GAME_MAX_Y / SEGMENT_HEIGHT;

    // Add padding to account for walls
    size_grid_x += 2;
    size_grid_y += 2;

    // Create a 2D Matrix to represent the game grid
    int **mtx = (int **) malloc(size_grid_x * sizeof(int *));
    for (int i = 0; i < size_grid_x; i++) {
        mtx[i] = (int *) malloc(size_grid_y * sizeof(int));
        for (int j = 0; j < size_grid_y; j++) {
            mtx[i][j] = 0;
        }
    }

    // Add the snake and apple to the 2D Matrix
    add_snake_to_grid(mtx, size_grid_x, size_grid_y, the_snake, apple);

    return mtx;
}

void add_snake_to_grid(int **grid, int size_grid_x, int size_grid_y, snake_t *the_snake, apple_t *apple) {

    // Add the walls to the 2D game matrix
    for (int i = 0; i < size_grid_x; i++) {
        grid[i][0] = WALL;
        grid[i][size_grid_y-1] = WALL;
    }
    for (int i = 0; i < size_grid_y; i++) {
        grid[0][i] = WALL;
        grid[size_grid_x-1][i] = WALL;
    }



    seg_t *snake_segment = the_snake->last; // Initiate a variable for snake segments
    int max = 100000;
    for (int i = 0; i < the_snake->length; i++) { // Loop to climb the snake segment by segment

        // Add snake segment position to the game grid
        // ADD PADDING TO ACCOUNT FOR WALL
        grid[(snake_segment->x/SEGMENT_WIDTH)+1][(snake_segment->y/SEGMENT_HEIGHT)+1] = SNAKE;

        if (snake_segment->prev) {
            snake_segment = snake_segment->prev; // Switch to next segment
        }
        
        max++;
        if (i > max) break;
    }

    // Add the apple to the game grid
    // ADD PADDING TO ACCOUNT FOR WALL
    grid[(apple->x/SEGMENT_WIDTH)+1][(apple->y/SEGMENT_HEIGHT)+1] = APPLE;
}


double ***generateNewQGrid() {
    int size_grid_x = GAME_MAX_X / SEGMENT_WIDTH;
    int size_grid_y = GAME_MAX_Y / SEGMENT_HEIGHT;

    // Add padding to account for walls
    size_grid_x += 2;
    size_grid_y += 2;

    // Create 3-dimensional array for Q learning
    double ***Q = (double***)malloc(size_grid_x * sizeof(double**));
    for (int i = 0; i < size_grid_x; i++) {
        Q[i] = (double**)malloc(size_grid_y * sizeof(double*));
        for (int j = 0; j < size_grid_y; j++) {
            Q[i][j] = (double*)malloc(4 * sizeof(double));
            for (int k = 0; k < 4; k++) {
                Q[i][j][k] = 0.0;
            }
        }
    }

    return Q;

}

// Function to reset the Q matrix after each hit apple 
//so old values will not interfere with reward
void reset_Q_matrix(double ***Q, int size_grid_x, int size_grid_y) {
    for (int i = 0; i < size_grid_x; i++) {
        for (int j = 0; j < size_grid_y; j++) {
            for (int k = 0; k < 4; k++) {
                Q[i][j][k] = 0.0;
            }
        }
    }
}

// Generate a move depending on the best action by Q learning agent
// move[0] represents x axis and move[1] represents y axis
int* generate_move(int next_act) {
    int* move = (int*) malloc(2 * sizeof(int));
    if (next_act == 0) {
        move[0] = 0;
        move[1] = 1;
    } else if (next_act == 1) {
        move[0] = 0;
        move[1] = -1;
    } else if (next_act == 2) {
        move[0] = -1;
        move[1] = 0;
    } else if (next_act == 3) {
        move[0] = 1;
        move[1] = 0;
    }
    return move;
}

// Action function to determine best action for Q agent
int* action_fcn(double* q_in, double epsilon, double wind) {

    int* action_and_move = (int*) malloc(4 * sizeof(int)); // initialize array for return
    
    int next_act = 0;
    double start_eps = (double) rand() / (double) RAND_MAX;

    
    // Epsilon sets if best action or random walk should be takes (random is recommended for learning)
    if (start_eps < epsilon) { // Generate random action
        next_act = rand() % 4; 
    } else { // Select best action as set by the Q matrix
        double max_val = q_in[0];
        int max_index = 0;
        for (int i = 1; i < 4; i++) {
            if (q_in[i] > max_val) {
                max_val = q_in[i];
                max_index = i;
            }
        }
        next_act = max_index;
    }

    
    int *move = generate_move(next_act); // Get the move array, where move[0] = x axis and move[1] = y axis
    
    double test_wind = (double) rand() / (double) RAND_MAX; // Check for wind
    if (test_wind <= wind) {
        int random_act = rand() % 4; // If wind generate random action
        move = generate_move(random_act);
    }

    

    // Array to return
    action_and_move[0] = next_act;
    action_and_move[1] = move[0];
    action_and_move[2] = move[1];

    // Free move array after use
    free_arr(move);

    return action_and_move;
}

// Function to free malloc array
void free_arr(int *arr) {
    free(arr);
}


// Function to update Q grid, simply calls learningQ.
void updateQGrid(double ***Q_grid, int **game_grid, snake_t *the_snake, int nr_of_games) {

    learningQ(Q_grid, game_grid, nr_of_games, the_snake);

}

void learningQ(double ***Q_grid, int **game_grid, int nr_of_games, snake_t *the_snake) {

    // the asgent reward associated with each obstacle
    int reward_list[] = {1, -100, -10, 200}; //Game, Snake, Wall, Apple

    // the status of the game
    int status_list[] = {-1, 1, 1, 1}; // -1 = non terminal, 1 = terminal


    //nr_of_games = 100;
    for (int i = 0; i < nr_of_games; i++) {

        int x_pos = the_snake->head->x / SEGMENT_WIDTH; // Set start position for the agent, i.e. the start position of the snake
        int y_pos = the_snake->head->y / SEGMENT_HEIGHT;
        game_grid[x_pos+1][y_pos+1] = 0;


        int max_loop = 0;
        while (status_list[game_grid[x_pos+1][y_pos+1]] != 1) {

         
            // Get next action and move for the agent
            int *action_move = action_fcn(Q_grid[x_pos+1][y_pos+1], EPSILON, WIND);
            int action = action_move[0]; // set the action
            int move[2]; // initialize array to store the move
            move[0] = action_move[1];
            move[1] = action_move[2];
            

            // SAVE X and Y POSITION
            int old_x = x_pos;
            int old_y = y_pos;

            // UPDATE POSITION
            x_pos = old_x + move[0];
            y_pos = old_y + move[1];

            free_arr(action_move); // Free the array returned from action_fcn

            // Get the status regarding the new position
            int status = status_list[game_grid[x_pos+1][y_pos+1]]; // +1 to account for wall padding 
                
            // Get the reward for the new position
            int reward = reward_list[game_grid[x_pos+1][y_pos+1]];
            
            // Get the Q value for the previous position
            double old_q_value = Q_grid[old_x+1][old_y+1][action];

            // The optimal police is the reward
            int optimal_policy = reward;

            // Get the maximum value Q value for the current position
            double max_q = -999999;
            for (int i = 0; i < 4; i++) {
                double q = Q_grid[x_pos+1][y_pos+1][i];
                if (q > max_q) {
                    max_q = q;
                }
            }

            // If current position is not terminal, set optimal policy as a function of reward, gamma and Q value for current position
            if (status != 1) optimal_policy = reward + (GAMMA * max_q);
            

            // Gey the new Q value as a function of alpha, the old Q value and the optimal policy
            double new_q_value = (1-ALPHA) * old_q_value + (ALPHA * optimal_policy);

          
            // Update the Q grid about the new Q value.
            Q_grid[old_x+1][old_y+1][action] = new_q_value;

            // Redundancy
            if (status == 1) break;


            max_loop++;
            if (max_loop > 1000000) break; // Redundancy, max 1 000 000 loops

        }

    }

}

// Function to create move and control the snake
int create_move(double ***q_grid, snake_t *the_snake) {
    int x_pos = the_snake->head->x / SEGMENT_WIDTH;
    int y_pos = the_snake->head->y /SEGMENT_HEIGHT;

    int *action_move = action_fcn(q_grid[x_pos+1][y_pos+1],0,0); // Action function with 0 epsilon and 0 wind for best possible action
    int action = action_move[0];
    int move[2];
    move[0] = action_move[1];
    move[1] = action_move[2];

    if (DEBUG_Q) {

        // Set new position as determined by the optimal move
        int new_x_pos = x_pos + move[0];
        int new_y_pos = y_pos + move[1];

        printf("NEW X %d \n", new_x_pos);
        printf("NEW Y %d \n", new_y_pos);
    }

    free_arr(action_move); // free the action_fcn array

    // Return the new move for the snake as determined by the Q learning matrix
    if (move[0] == 1 && move[1] == 0) {
        if (DEBUG_Q) printf("RIGHT\n");
        return RIGHT;
    } else if (move[0] == -1 && move[1] == 0) {
        if (DEBUG_Q) printf("LEFT\n");
        return LEFT;
    } else if (move[0] == 0 && move[1] == 1) {
        if (DEBUG_Q) printf("DOWN\n");
        return DOWN;
    } else if (move[0] == 0 && move[1] == -1) {
        if (DEBUG_Q) printf("UP\n");
        return UP;
    } else {
        printf("Invalid Move\n");
    }

    return 1;

}





void free_3D_matrix(double ***mtx, int size_grid_x, int size_grid_y) {
    int i, j;
    for (i = 0; i < size_grid_x; i++) {
        for (j = 0; j < size_grid_y; j++) {
            free(mtx[i][j]);
        }
        free(mtx[i]);
    }
    free(mtx);
}

void free_2d_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void print_2D_matrix(int **game_grid, int size_grid_x, int size_grid_y) {
    
    for(int i = 0; i < size_grid_x; i++) {
        for(int j = 0; j < size_grid_y; j++) {
            printf("%d ", game_grid[i][j]);
        }
        printf("\n");
    }
}
