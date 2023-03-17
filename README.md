# SNAKE GAME

## Q LEARNING	

Snake game where the game is played by a Q learning agent.
The code for creating and training the Q learning agent is found in q_learning.c while the snake functionality is found in snake.c

The game uses GTK-3 to create the graphics

To compile the game run:
```
gcc `pkg-config gtk+-3.0 --cflags` snake.c main.c -o snake_game `pkg-config gtk+-3.0 --libs`
```

The speed of the snake and using Q learning can be edited in the main.c file:
```C
#define SPEED 40
#define Q_ACTIVE 1
```
If one desires to control the snake manually set Q_ACTIVE to 0

The Q learning algorithm can be altered by changing the following values in q_learning.h
```C
#define EPSILON 0.9
#define WIND 0
#define GAMMA 0.95
#define ALPHA 0.2
```
Or by altering the weights assigned to the obstacle/reward
```C
int reward_list[] = {1, -100, -10, 200}; //Game, Snake, Wall, Apple
```