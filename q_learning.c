
//#include "snake.h"

// gcc `pkg-config gtk+-3.0 --cflags` q_learning.c snake0.c main.c -o Q_test `pkg-config gtk+-3.0 --libs`

#include <stdlib.h>
#include <stdio.h>

#include <time.h>

#include "q_learning.h"

// typedef struct List {
//     int snake;
//     int apple;
//     int wall;
// } q_list;



//Get game grid
//Generate Q grid

int **generateNewGameGrid(snake_t *the_snake, apple_t* apple) {
    int size_grid_x = GAME_MAX_X / SEGMENT_WIDTH;
    int size_grid_y = GAME_MAX_Y / SEGMENT_HEIGHT;

    //size_grid_x += 2;
    //size_grid_y += 2;

    size_grid_x += 2;
    size_grid_y += 2;

    int **mtx = (int **) malloc(size_grid_x * sizeof(int *));
    //for (int i = 0; i < size_grid_x; i++) {
    //    mtx[i] = (int *) malloc(size_grid_y * sizeof(int));
    //}
    for (int i = 0; i < size_grid_x; i++) {
        mtx[i] = (int *) malloc(size_grid_y * sizeof(int));
        for (int j = 0; j < size_grid_y; j++) {
            mtx[i][j] = 0;
        }
    }

    add_snake_to_grid(mtx, size_grid_x, size_grid_y, the_snake, apple);

    return mtx;
}

void add_snake_to_grid(int **grid, int size_grid_x, int size_grid_y, snake_t *the_snake, apple_t *apple) {

    //size_grid_x -= 2;
    //size_grid_y -= 2;

    for (int i = 0; i < size_grid_x; i++) {
        grid[i][0] = WALL;
        grid[i][size_grid_y-1] = WALL;
    }
    for (int i = 0; i < size_grid_y; i++) {
        grid[0][i] = WALL;
        grid[size_grid_x-1][i] = WALL;
    }

    seg_t *snake_segment = the_snake->last;
    int max = 10000;
    for (int i = 0; i < the_snake->length; i++) {

        //grid[snake_segment->x/SEGMENT_WIDTH][snake_segment->y/SEGMENT_HEIGHT] = SNAKE;
        // ADD PADDING TO ACCOUNT FOR WALL
        grid[(snake_segment->x/SEGMENT_WIDTH)+1][(snake_segment->y/SEGMENT_HEIGHT)+1] = SNAKE;

        if (snake_segment->prev) {
            snake_segment = snake_segment->prev;
        }
        
        max++;
    }

    //grid[apple->x/SEGMENT_WIDTH][apple->y/SEGMENT_HEIGHT] = APPLE;
    // ADD PADDING TO ACCOUNT FOR WALL
    grid[(apple->x/SEGMENT_WIDTH)+1][(apple->y/SEGMENT_HEIGHT)+1] = APPLE;
}


double ***generateNewQGrid() {
    int size_grid_x = GAME_MAX_X / SEGMENT_WIDTH;
    int size_grid_y = GAME_MAX_Y / SEGMENT_HEIGHT;

    size_grid_x += 2;
    size_grid_y += 2;

    // Create 3-dimensional array with dimensions (size_grid_x, size_grid_y, 4)
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

void reset_Q_matrix(double ***Q, int size_grid_x, int size_grid_y) {
    for (int i = 0; i < size_grid_x; i++) {
        for (int j = 0; j < size_grid_y; j++) {
            for (int k = 0; k < 4; k++) {
                Q[i][j][k] = 0.0;
            }
        }
    }
}

dir_t generate_move0(int next_act) {
    //int* move = (int*) malloc(2 * sizeof(int));
    dir_t move;
    if (next_act == 0) {
        //move[0] = 0;
        //move[1] = 1;
        move = DOWN;
    } else if (next_act == 1) {
        //move[0] = 0;
        //move[1] = -1;
        move = UP;
    } else if (next_act == 2) {
        //move[0] = -1;
        //move[1] = 0;
        move = LEFT;
    } else if (next_act == 3) {
        //move[0] = 1;
        //move[1] = 0;
        move = RIGHT;
    }
    return move;
}

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

int* action_fcn(double* q_in, double epsilon, double wind) {


    //double epsilon = EPSILON;
    //double wind = WIND;

    //  for (int j = 0; j < 4; j++) {
    //     printf("Q: %f \n", q_in[j]);
    // }

    int* action_and_move = (int*) malloc(4 * sizeof(int));
    
    int next_act = 0;
    double start_eps = (double) rand() / (double) RAND_MAX;
    // printf("START EPS: %f \n ", start_eps);

    if (start_eps < epsilon) {
        next_act = rand() % 4;
        // printf("RANDOM ");
    } else {
        double max_val = q_in[0];
        int max_index = 0;
        for (int i = 1; i < 4; i++) {
            if (q_in[i] > max_val) {

                // printf("value %f is larger", q_in[i]);
                // printf("than %f \n", max_val);


                max_val = q_in[i];
                max_index = i;
            }
        }
        next_act = max_index;
    }

    // printf("NEXT ACT: %d \n", next_act);

    
    int *move = generate_move(next_act);
    
    double test_wind = (double) rand() / (double) RAND_MAX;
    if (test_wind <= wind) {
        int random_act = rand() % 4;
        move = generate_move(random_act);
    }

    

    action_and_move[0] = next_act;
    action_and_move[1] = move[0];
    action_and_move[2] = move[1];

    free_arr(move);

    return action_and_move;
}

void free_arr(int *arr) {
    free(arr);
}



void updateQGrid(double ***Q_grid, int **game_grid, snake_t *the_snake, apple_t *apple, int nr_of_games) {

    //int nr_of_games = 1000;
    learningQ(Q_grid, game_grid, nr_of_games, the_snake);
    printf("NR OF GAMES: %d \n", nr_of_games);
    //int move = create_move(Q_grid, the_snake);

    //printf("Line 0:\n");
    //printf("%f \n", Q_grid[the_snake->head->x / SEGMENT_WIDTH][the_snake->head->y / SEGMENT_HEIGHT][0]);
    //printf("%f \n", Q_grid[the_snake->head->x / SEGMENT_WIDTH][the_snake->head->y / SEGMENT_HEIGHT][1]);
    //printf("%f \n", Q_grid[the_snake->head->x / SEGMENT_WIDTH][the_snake->head->y / SEGMENT_HEIGHT][2]);
    //printf("%f \n", Q_grid[the_snake->head->x / SEGMENT_WIDTH][the_snake->head->y / SEGMENT_HEIGHT][3]);

    
    //for (int j = 0; j < 50; j++) {
        //for (int k = 0; k < 4; k++) {
            //printf("%f ", Q_grid[0][j][k]);
        //}
        //printf("\n");
    //}


}

void learningQ(double ***Q_grid, int **game_grid, int nr_of_games, snake_t *the_snake) {

    //# the reward associated with each obstacle
    //#reward_list = {'fire': -50 , 'cliff': -100, 'goal': 100 , 'start' : -1 , '-' : -1 }
    //# the status of the game: 1 meaning the game has ended, the agent has hit a terminal state
    //#status_list = {'fire': -1 , 'cliff': 1, 'goal': 1 , 'start' : -1 , '-' : -1 }

    //struct List reward_list = {-50, -100, 100}; // snake, wall, goal
    //struct List status_list = {1,1,1};

    int reward_list[] = {-1, -80, -40, 100}; //game, snake, wall, apple
    int status_list[] = {-1, 1, 1, 1}; // die, die, die

    int start_position[] = {the_snake->head->x, the_snake->head->y};

    // printf("NUM OF GAMES: %d \n", nr_of_games);

    //nr_of_games = 100;
    for (int i = 0; i < nr_of_games; i++) {
        //int x_pos = start_position[0];
        //int y_pos = start_position[1];

        int x_pos = the_snake->head->x / SEGMENT_WIDTH;
        int y_pos = the_snake->head->y / SEGMENT_HEIGHT;
        game_grid[x_pos+1][y_pos+1] = 0;

        //printf("GAME GRID: %d", game_grid[x_pos][y_pos]);

        int max_loop = 0;
        while (status_list[game_grid[x_pos+1][y_pos+1]] != 1) {
            

            //printf("GAME GRID: %d", game_grid[x_pos][y_pos]);

            //Find out what move to make using  
            //SET Q_IN
            //q_in = Q[x_val][y_val]


            //move, action = action_fcn(Q[x_val][y_val],epsilon,wind) <- PYTHON CODE
            int *action_move = action_fcn(Q_grid[x_pos+1][y_pos+1], EPSILON, WIND);
            int action = action_move[0];
            int move[2];
            move[0] = action_move[1];
            move[1] = action_move[2];

            // printf("ACTION: %d \n",action);
            // for (int j = 0; j < 4; j++) {
            //     printf("%f ", Q_grid[x_pos][y_pos][j]);
            // }
            // printf("\n");
            

            // SAVE POSITION
            int old_x = x_pos;
            int old_y = y_pos;

            // UPDATE POSITION
            x_pos = old_x + move[0];
            y_pos = old_y + move[1];

            // printf("new x %d \n", x_pos);
            // printf("new y %d \n", y_pos);

            // if (x_pos < 0) x_pos = 0;
            // if (y_pos < 0) y_pos = 0;

            // if (x_pos > 20) x_pos = 20;
            // if (y_pos > 20) y_pos = 20;

            // printf("X POS %d \n", x_pos);
            // printf("Y POS %d \n", y_pos);

            //printf("MOVE 0 %d \n", move[0]);
            //printf("MOVE 1 %d \n", move[1]);

            //printf("NEW X %d \n", x_pos);
            //printf("NEW Y %d \n", y_pos);

            free_arr(action_move);

       
            int status = status_list[game_grid[x_pos+1][y_pos+1]];
            
            // printf("STATuS %d \n", status);

            // printf("old X POS %d \n", old_x);
            // printf(" old Y POS %d \n", old_y);
                
            

            // printf("GAME GRID: %d \n", game_grid[x_pos][y_pos]);

            int reward = reward_list[game_grid[x_pos+1][y_pos+1]];
            
            // printf("REWARD %d \n", reward);
            // if (reward > 10) {
            //     printf("HIT GOAL %d \n", reward);
            //     printf("GAME %d \n", i);
            // }

      

            double old_q_value = Q_grid[old_x+1][old_y+1][action];
            // printf("OLD Q %f \n", old_q_value);

            int optimal_policy = reward;
            // printf("OPTIMLA POLICY: %d \n", optimal_policy);

            //if status != 1:optimal_policy = reward + (gamma * np.max(Q[x_val, y_val]))

            double max_q = -999999;
            for (int i = 0; i < 4; i++) {
                double q = Q_grid[x_pos+1][y_pos+1][i];
                if (q > max_q) {
                    max_q = q;
                }
            }

            if (status != 1) optimal_policy = reward + (GAMMA * max_q);
            

            //new_q_value = (1-alpha) * old_q_value + (alpha * optimal_policy)
            double new_q_value = (1-ALPHA) * old_q_value + (ALPHA * optimal_policy);
            // printf("NEW Q %f \n", new_q_value);

          

            Q_grid[old_x+1][old_y+1][action] = new_q_value;
            // printf("Q GRID: %f\n", Q_grid[old_x][old_y][action]);

            if (status == 1) break;

            // for (int j = 0; j < 4; j++) {
            //     printf("%f ", Q_grid[old_x][old_y][j]);
            // }
            // printf("\n");


            max_loop++;
            if (max_loop > 10000) break;

            if (i % 100 == 0) {
                //printf("LOOP %d:\n", max_loop);
            }

            //if (max_loop > 50) printf("Training 50:\n");
        }
        // printf("######## max loop event: %d \n", max_loop);

        // if (i % 100 == 0) printf("GAME %d:\n", i);
    }



}

int create_move(double ***q_grid, snake_t *the_snake) {
    int x_pos = the_snake->head->x / SEGMENT_WIDTH;
    int y_pos = the_snake->head->y /SEGMENT_HEIGHT;

    int *action_move = action_fcn(q_grid[x_pos+1][y_pos+1],0,0);
    int action = action_move[0];
    int move[2];
    move[0] = action_move[1];
    move[1] = action_move[2];

    int new_x_pos = x_pos + move[0];
    int new_y_pos = y_pos + move[1];

    printf("NEW X %d \n", new_x_pos);
    printf("NEW Y %d \n", new_y_pos);

    free_arr(action_move);

    if (move[0] == 1 && move[1] == 0) {
        printf("RIGHT\n");
        return RIGHT;
    } else if (move[0] == -1 && move[1] == 0) {
        printf("LEFT\n");
        return LEFT;
    } else if (move[0] == 0 && move[1] == 1) {
        printf("DOWN\n");
        return DOWN;
    } else if (move[0] == 0 && move[1] == -1) {
        printf("UP\n");
        return UP;
    } else {
        printf("Invalid move\n");
    }

    



    return 1;

}

int create_moves0(double ***q_grid, int **game_grid, snake_t *the_snake, apple_t *apple) {

    int status_list[] = {-1, 1, 1, 1};
    int reward_list[] = {-1, -50, -100, 100};

    int x_pos = the_snake->head->x / SEGMENT_WIDTH;
    int y_pos = the_snake->head->y /SEGMENT_HEIGHT;

    int max = 0;

    while (status_list[game_grid[x_pos][y_pos]] != 1) {

        int *action_move = action_fcn(q_grid[x_pos][y_pos],0,0);
        int action = action_move[0];
        int move[2];
        move[0] = action_move[1];
        move[1] = action_move[2];

        //int new_x_pos = x_pos + move[0];
        //int new_y_pos = y_pos + move[1];

        x_pos = x_pos + move[0];
        y_pos = y_pos + move[1];

        printf("NEW X %d \n", x_pos);
        printf("NEW Y %d \n", y_pos);

        printf("APPLE COORDINATES: \n");
        printf("X %d \n", apple->x);
        printf("Y %d \n", apple->y);

        if (reward_list[game_grid[x_pos][y_pos]] > 50) {
            printf("HIT JACKPOT n");
        }

        free_arr(action_move);

        if (move[0] == 1 && move[1] == 0) {
            printf("RIGHT\n");
          
        } else if (move[0] == -1 && move[1] == 0) {
            printf("LEFT\n");
           
        } else if (move[0] == 0 && move[1] == 1) {
            printf("DOWN\n");
           
        } else if (move[0] == 0 && move[1] == -1) {
            printf("UP\n");
            
        } else {
            printf("Invalid move\n");
        }



        max++;
        if (max > 100) {
            printf("HIT MAX n");
            break;
        }

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

void free_2d_matrix(double **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
