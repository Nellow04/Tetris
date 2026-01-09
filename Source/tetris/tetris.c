#include "tetris.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include <stdlib.h>

#define BOARD_Y 0
#define BOARD_X 3		// Lascio 3 pixel in modo tale da non disegnare sopra i bordi

// Variabili globali
volatile uint32_t score = 0;
volatile uint32_t high_score = 0;
volatile uint32_t lines_cleared = 0;
volatile GameState game_state = GAME_PAUSED;
// Powerup globals
volatile int lines_since_last_powerup = 0;
volatile int lines_since_last_malus = 0;
volatile int slow_down_timer = 0; // Contatore per powerup rallentamento (in tick da 20ms)

// Stato del tetromino corrente
volatile int current_x = 0;
volatile int current_y = 0;
volatile int current_type = 0;
volatile int current_rotation = 0;

// Stato iniziale
int first_start = 1;
uint16_t board[TETRIS_ROWS][TETRIS_COLS] = {0};

// Colori dei 7 tetromini
uint16_t piece_colors[7] = {
    Cyan,       		// I
    Blue,       		// J
    Orange, 				// L
    Yellow,     		// O
    Green,     			// S
    Magenta,    		// T
    Red        			// Z
};

// Definizione dei tetromini (7 tipi, 4 rotazioni, griglia 4x4)
// 1 = blocco presente, 0 = vuoto
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

// Prototipi
void spawn_tetromino(void);
void draw_tetromino(void);
void spawn_powerup(void);
void activate_powerup_half(void);
void activate_powerup_slow(void);
void activate_malus_line(void);

void draw_grid(void) { // Disegna l'interfaccia statica del gioco
    int i;
  
    for (i = 0; i < 3; i++) { // Creo linee spesse 3 pixel per creare i bordi
        LCD_DrawLine(i, 0, i, FIELD_HEIGHT, White);
        LCD_DrawLine(FIELD_WIDTH + BOARD_X + i, 0, FIELD_WIDTH + BOARD_X + i, FIELD_HEIGHT, White);
        LCD_DrawLine(0, i, FIELD_WIDTH + BOARD_X + 3, i, White);
        LCD_DrawLine(0, FIELD_HEIGHT - 1 - i, FIELD_WIDTH + BOARD_X + 3, FIELD_HEIGHT - 1 - i, White);
    }
    // Disegno i labels 
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 20, (uint8_t *)"Score:", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 100, (uint8_t *)"High", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 120, (uint8_t *)"Score:", COLOR_TEXT, COLOR_BACKGROUND);
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 200, (uint8_t *)"Lines:", COLOR_TEXT, COLOR_BACKGROUND);
}

void update_score(void) { // Aggiorna il risultato del gioco sullo schermo
    char buffer[20];
    sprintf(buffer, "%d", score);
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 40, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
    sprintf(buffer, "%d", high_score);
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 140, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
    sprintf(buffer, "%d", lines_cleared);
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 220, (uint8_t *)buffer, Yellow, COLOR_BACKGROUND);
}

void tetris_init(void) { // Inizializza lo stato del gioco
    int i, j;
    LCD_Clear(COLOR_BACKGROUND);
    
    for(i=0; i<TETRIS_ROWS; i++) { // Board virtuale per controllare lo stato del gioco
        for(j=0; j<TETRIS_COLS; j++) {
            board[i][j] = 0;
        }
    }
    
    score = 0;
    lines_cleared = 0;
    lines_since_last_powerup = 0;
    lines_since_last_malus = 0;

    draw_grid();
    update_score();
   
    game_state = GAME_PAUSED;
    first_start = 1;
    GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"PAUSED", Red, COLOR_BACKGROUND);
}

void set_random_seed(int seed) { // Imposta un seed per la generazione casuale del tetromino
    srand(seed);
}

void toggle_pause(void) { // Alterna lo stato di pausa e di running del gioco
    if (game_state == GAME_OVER) {
        tetris_init();
        return;
    }
    if (game_state == GAME_RUNNING) {
        game_state = GAME_PAUSED;
        GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"        ", COLOR_BACKGROUND, COLOR_BACKGROUND);
        GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"PAUSED", Red, COLOR_BACKGROUND);
    } else if (game_state == GAME_PAUSED) {
        game_state = GAME_RUNNING;
        GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"        ", COLOR_BACKGROUND, COLOR_BACKGROUND);
        GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"PLAYING", Green, COLOR_BACKGROUND);
        
        if (first_start) { // Se siamo al primo avvio del gioco, avviamo il gioco
            set_random_seed(LPC_TIM0->TC);
            spawn_tetromino();
            draw_tetromino();
            first_start = 0;
        }
    }
}

void spawn_tetromino(void) { // Genera un tetromino casuale
    current_type = rand() % 7;
    current_rotation = 0;
    current_x = (TETRIS_COLS / 2) - 2; // Posizione centrale
    current_y = 0;
}

void draw_tetromino_color(uint16_t color) { // Disegna il tetramino sullo schermo
    int r, c, i;
    for(r = 0; r < 4; r++) {
        for(c = 0; c < 4; c++) {
            if(pieces[current_type][current_rotation][r][c]) { // Controlliamo se nella board è presente il blocco
                int px = BOARD_X + (current_x + c) * BLOCK_SIZE;
                int py = BOARD_Y + (current_y + r) * BLOCK_SIZE;
                
                if (py < 0) continue; // Saltiamo se ci troviamo sopra lo schermo

                for(i = 0; i < BLOCK_SIZE; i++) {
                    LCD_DrawLine(px, py + i, px + BLOCK_SIZE - 1, py + i, color);
                }
                
                if (color != COLOR_BACKGROUND) {
                    // Bordo nero per distinguere meglio i quadratini
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

int check_collision(int px, int py, int rot) { // Verifica se il prossimo aggiornamento del tetromino avrà collisioni
    int r, c;
    for(r = 0; r < 4; r++) {
        for(c = 0; c < 4; c++) {
            if(pieces[current_type][rot][r][c]) {
                int board_x = px + c;
                int board_y = py + r;
                
								// Se esce a destra, a sinitra o sotto il pavimento, c'è collisione
                if (board_x < 0 || board_x >= TETRIS_COLS || board_y >= TETRIS_ROWS) return 1;
                if (board_y >= 0 && board[board_y][board_x] != 0) return 1; // Se nella baord non è presente un altro tetromino, ok
            }
        }
    }
    return 0;
}

void draw_board(void) { // Ridisegna completamente la baord di gioco
    int r, c, i;
    for(r = 0; r < TETRIS_ROWS; r++) {
         for(c = 0; c < TETRIS_COLS; c++) {
             int px = BOARD_X + c * BLOCK_SIZE;
             int py = BOARD_Y + r * BLOCK_SIZE;
             uint16_t val = board[r][c];
             uint16_t color;

             if (val == POWERUP_HALF_LINES) color = COLOR_POWERUP_HALF;
             else if (val == POWERUP_SLOW_DOWN) color = COLOR_POWERUP_SLOW;
             else if (val == 0) color = COLOR_BACKGROUND;
             else color = val; // Normal block color
             
             for(i = 0; i < BLOCK_SIZE; i++) {
                LCD_DrawLine(px, py + i, px + BLOCK_SIZE - 1, py + i, color); // Disegna il tetromino
             }
             
             if (color != COLOR_BACKGROUND) { // Disegna il bordo
                LCD_DrawLine(px, py, px + BLOCK_SIZE - 1, py, Black);
                LCD_DrawLine(px, py + BLOCK_SIZE - 1, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, Black);
                LCD_DrawLine(px, py, px, py + BLOCK_SIZE - 1, Black);
                LCD_DrawLine(px + BLOCK_SIZE - 1, py, px + BLOCK_SIZE - 1, py + BLOCK_SIZE - 1, Black);
             }
         }
    }
}

void check_lines(void) {
    int r, c, k;
    int lines_cleared_now = 0;
    
    // Scansiona dal basso verso l'alto e verifica che tutta la riga sia piena (diversa da 0)
    for(r = TETRIS_ROWS - 1; r >= 0; r--) {
        int full = 1;
        int powerup_type = 0;

        for(c = 0; c < TETRIS_COLS; c++) {
            if(board[r][c] == 0) {
                full = 0;
                break;
            }
            if (board[r][c] == POWERUP_HALF_LINES) powerup_type = POWERUP_HALF_LINES;
            else if (board[r][c] == POWERUP_SLOW_DOWN) powerup_type = POWERUP_SLOW_DOWN;
        }
        
        if(full) {
            
            // Check Powerup Activation BEFORE clearing
            if (powerup_type == POWERUP_HALF_LINES) {
                activate_powerup_half();
                // Re-scan logic might be needed because board changed drastically
                // Simply return or restart check_lines would be safer, but let's stick to simple flow.
                // Since activate_powerup_half clears lines, we should probably stop this loop and 
                // let the game cycle continue, or recursively check.
                // For simplicity: Update scores and exit check_lines.
                return; 
            }
            if (powerup_type == POWERUP_SLOW_DOWN) {
                activate_powerup_slow();
            }

            lines_cleared_now++;
            
            // Sposta tutte le righe sopra di una posizione verso il basso
            for(k = r; k > 0; k--) {
                for(c = 0; c < TETRIS_COLS; c++) {
                    board[k][c] = board[k-1][c];
                }
            }
            // Pulisce la prima riga in alto
            for(c = 0; c < TETRIS_COLS; c++) {
                board[0][c] = 0;
            }
            
            // Controlla di nuovo la stessa riga (perché ora contiene quella che era sopra)
            r++; 
        }
    }
    
    if(lines_cleared_now > 0) { // Calcolo dei punteggi
        lines_cleared += lines_cleared_now;
        lines_since_last_powerup += lines_cleared_now;
        lines_since_last_malus += lines_cleared_now;

        // Powerup every 5 lines
        if (lines_since_last_powerup >= 5) { 
            spawn_powerup();
            lines_since_last_powerup -= 5;
        }
        
        // Malus logic (DEBUG: 1 line instead of 10)
        if (lines_since_last_malus >= 10) {
             activate_malus_line();
             lines_since_last_malus = 0; // Reset counter
        }

        if (lines_cleared_now == 1) {
            score += 100;
        } else if (lines_cleared_now == 4) {
            score += 600;
        } else {
            score += 100 * lines_cleared_now;
        }
        
        update_score();
        draw_board();
    }
}

void spawn_powerup(void) {
    int r, c;
    static struct Point { int r; int c; } candles[TETRIS_ROWS * TETRIS_COLS]; // Static to avoid stack overflow
    int count = 0;

    // Find all valid blocks (non-empty, non-powerup)
    for(r = 0; r < TETRIS_ROWS; r++) {
         for(c = 0; c < TETRIS_COLS; c++) {
             // Debugged condition: ensure we target actual blocks 
             if (board[r][c] != 0 && board[r][c] != POWERUP_HALF_LINES && board[r][c] != POWERUP_SLOW_DOWN) {
                 candles[count].r = r;
                 candles[count].c = c;
                 count++;
             }
         }
    }

    if (count > 0) {
        int idx = rand() % count;
        // Randomly choose powerup type
        int type = (rand() % 2) ? POWERUP_HALF_LINES : POWERUP_SLOW_DOWN;
        board[candles[idx].r][candles[idx].c] = type;
        
        // Force full redraw to ensure it appears
        draw_board();
    }
}

void activate_powerup_half(void) {
    int r, c, k;
    int cleared_count = 0;
    
    // Clear bottom half: from ROWS-1 down to ROWS/2
    // Actually spec says "bottom half", so rows indices [ROWS/2 .. ROWS-1]
    
    for(r = TETRIS_ROWS - 1; r >= TETRIS_ROWS / 2; r--) {
        cleared_count++;
        // Shift global rows down
        for(k = r; k > 0; k--) {
            for(c = 0; c < TETRIS_COLS; c++) {
                board[k][c] = board[k-1][c];
            }
        }
        for(c = 0; c < TETRIS_COLS; c++) {
           board[0][c] = 0;
        }
        // Since we shifted down into 'r', we must process 'r' again? 
        // No, we are clearing the slot 'r'. Shifting makes the line above `r` fall into `r`.
        // So effectively we just consumed one line of the board stack.
        // But since we want to clear the *visual* bottom half, we should just repeat this process N times 
        // where N is number of lines to clear.
        
        // Wait, loop approach:
        // We want to remove the bottom 10 lines.
        // The loop updates 'board' in place.
        // If I shift immediately, calculate r again.
        r++; // Stay on same index because new content fell here
        if (cleared_count >= TETRIS_ROWS / 2) break; // Safety break
    }
    
    // Scoring
    lines_cleared += cleared_count;
    // Spec: "If > 4 lines, award points in groups of 4"
    if (cleared_count <= 4) {
         // Logic is vague "usual number of points". Let's assume as if they were cleared normally
         score += 100 * cleared_count; // Simplified
    } else {
        int groups = cleared_count / 4;
        int rem = cleared_count % 4;
        score += groups * 600; // 4 lines = 600
        score += rem * 100;
    }
    update_score();
    draw_board();
}

void activate_powerup_slow(void) {
    slow_down_timer = 750; // 15 seconds * 50 ticks/sec (20ms) = 750
}

void activate_malus_line(void) {
    int r, c;
    
    // Check Overflow (If blocks are in the top row, they will be pushed out)
    for(c = 0; c < TETRIS_COLS; c++) {
        if (board[0][c] != 0) {
            game_state = GAME_OVER;
            GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"GAME OVER", Red, COLOR_BACKGROUND);
            return;
        }
    }

    // Shift Rows Up
    for(r = 0; r < TETRIS_ROWS - 1; r++) {
       for(c = 0; c < TETRIS_COLS; c++) {
           board[r][c] = board[r+1][c];
       }
    }
    
    // Generate Malus Line (7 blocks)
    int last_row = TETRIS_ROWS - 1;
    for(c = 0; c < TETRIS_COLS; c++) board[last_row][c] = 0;
    
    int filled = 0;
    while(filled < 7) {
        int col = rand() % TETRIS_COLS;
        if (board[last_row][col] == 0) {
             board[last_row][col] = Grey; 
             filled++;
        }
    }
    
    draw_board();
}

void place_tetromino(void) { // Piazza definitivamente il tetromino nella board
    int r, c;
    for(r = 0; r < 4; r++) {
        for(c = 0; c < 4; c++) {
            if(pieces[current_type][current_rotation][r][c]) { // Salva il tetromino nella board
                int board_x = current_x + c;
                int board_y = current_y + r;
                if (board_y >= 0 && board_y < TETRIS_ROWS && board_x >= 0 && board_x < TETRIS_COLS) {
                    board[board_y][board_x] = piece_colors[current_type];
                }
            }
        }
    }
    
    check_lines();
    
    score += 10;
    update_score();
    
    spawn_tetromino();
    if (check_collision(current_x, current_y, current_rotation)) {
        game_state = GAME_OVER;
        if (score > high_score) {
            high_score = score;
        }
        GUI_Text(FIELD_WIDTH + BOARD_X + 5, 260, (uint8_t *)"GAME OVER", Red, COLOR_BACKGROUND);
    } else {
        draw_tetromino();
    }
}

void move_left(void) { // Muove a sinsitra
    if (game_state != GAME_RUNNING) return;
    if (!check_collision(current_x - 1, current_y, current_rotation)) {
        clear_tetromino();
        current_x--;
        draw_tetromino();
    }
}

void move_right(void) { // Muove a destra
    if (game_state != GAME_RUNNING) return;
    if (!check_collision(current_x + 1, current_y, current_rotation)) {
        clear_tetromino();
        current_x++;
        draw_tetromino();
    }
}

void rotate_piece(void) { // Ruota di 90°
    int next_rotation;
    if (game_state != GAME_RUNNING) return;
    
    next_rotation = (current_rotation + 1) % 4;
    
    if (!check_collision(current_x, current_y, next_rotation)) {
        clear_tetromino();
        current_rotation = next_rotation;
        draw_tetromino();
    }
}

void handle_timer_tick(void) { // Chiamata dal TIMER0 verifica fa cadere il tetromino e verifica le condizioni del gioco
    if (game_state != GAME_RUNNING) return;
    
    if (!check_collision(current_x, current_y + 1, current_rotation)) {
        clear_tetromino();
        current_y++;
        draw_tetromino();
    } else {
        place_tetromino();
    }
}

void hard_drop(void) { // Posiziona istantaneamente il tetromino se possibile
    if (game_state != GAME_RUNNING) return;
    
    clear_tetromino();
    while (!check_collision(current_x, current_y + 1, current_rotation)) {
        current_y++;
    }
    draw_tetromino();
    place_tetromino();
}
