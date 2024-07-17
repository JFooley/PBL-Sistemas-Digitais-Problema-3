#include "address_map_arm.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILE "/dev/gpucjjg"

int simple_driver_open() {
    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Falha ao abrir o dispositivo");
        return -1;
    }
    // printf("Dispositivo aberto com sucesso!\n");
    return fd;
}

int simple_driver_close(int fd) {
    int ret = close(fd);
    if (ret < 0) {
        perror("Falha ao fechar o dispositivo");
        return -1;
    }
    // printf("Dispositivo fechado com sucesso!\n");
    return 0;
}

void intToCharArray(unsigned long long number, char *array) {
    int i;
    for (i = 0; i < 7; i++) {
        // Copia 8 bits do número para o char atual do array
        array[i] = (unsigned char)((number >> (i * 8)) & 0xFF);
    }
}

void printBinaryArray(const unsigned char *array) {
    printf("Array na lib:\n");

    int i, j;
    for (i = 7; i >= 0; i--) {  // Invertendo a ordem dos bytes
        for (j = 7; j >= 0; j--) {
            printf("%d", (array[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n\n");
}

void fileWriter(unsigned long long word) {
    static unsigned char array[8];

    intToCharArray(word, array);

    // debug
    // printBinaryArray(array);

    int fd = simple_driver_open();
    if (fd < 0) {
        return EXIT_FAILURE;
    }

    ssize_t bytes_written = write(fd, array, 8);

    if (bytes_written < 0) {
        perror("Falha ao escrever no dispositivo");
        return -1;
    }

    simple_driver_close(fd);
}

static void WBR_BG(unsigned long long R, unsigned long long G, unsigned long long B) {
    unsigned long long word = 0;
    
    word |= ((unsigned long long)R & 0b111) << 9;
    word |= ((unsigned long long)G & 0b111) << 12;
    word |= ((unsigned long long)B & 0b111) << 15;

    fileWriter(word);
}

static void WBR_S(unsigned long long reg, unsigned long long offset, unsigned long long X, unsigned long long Y, unsigned long long onScreen) {
    unsigned long long word = 0;

    word |= ((unsigned long long)reg & REG_MASK) << 4;

    word |= ((unsigned long long)offset & OFFSET_MASK) << 9;
    word |= ((unsigned long long)Y & 0b1111111111) << 19;
    word |= ((unsigned long long)X & 0b1111111111) << 29;
    word |= ((unsigned long long)onScreen & 0b1) << 39;

    fileWriter(word);
}

static void WSM(unsigned long long sprite_index, unsigned long long pixel_index, unsigned long long R, unsigned long long G, unsigned long long B) {
    unsigned long long word = 0;
    unsigned long long mem_address;

    if (pixel_index >= 380) pixel_index = 379;

    mem_address = (sprite_index * 400 + 20) + pixel_index;

    word |= ((unsigned long long)WSM_OPCODE & OPCODE_MASK) << 0;
    word |= ((unsigned long long)mem_address & SMEN_OFFSET_MASK) << 4;

    word |= ((unsigned long long)R & 0b111) << 18;
    word |= ((unsigned long long)G & 0b111) << 21;
    word |= ((unsigned long long)B & 0b111) << 24; 

    fileWriter(word);
}

static void WBM(unsigned long long mem_address, unsigned long long R, unsigned long long G, unsigned long long B) {
    unsigned long long word = 0;

    word |= ((unsigned long long)WBM_OPCODE & OPCODE_MASK) << 0;
    word |= ((unsigned long long)mem_address & SMEN_OFFSET_MASK) << 4;

    word |= ((unsigned long long)R & 0b111) << 18;
    word |= ((unsigned long long)G & 0b111) << 21;
    word |= ((unsigned long long)B & 0b111) << 24;  

    fileWriter(word);
}

static void DP(unsigned long long address, unsigned long long ref_point_X, unsigned long long ref_point_Y, unsigned long long size, unsigned long long R, unsigned long long G, unsigned long long B, unsigned long long shape) {
    unsigned long long word = 0;

    word |= ((unsigned long long)DP_OPCODE & OPCODE_MASK) << 0;
    word |= ((unsigned long long)address & DPMEN_OFFSET_MASK) << 4;

    word |= ((unsigned long long)ref_point_X & REF_POINT_MASK) << 8;
    word |= ((unsigned long long)ref_point_Y & REF_POINT_MASK) << 17;
    word |= ((unsigned long long)size & 0b1111) << 26;
    word |= ((unsigned long long)R & 0b111) << 30;
    word |= ((unsigned long long)G & 0b111) << 33;
    word |= ((unsigned long long)B & 0b111) << 36;
    word |= ((unsigned long long)shape & 0b1) << 39;

    fileWriter(word);
}
