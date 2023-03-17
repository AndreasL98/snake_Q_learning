    /*
 *     **** Nothing to do here ****
 *     (but you may experiment... at own risk)
 *
 *
 *     The GUI and keyboard handling for the Snake game
 *     Using GTK3, see https://developer.gnome.org/gtk3/stable/
 *
 *     See CMakeList.txt for dependencies of libraries and more
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "snake.h"
#include "q_learning.h"

#include <time.h>


#define SPEED 40 // Yes! ORG: 150

#define Q_ACTIVE 1

// gcc `pkg-config gtk+-3.0 --cflags` snake.c main.c -o out_main `pkg-config gtk+-3.0 --libs`

// Surface to draw the snake and the apples
static cairo_surface_t *surface = NULL;

static void draw_message(cairo_t *cr, char *msg, int x, int y, bool center); // Helper

// Pointers to global data
static snake_t *the_snake;
static apple_t *the_apple;

static double ***Q_mtx;

int new_game = 1;

//------ Initialization (called first of all) -------------

// Useful to have long snake for testing
static snake_t *get_snake(int length) {
    the_snake = snake_new(GAME_MAX_X / 2, GAME_MAX_Y / 2, SEGMENT_WIDTH, SEGMENT_HEIGHT);
    for (int i = 0; i < length; i++) {
        snake_append_segment(the_snake);
        snake_move(the_snake);
    }
    return the_snake;
}

static void init() {
    the_snake = get_snake(0);  // Will default have length 2
    the_apple = apple_new(the_snake);

    Q_mtx = generateNewQGrid();
}

//------ The game loop (called by a timer) -------------

static bool done = false;  // Used to run one last update to display text

static gboolean update(gpointer widget) {

    if (done) {
        return false;  // Stop timer
    }

    GtkWidget *drawing_area = widget;
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 0);   // Clear screen (snake segments)
    cairo_paint(cr);

    cairo_set_source_rgba(cr, 0, 0, 1, 1); // Color for segments

    int w = the_snake->width;
    int h = the_snake->height;
    int x;
    int y;
    // Draw from end
    seg_t *pos = the_snake->last;
    seg_t *head = the_snake->head;
    while (pos != head) {
        x = pos->x;
        y = pos->y;
        cairo_rectangle(cr, x, y, w, h);
        cairo_fill(cr);
        pos = pos->prev;
    }

    // Draw head
    cairo_set_source_rgba(cr, 1, 0, 0, 1); // Head should be RED
    x = head->x;
    y = head->y;
    cairo_rectangle(cr, x, y, w, h);  // Drawing is from top left corner
    cairo_fill(cr);


    // Draw apple
    cairo_set_source_rgba(cr, 0, 1, 0, 1);
    cairo_rectangle(cr, the_apple->x, the_apple->y, the_apple->width, the_apple->height);
    cairo_fill(cr);

    // Draw points
    char pts_str[16];
    sprintf(pts_str, "Points %d", the_snake->length - 2);
    draw_message(cr, pts_str, 5, 20, false);

    //Q LEARNING CODE

    int **game_grid = generateNewGameGrid(the_snake, the_apple); // CREATE NEW GAME GRID

    int size_grid_x = GAME_MAX_X / SEGMENT_WIDTH +2;
    int size_grid_y = GAME_MAX_Y / SEGMENT_HEIGHT +2;
    //reset_Q_matrix(Q_mtx, size_grid_x, size_grid_y);

    //print_2D_matrix(game_grid, size_grid_x, size_grid_y);

    int nr_of_games = 3000;

    // If new game, there is a new Q matrix and more games are required for agent learning
    if (new_game) {
        nr_of_games = 7000;
        new_game = 0;
        
    }
    updateQGrid(Q_mtx, game_grid, the_snake, nr_of_games); // LEARN Q GRID


    int rows = (GAME_MAX_X / SEGMENT_WIDTH) +2;
    free_2d_matrix(game_grid, rows);

    int move = create_move(Q_mtx, the_snake);
    if (Q_ACTIVE) {
        the_snake->dir = move;
    }

    // END OF Q LEARNING CODE

    // Move and check everything
    snake_move(the_snake);

    // sleep(10);

    if (snake_hit_apple(the_snake, the_apple)) {
        snake_append_segment(the_snake);
        apple_destroy(the_apple);
        the_apple = apple_new(the_snake);
        //the_apple->y = 0;

        int size_grid_x = GAME_MAX_X / SEGMENT_WIDTH +2;
        int size_grid_y = GAME_MAX_Y / SEGMENT_HEIGHT +2;
        reset_Q_matrix(Q_mtx, size_grid_x, size_grid_y);
        new_game = 1;

        // NEW Q LEARNING MATRIX
        // RETRAIN Q LEARNING MATRIX
    }
    if (snake_hit_wall(the_snake)) {
        //fprintf(stderr, "Hit wall");   // Debug
        draw_message(cr, "Ooouughht ... hit wall",
                     GAME_MAX_X / 2, GAME_MAX_Y / 2, true);
        done = true;
    }

    if (snake_hit_self(the_snake)) {
        //fprintf(stderr, "Hit self");   // Debug
        draw_message(cr, "Gnnrrrrr... hit myself",
                     GAME_MAX_X / 2, GAME_MAX_Y / 2, true);
        done = true;
    }

    gtk_widget_queue_draw(drawing_area); // Must have!! Why??
    cairo_destroy(cr);
    return true;
}

static void draw_message(cairo_t *cr, char *msg, int x, int y, bool center) {
    cairo_text_extents_t extents;
    cairo_select_font_face(cr, "Courier",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    cairo_set_source_rgb(cr, 1, 1, 1);
    if (center) {
    cairo_text_extents(cr, msg, &extents);
    cairo_move_to(cr, x - extents.width / 2, y);
    } else {
        cairo_move_to(cr, x, y);
    }
    cairo_show_text(cr, msg);
}

// ------------ Handling keyboard -------------------

static gboolean key_press_event_cb(GtkWidget *widget,
                                   GdkEventKey *event,
                                   gpointer data) {
    if (event->keyval == GDK_KEY_Left) {
        snake_turn(the_snake, LEFT);
    } else if (event->keyval == GDK_KEY_Right) {
        snake_turn(the_snake, RIGHT);
    } else if (event->keyval == GDK_KEY_Up) {
        snake_turn(the_snake, UP);
    } else if (event->keyval == GDK_KEY_Down) {
        snake_turn(the_snake, DOWN);
    } else {
        // Nothing (space bar to start)
    }

    // We've handled the event, stop processing
    return false;
}


// ------------  Graphical stuff --------------------

// Create a new surface of the appropriate size
static gboolean configure_event_cb(GtkWidget *widget,
                                   GdkEventConfigure *event,
                                   gpointer data) {
    if (surface) {
        cairo_surface_destroy(surface);
    }
    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                                CAIRO_CONTENT_COLOR,
                                                gtk_widget_get_allocated_width(widget),
                                                gtk_widget_get_allocated_height(widget));

    // We've handled the configure event, no need for further processing.
    return true;
}

/*
 * Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean draw_cb(GtkWidget *widget,
                        cairo_t *cr,
                        gpointer data) {
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);  // Must be there
    return false;
}

static void close_window(void) {
    if (surface != NULL)
        cairo_surface_destroy(surface);

}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), "SNAKE");

    g_signal_connect (window, "destroy", G_CALLBACK(close_window), NULL);

    gtk_container_set_border_width(GTK_CONTAINER (window), 8);

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER (window), frame);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, GAME_MAX_X, GAME_MAX_Y);

    gtk_container_add(GTK_CONTAINER (frame), drawing_area);

    // ACTIVATE Q GRID
   

    // Signals used to handle the backing surface
    g_signal_connect (drawing_area, "draw",
                      G_CALLBACK(draw_cb), NULL);
    g_signal_connect (drawing_area, "configure-event",
                      G_CALLBACK(configure_event_cb), NULL);
    g_signal_connect (window, "key-press-event",
                      G_CALLBACK(key_press_event_cb), NULL);
    gtk_widget_set_events(window, gtk_widget_get_events(window)
                                  | GDK_KEY_PRESS_MASK);
    gtk_widget_show_all(window);

    srand((unsigned) time(NULL));     // Used for the apple_new
    init();
    // Start timer to call the callback
    g_timeout_add(SPEED, update, drawing_area);
}

/*
 *     Application start
 */
int main(int argc, char **argv) {

    srand(time(NULL)); // Shold be in main???

    GtkApplication *app;
    int status;

    app = gtk_application_new("chl.hajo.snake", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    return status;
}