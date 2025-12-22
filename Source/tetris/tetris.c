#include "tetris.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include <stdlib.h>

/* Variabili globali */
volatile uint32_t score = 0;
volatile uint32_t high_score = 0;
volatile uint32_t lines_cleared = 0;
volatile GameState game_state = GAME_PAUSED;

/* Stato del pezzo corrente */
volatile int current_x = 0;
volatile int current_y = 0;
volatile int current_type = 0;
volatile int current_rotation = 0;

int first_start = 1;
uint16_t board[TETRIS_ROWS][TETRIS_COLS] = {0};

/* Colori dei 7 pezzi */
uint16_t piece_colors[7] = {
    Cyan,       // I
    Blue,       // J
    COLOR_ORANGE, // L
    Yellow,     // O
    Green,      // S
    Magenta,    // T
    Red         // Z
};

/* 
 * Definizione dei pezzi (7 tipi, 4 rotazioni, griglia 4x4)
 * 1 = blocco presente, 0 = vuoto
 */
const uint8_t pieces[7][4][4][4] = {
    // I
    {
        {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}}
    },
    // J
    {
        {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}
    },
    // L
    {
        {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}},
        {{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}
    },
    // O
    {
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}
    },
    // S
    {
        {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
        {{1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
    },
    // T
    {
        {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}
    },
    // Z
    {
        {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0}}
    }
};

/* Forward declarations */
void spawn_tetromino(void);
void draw_tetromino(void);
void clear_tetromino(void);
int check_collision(int px, int py, int rot);
void place_tetromino(void);

void draw_grid(void) {
    int i;
    /* Creazione dei bordi */
    for (i = 0; i < 3; i++) {
        LCD_DrawLine(i, 0, i, FIELD_HEIGHT, White);
        LCD_DrawLine(FIELD_WIDTH + i, 0, FIELD_WIDTH + i, FIELD_HEIGHT, White);
        LCD_DrawLine(0, i, FIELD_WIDTH, i, White);
        LCD_DrawLine(0, FIELD_HEIGHT - 1 - i, FIELD_WIDTH, FIELD_HEIGHT - 1 - i, White);
    }
    /* Labels */
    GUI_Text(FIELD_WIDTH + 5, 20, (uint8_t *)"Score:", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + 5, 100, (uint8_t *)"High", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + 5, 120, (uint8_t *)"Score:", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + 5, 200, (uint8_t *)"Lines:", COLOR_TEXT, COLOR_BACKGROUND);
}

void update_score(void) {
    char buffer[20];
    sprintf(buffer, "%d", score);
    GUI_Text(FIELD_WIDTH + 5, 40, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
    sprintf(buffer, "%d", high_score);
    GUI_Text(FIELD_WIDTH + 5, 140, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
    sprintf(buffer, "%d", lines_cleared);
    GUI_Text(FIELD_WIDTH + 5, 220, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
}

void tetris_init(void) {
    int i, j;
    LCD_Clear(COLOR_BACKGROUND);
    
    for(i=0; i<TETRIS_ROWS; i++) {
        for(j=0; j<TETRIS_COLS; j++) {
            board[i][j] = 0;
        }
    }

    draw_grid();
    update_score();
   
    game_state = GAME_PAUSED;
    first_start = 1;
    GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"PAUSED", Red, COLOR_BACKGROUND);
}

void set_random_seed(int seed) {
    srand(seed);
}

void toggle_pause(void) {
    if (game_state == GAME_RUNNING) {
        game_state = GAME_PAUSED;
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"        ", COLOR_BACKGROUND, COLOR_BACKGROUND);
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"PAUSED", Red, COLOR_BACKGROUND);
    } else if (game_state == GAME_PAUSED) {
        game_state = GAME_RUNNING;
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"        ", COLOR_BACKGROUND, COLOR_BACKGROUND);
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"PLAYING", Green, COLOR_BACKGROUND);
        
        if (first_start) {
            set_random_seed(LPC_TIM0->TC);
            spawn_tetromino();
            draw_tetromino();
            first_start = 0;
        }
    }
}

void spawn_tetromino(void) {
    current_type = rand() % 7;
    current_rotation = 0;
    current_x = (TETRIS_COLS / 2) - 2; 
    current_y = 0;
}

void draw_tetromino_color(uint16_t color) {
    int r, c, i;
    for(r = 0; r < 4; r++) {
        for(c = 0; c < 4; c++) {
            if(pieces[current_type][current_rotation][r][c]) {
                int px = (current_x + c) * BLOCK_SIZE;
                int py = (current_y + r) * BLOCK_SIZE;
                
                if (py < 0) continue; // Skip if above screen

                for(i = 0; i < BLOCK_SIZE; i++) {
                    LCD_DrawLine(px, py + i, px + BLOCK_SIZE - 1, py + i, color);
                }
                
                if (color != COLOR_BACKGROUND) {
                    // Draw border
                    LCD_DrawLine(px, py, px + BLOCK_SIZE - 1, py, Black);
                    LCD_DrawLine(px, py + BLOCK_SIZE - 1, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, Black);
                    LCD_DrawLine(px, py, px, py + BLOCK_SIZE - 1, Black);
                    LCD_DrawLine(px + BLOCK_SIZE - 1, py, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, Black);
                }
            }
        }
    }
}

void draw_tetromino(void) {
    draw_tetromino_color(piece_colors[current_type]);
}

void clear_tetromino(void) {
    draw_tetromino_color(COLOR_BACKGROUND);
}

int check_collision(int px, int py, int rot) {
    int r, c;
    for(r = 0; r < 4; r++) {
        for(c = 0; c < 4; c++) {
            if(pieces[current_type][rot][r][c]) {
                int board_x = px + c;
                int board_y = py + r;
                
                if (board_x < 0 || board_x >= TETRIS_COLS || board_y >= TETRIS_ROWS) return 1;
                if (board_y >= 0 && board[board_y][board_x] != 0) return 1;
            }
        }
    }
    return 0;
}

void place_tetromino(void) {
    int r, c;
    for(r = 0; r < 4; r++) {
        for(c = 0; c < 4; c++) {
            if(pieces[current_type][current_rotation][r][c]) {
                int board_x = current_x + c;
                int board_y = current_y + r;
                if (board_y >= 0 && board_y < TETRIS_ROWS && board_x >= 0 && board_x < TETRIS_COLS) {
                    board[board_y][board_x] = piece_colors[current_type];
                }
            }
        }
    }
    
    spawn_tetromino();
    if (check_collision(current_x, current_y, current_rotation)) {
        game_state = GAME_OVER;
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"GAME OVER", Red, COLOR_BACKGROUND);
    } else {
        draw_tetromino();
    }
}

void move_left(void) {
    if (game_state != GAME_RUNNING) return;
    if (!check_collision(current_x - 1, current_y, current_rotation)) {
        clear_tetromino();
        current_x--;
        draw_tetromino();
    }
}

void move_right(void) {
    if (game_state != GAME_RUNNING) return;
    if (!check_collision(current_x + 1, current_y, current_rotation)) {
        clear_tetromino();
        current_x++;
        draw_tetromino();
    }
}

void rotate_piece(void) {
    int next_rotation;
    if (game_state != GAME_RUNNING) return;
    
    next_rotation = (current_rotation + 1) % 4;
    
    if (!check_collision(current_x, current_y, next_rotation)) {
        clear_tetromino();
        current_rotation = next_rotation;
        draw_tetromino();
    }
}

void handle_timer_tick(void) {
    if (game_state != GAME_RUNNING) return;
    
    if (!check_collision(current_x, current_y + 1, current_rotation)) {
        clear_tetromino();
        current_y++;
        draw_tetromino();
    } else {
        place_tetromino();
    }
}

void hard_drop(void) {
    if (game_state != GAME_RUNNING) return;
    
    clear_tetromino();
    while (!check_collision(current_x, current_y + 1, current_rotation)) {
        current_y++;
    }
    draw_tetromino();
    place_tetromino();
}
