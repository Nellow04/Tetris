#include "music.h"
#include "../timer/timer.h"

	// Tetris Tema A
static Note song_tetris[] = {
    {NOTE_E5, 20}, {NOTE_B4, 10}, {NOTE_C5, 10}, {NOTE_D5, 20}, {NOTE_C5, 10}, {NOTE_B4, 10}, 
    {NOTE_A4, 20}, {NOTE_A4, 10}, {NOTE_C5, 10}, {NOTE_E5, 20}, {NOTE_D5, 10}, {NOTE_C5, 10},
    {NOTE_B4, 20}, {NOTE_C5, 10}, {NOTE_D5, 20}, {NOTE_E5, 20},
    {NOTE_C5, 20}, {NOTE_A4, 20}, {NOTE_A4, 20}, {NOTE_PAUSE, 20}, // Pausa
    
    // Tema B
    {NOTE_D5, 20}, {NOTE_F5, 10}, {NOTE_A5, 20}, {NOTE_G5, 10}, {NOTE_F5, 10},
    {NOTE_E5, 30}, {NOTE_C5, 10}, {NOTE_E5, 20}, {NOTE_D5, 10}, {NOTE_C5, 10},
    {NOTE_B4, 20}, {NOTE_B4, 10}, {NOTE_C5, 10}, {NOTE_D5, 20}, {NOTE_E5, 20},
    {NOTE_C5, 20}, {NOTE_A4, 20}, {NOTE_A4, 20}, {NOTE_PAUSE, 20}
};

static int song_len = sizeof(song_tetris) / sizeof(Note);
static int current_note_index = 0;
static int current_note_ticks_remaining = 0;

// Stati SFX
static int sfx_active = 0;
static int sfx_ticks_remaining = 0;
static int sfx_freq = 0;

void setup_timer_music(int freq) {
    if (freq <= 0) {
        disable_timer(3);
        LPC_DAC->DACR = 0;
        return;
    }
    uint32_t mr0 = 25000000 / (2 * freq);
    LPC_TIM3->MR0 = mr0;
    LPC_TIM3->MCR = 3; // Interrupt & Reset on MR0
    LPC_TIM3->TCR = 2; // Reset
    enable_timer(3);
}

void music_init(void) {
    LPC_SC->PCONP |= (1 << 23); // Power up Timer 3 (Bit 23)
    LPC_SC->PCONP |= (1 << 12); // Power up ADC/DAC
    
    LPC_PINCON->PINSEL1 |= (1 << 21); 
    LPC_PINCON->PINSEL1 &= ~(1 << 20);
    
    init_timer(3, 0, 0, 3, 10000); 

    NVIC_EnableIRQ(TIMER3_IRQn);
    NVIC_SetPriority(TIMER3_IRQn, 0);

    current_note_index = 0;
    Note first = song_tetris[0];
    current_note_ticks_remaining = first.duration;
    setup_timer_music(first.freq);
}

void music_update_tick(void) {

    if (sfx_active) {
        if (sfx_ticks_remaining > 0) {
            sfx_ticks_remaining--;
            return; 
        } else {
            sfx_active = 0;
            Note next = song_tetris[current_note_index];
            setup_timer_music(next.freq);
        }
    }

    if (current_note_ticks_remaining > 0) {
        current_note_ticks_remaining--;
    } 
    
    if (current_note_ticks_remaining <= 0) {
        current_note_index++;
        if (current_note_index >= song_len) {
            current_note_index = 0;
        }
        
        Note next = song_tetris[current_note_index];
        current_note_ticks_remaining = next.duration;
        
        setup_timer_music(next.freq);
    }
}

void music_handle_timer_irq(void) {
    static int dac_state = 0;

    uint32_t volume = 0x080; 

    if (dac_state) {
        LPC_DAC->DACR = (0 << 6); // Min
        dac_state = 0;
    } else {
        LPC_DAC->DACR = (volume << 6); // Volume (10-bit value)
        dac_state = 1;
    }
    LPC_TIM3->IR = 1; // Clear Interrupt
}

void play_sfx(int freq, int duration_ticks) {
    sfx_active = 1;
    sfx_ticks_remaining = duration_ticks;
    sfx_freq = freq;
    setup_timer_music(freq);
}

void play_effect_rotate(void) {
    play_sfx(1000, 2); 
}

void play_effect_move(void) {
    play_sfx(400, 1); 
}

void play_effect_line_clear(void) {
    play_sfx(2000, 10); 
}

void play_effect_game_over(void) {
    play_sfx(100, 50); 
}
