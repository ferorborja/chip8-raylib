#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <ncurses.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGTH 32
#define VF V[15]

struct Chip8_Definition {
    uint8_t memory[4096];
    uint8_t * V[16];

    uint16_t stack[16];

    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t I;
    uint16_t PC;

    _Bool framebuffer[SCREEN_WIDTH*SCREEN_HEIGTH];
};

int main(void) {

    initscr();
    printw("Hello World!");
    refresh();
    getch();
    endwin();


    return 0;

}
