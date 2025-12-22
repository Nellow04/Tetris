#ifndef __TETRIS_H		// Controlla se nel progetto esiste gi� una macro con questo nome
#define __TETRIS_H		// Se non esiste, la definisce

#include "LPC17xx.h"

/* Costanti */
#define TETRIS_ROWS 20
#define TETRIS_COLS 10
#define BLOCK_SIZE 	16

/* Layout schermo */
#define FIELD_WIDTH 	(TETRIS_COLS * BLOCK_SIZE)
#define FIELD_HEIGHT 	(TETRIS_ROWS * BLOCK_SIZE)

/* Colori */
#define COLOR_BACKGROUND 	Black
#define COLOR_GRID_LINE 	Grey
#define COLOR_TEXT 			White
#define COLOR_ORANGE        0xFD20

/* Variabili globali */
extern volatile uint32_t score;
extern volatile uint32_t high_score;
extern volatile uint32_t lines_cleared;

/* Stato del pezzo corrente */
extern volatile int current_x;
extern volatile int current_y;
extern volatile int current_type;
extern volatile int current_rotation;

/* Game State */
typedef enum {
    GAME_PAUSED,
    GAME_RUNNING,
    GAME_OVER
} GameState;

extern volatile GameState game_state;

/* Prototipi delle funzioni */
void tetris_init(void);
void draw_grid(void);
void update_score(void);
void toggle_pause(void);
void set_random_seed(int seed);

/* Fisica */
void handle_timer_tick(void);
void move_left(void);
void move_right(void);
void rotate_piece(void);

#endif