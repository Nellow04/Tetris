#include "LPC17xx.h"

// Dimensioni della griglia e dei blocchi
#define TETRIS_ROWS 20
#define TETRIS_COLS 10
#define BLOCK_SIZE 	16

// Layout dello schermo
#define FIELD_WIDTH 	(TETRIS_COLS * BLOCK_SIZE)
#define FIELD_HEIGHT 	(TETRIS_ROWS * BLOCK_SIZE)

// Colori
#define COLOR_BACKGROUND 	Black
#define COLOR_GRID_LINE 	Grey
#define COLOR_TEXT 				White
#define Orange        		0xFD20
#define COLOR_POWERUP_HALF 0xCAE4       // Marrone
#define COLOR_POWERUP_SLOW 0xA145       // Oro

// Tipi di blocchi nella board
#define BLOCK_EMPTY         0
#define BLOCK_NORMAL        1
#define POWERUP_HALF_LINES  2
#define POWERUP_SLOW_DOWN   3

// Variabili globali
extern volatile uint32_t score;
extern volatile uint32_t high_score;
extern volatile uint32_t lines_cleared;

// Stato del tetromino corrente
extern volatile int current_x;
extern volatile int current_y;
extern volatile int current_type;
extern volatile int current_rotation;

// Stato del gioco
typedef enum {
    GAME_PAUSED,
    GAME_RUNNING,
    GAME_OVER
} GameState;

extern volatile GameState game_state;

// Prototipi delle funzioni
void tetris_init(void);						// Inizializza il gioco
void draw_grid(void);							// Disegna la griglia
void update_score(void);					// Aggiorna il risultato
void toggle_pause(void);					// Alterna lo stato di pausa del gioco
void set_random_seed(int seed);		// Imposta un seed per la generazione casuale dei tetromini
void draw_board(void);						// Disegna la board virtuale dove sono salvati i tetromini
void handle_timer_tick(void);			// Gestisce la fisica di caduta dei tetromini
void move_left(void);							// Muove il tetromino a sinsitra
void move_right(void);						// Muove il tetromino a destra
void rotate_piece(void);					// Ruota il tetromino di 90° a destra
void hard_drop(void);							// Piazza istantaneamente il tetromino ove possibile
