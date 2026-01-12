#ifndef __MUSIC_H
#define __MUSIC_H

#include "LPC17xx.h"

// Note Frequencies (Hz)
#define NOTE_PAUSE 0
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  987
#define NOTE_C6  1046

typedef struct {
    uint16_t freq;
    uint16_t duration; // In 20ms ticks
} Note;

void music_init(void);
void music_update_tick(void); // Called every 20ms (Timer 2)
void music_handle_timer_irq(void); // Handle Timer 3 Interrupt

// SFX
void play_effect_rotate(void);
void play_effect_move(void);
void play_effect_line_clear(void);
void play_effect_game_over(void);

#endif
