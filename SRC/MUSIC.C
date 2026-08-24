#include <dos.h>
#include <stdio.h>

static int *music_notes = NULL;
static int *music_durations = NULL;
static int music_size = 0;
static int music_index = 0;
static int music_tick_left = 0;
static int music_active = 0;

static void interrupt (*old_timer_isr)() = NULL;

void pc_speaker_on(int freq)
{
    unsigned char tmp;
    unsigned int divisor;

    if (freq < 19) freq = 19;  
    divisor = 1193180L / freq;

    outportb(0x43, 0xB6);
    outportb(0x42, divisor & 0xFF);
    outportb(0x42, (divisor >> 8) & 0xFF); 

    tmp = inportb(0x61);
    if ((tmp & 3) != 3) {
        outportb(0x61, tmp | 3);
    }
}

void pc_speaker_off()
{
    unsigned char tmp;
    tmp = inportb(0x61);
    outportb(0x61, tmp & 0xFC);
}

static void interrupt music_timer_isr()
{
    if (old_timer_isr) {
        old_timer_isr();
    }

    if (!music_active) return;

    if (music_tick_left > 0) {
        music_tick_left--;
    } else {
        music_index++;
        if (music_index >= music_size) {
            pc_speaker_off();
            music_active = 0;
            music_index = 0;
            music_tick_left = 0;
            return;
        }

        music_tick_left = music_durations[music_index];
        if (music_notes[music_index] == 0) {
            pc_speaker_off();
        } else {
            pc_speaker_on(music_notes[music_index]);
        }
    }
}

void music_stop()
{
    if (music_active) {
        pc_speaker_off();
        music_active = 0;
    }

}

void music_play(int *notes, int *durations, int size)
{
    music_stop();

    music_notes = notes;
    music_durations = durations;
    music_size = size;
    music_index = -1;
    music_tick_left = 0;
    music_active = 1;

    if (old_timer_isr == NULL) {
        old_timer_isr = getvect(0x1C);
        setvect(0x1C, music_timer_isr);
    }
}



void music_shutdown()
{
    music_stop();
    if (old_timer_isr != NULL) {
        setvect(0x1C, old_timer_isr);
        old_timer_isr = NULL;
    }
}
