#include <raylib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>


#define SCREEN_WIDTH 64
#define SCREEN_HEIGTH 32
#define TILE_SIZE 20
#define VF V[15]
#define FONT_SIZE 5 * 16
#define PC_START_ADDR 0x200
#define FONT_ADDR 0x50

struct chip8_t {
    uint8_t memory[4096];
    uint16_t V[16];

    uint16_t stack[16];
    uint16_t stack_pointer;

    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t I;
    uint16_t PC;

    bool framebuffer[SCREEN_WIDTH*SCREEN_HEIGTH];
    bool keypad[16];

    // Nibbles
    uint16_t NNN;
    uint8_t NN;
    uint8_t N;
    uint8_t X;
    uint8_t Y;

};

uint8_t fontdata[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void inst_00e0(struct chip8_t* chip8) {
    memset(chip8->framebuffer, 0, sizeof(chip8->framebuffer));
}

void inst_00ee(struct chip8_t* chip8) {

}

void inst_1nnn(struct chip8_t* chip8) {
    chip8->PC = chip8->NNN;
}

void inst_6xnn(struct chip8_t* chip8) {
    chip8->V[chip8->X] = chip8->NN;
}

void inst_7xnn(struct chip8_t* chip8) {
    chip8->V[chip8->X] += chip8->NN;
}

void inst_annn(struct chip8_t* chip8) {
    chip8->I = chip8->NNN;
}

void inst_dxyn(struct chip8_t* chip8) {
    uint8_t x_crd, y_crd, height;
    uint8_t current_x, current_y;
    x_crd = chip8->V[chip8->X] % SCREEN_WIDTH;
    y_crd = chip8->V[chip8->Y] % SCREEN_HEIGTH;
    height = chip8->N;

    chip8->V[0xF] = 0;

    for (int row = 0; row < height ; row++){ 
        if (y_crd + row > SCREEN_HEIGTH) {
            break;
        }
        uint8_t sprite = chip8->memory[(chip8->I) + row];
        for (uint8_t px_count= 0; px_count < 8; px_count++) {
            current_x = (x_crd + px_count);
            current_y = (y_crd + row);

            uint8_t display_index = current_y * SCREEN_WIDTH + current_x;
            uint8_t current_px = chip8->framebuffer[display_index];

            if (current_x > SCREEN_WIDTH) {
                break;
            }

            if (sprite & (0x80 >> px_count)){
                if (current_px == true) {
                    chip8->framebuffer[display_index] = false;
                    chip8->V[0xF] = 0x1;
                } else if (current_px == false) {
                    chip8->framebuffer[display_index] = true;
                }
            }
        }
    }
}
void chip8_cycle(struct chip8_t* chip8) {
    uint16_t opcode = (chip8->memory[chip8->PC]) << 8 | 
        (chip8->memory[chip8->PC+1]) ;
    chip8->PC += 2;

    chip8->NNN = opcode & 0x0FFF;
    chip8->NN = opcode & 0x00FF;
    chip8->X = (opcode >> 8) & 0x0F; // First Nibble
    chip8->Y = (opcode >> 4) & 0x0F; // Second Nibble
    chip8->N = opcode & 0x000F; // Third Nibble 

    printf("PC: %04X OP: %04X I: %04X V0:%02X V1:%02X\n", 
       chip8->PC, opcode, chip8->I, chip8->V[0], chip8->V[1]);
    
    switch ((opcode >> 12) & 0x0F) { 
        // READ FIRST NIBBLE
        case 0x00:
            switch(chip8->NN) {
                case 0xe0:
                    inst_00e0(chip8);
                    break;
                case 0xee:
                    inst_00ee(chip8);
                    break;
            }
            break;
        case 0x01:
            inst_1nnn(chip8);
            break;
        case 0x06:
            inst_6xnn(chip8);
            break;
        case 0x07:
            inst_7xnn(chip8);
            break;
        case 0x0a:
            inst_annn(chip8);
            break;
        case 0x0d:
            inst_dxyn(chip8);
            break;
    }

}

void chip8_init(struct chip8_t *chip8) {
    memset(chip8, 0, sizeof(struct chip8_t));
    memcpy(&chip8->memory[FONT_ADDR], &fontdata, sizeof(fontdata));

    chip8->PC = PC_START_ADDR;

    chip8->stack_pointer = 0;

}

int main(int argc, char* argv[]){
    (void) argc;
    const int32_t wndw_width = SCREEN_WIDTH * TILE_SIZE;
    const int32_t wndw_height = SCREEN_HEIGTH * TILE_SIZE;

    const char *rom_name = argv[1];

    FILE* rom_file = fopen(rom_name, "rb");

    fseek(rom_file, 0, SEEK_END);
    int rom_size = ftell(rom_file);
    rewind(rom_file);

    struct chip8_t chip8;
    chip8_init(&chip8);

    fread(&chip8.memory[PC_START_ADDR], 1, rom_size, rom_file);

    InitWindow(wndw_width, wndw_height, "CHIP-8 Emulator");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int clock_count = 0; clock_count < 39; clock_count++){
            chip8_cycle(&chip8);
            for (int y = 0; y < SCREEN_HEIGTH; y++) {
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    uint16_t display_index = y * SCREEN_WIDTH + x;
                    if (chip8.framebuffer[display_index] == true) {
                        DrawRectangle(x * TILE_SIZE, 
                                      y * TILE_SIZE,
                                      TILE_SIZE, 
                                      TILE_SIZE,
                                      RAYWHITE);
                    }
                }
            }
        }
        EndDrawing();
    }

    for (int i = 0; i < (64*32); i++) {
        printf("px: %d value: %d ", i, chip8.framebuffer[i]);
    }
    printf("\n");
    CloseWindow();
    return 0;

}
