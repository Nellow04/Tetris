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
#define COLOR_TEXT 				White

/* Structures */
typedef struct {
    int8_t x;
    int8_t y;
} Point;

typedef struct {
    Point blocks[4];
    uint16_t color;
} TetrominoDef;

typedef struct {
    int x;
    int y;
    int type; 
    int rotation; 
    Point blocks[4]; 
    uint16_t color;
} CurrentTetromino;

/* Variabili globali */
extern volatile uint32_t score;
extern volatile uint32_t high_score;
extern volatile uint32_t lines_cleared;
extern volatile CurrentTetromino current_piece;
extern TetrominoDef tetrominocad_shape[7];

/* Game State */
typedef enum {
    GAME_PAUSED,
    GAME_RUNNING,
    GAME_OVER
} GameState;

extern volatile GameState game_state;

/* Prototipi delle funzioni */
void tetris_init();
void draw_grid();
void update_score();
void toggle_pause();
void spawn_tetromino();
void draw_tetromino();
void set_random_seed(int seed);

#endif