#include "tetris.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include <stdlib.h>

/* Variabili globali */
volatile uint32_t score = 0;
volatile uint32_t high_score = 0;
volatile uint32_t lines_cleared = 0;
volatile GameState game_state = GAME_PAUSED;
volatile CurrentTetromino current_piece;
int first_start = 1;

TetrominoDef tetrominocad_shape[7] = {
    // I
    {{{0,1}, {1,1}, {2,1}, {3,1}}, Cyan},
    // J
    {{{0,0}, {0,1}, {1,1}, {2,1}}, Blue},
    // L
    {{{2,0}, {0,1}, {1,1}, {2,1}}, Red},
    // O
    {{{1,0}, {2,0}, {1,1}, {2,1}}, Yellow},
    // S
    {{{1,0}, {2,0}, {0,1}, {1,1}}, Green},
    // T
    {{{1,0}, {0,1}, {1,1}, {2,1}}, Magenta},
    // Z
    {{{0,0}, {1,0}, {1,1}, {2,1}}, Red}
};

void tetris_init() {
    LCD_Clear(COLOR_BACKGROUND);
    draw_grid();
    update_score();
   
    game_state = GAME_PAUSED;
    first_start = 1;
    GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"PAUSED", Red, COLOR_BACKGROUND);
}

void set_random_seed(int seed) {
    srand(seed);
}

void toggle_pause() {
    if (game_state == GAME_RUNNING) {
        game_state = GAME_PAUSED;
				GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"        ", COLOR_BACKGROUND, COLOR_BACKGROUND); // Clear text
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"PAUSED", Red, COLOR_BACKGROUND);
    } else if (game_state == GAME_PAUSED) {
        game_state = GAME_RUNNING;
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"        ", COLOR_BACKGROUND, COLOR_BACKGROUND); // Clear text
        GUI_Text(FIELD_WIDTH + 5, 260, (uint8_t *)"PLAYING", Green, COLOR_BACKGROUND);
        
        if (first_start) {
            spawn_tetromino();
            draw_tetromino();
            first_start = 0;
        }
    }
}

void spawn_tetromino() {
    int type = rand() % 7;
    int i;
    
    current_piece.type = type;
    current_piece.rotation = 0;
    current_piece.x = (TETRIS_COLS / 2) - 2; 
    current_piece.y = 0;
    current_piece.color = tetrominocad_shape[type].color;
    
    for(i=0; i<4; i++) {
        current_piece.blocks[i] = tetrominocad_shape[type].blocks[i];
    }
}

void draw_tetromino() {
    int i, r;
    for(i=0; i<4; i++) {
        int x = (current_piece.x + current_piece.blocks[i].x) * BLOCK_SIZE;
        int y = (current_piece.y + current_piece.blocks[i].y) * BLOCK_SIZE;
        
        for(r=0; r<BLOCK_SIZE; r++) {
            LCD_DrawLine(x, y+r, x+BLOCK_SIZE-1, y+r, current_piece.color);
        }
        
        // Optional: Draw border for block
        LCD_DrawLine(x, y, x+BLOCK_SIZE-1, y, Black);
        LCD_DrawLine(x, y+BLOCK_SIZE-1, x+BLOCK_SIZE-1, y+BLOCK_SIZE-1, Black);
        LCD_DrawLine(x, y, x, y+BLOCK_SIZE-1, Black);
        LCD_DrawLine(x+BLOCK_SIZE-1, y, x+BLOCK_SIZE-1, y+BLOCK_SIZE-1, Black);
    }
}

void draw_grid() {
    int i;
    
    /* Creazione dei bordi */
    for (i = 0; i < 3; i++) {
        /* Bordo sinistro*/
        LCD_DrawLine(i, 0, i, FIELD_HEIGHT, White);
        /* Bordo destro*/
        LCD_DrawLine(FIELD_WIDTH + i, 0, FIELD_WIDTH + i, FIELD_HEIGHT, White);
        /* Bordo superiore */
        LCD_DrawLine(0, i, FIELD_WIDTH, i, White);
        /* Bordo inferiore */
        LCD_DrawLine(0, FIELD_HEIGHT - 1 - i, FIELD_WIDTH, FIELD_HEIGHT - 1 - i, White);
    }

    /* Creazione dei labels */
    GUI_Text(FIELD_WIDTH + 5, 20, (uint8_t *)"Score:", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + 5, 100, (uint8_t *)"High", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + 5, 120, (uint8_t *)"Score:", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + 5, 200, (uint8_t *)"Lines:", COLOR_TEXT, COLOR_BACKGROUND);
}

void update_score() {
    char buffer[20];
    
    /* Score */
    sprintf(buffer, "%d", score);
    GUI_Text(FIELD_WIDTH + 5, 40, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
    
    /* High Score */
    sprintf(buffer, "%d", high_score);
    GUI_Text(FIELD_WIDTH + 5, 140, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
    
    /* Lines Cleared */
    sprintf(buffer, "%d", lines_cleared);
    GUI_Text(FIELD_WIDTH + 5, 220, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
}
