#include "address_map_arm.h"
#include <asm/io.h>

static int WBR_BG(volatile int instructionComplet) {
    unsigned int dataBus[2];

    unsigned int maskA = (1ULL << 8) - 1; // Mascara até o bit 8
    unsigned int maskB = ~((1ULL << 8) - 1); // Mascara após o bit 8
    
    unsigned int dataB = maskB & instructionComplet;
    unsigned int dataA = maskA & instructionComplet;

    dataBus[0] = dataA;
    dataBus[1] = dataB;

    return dataBus;
}

static int WBR_S(volatile int instructionComplet) {
    unsigned int dataBus[2];

    unsigned int maskA = (1ULL << 8) - 1; // Mascara até o bit 8
    unsigned int maskB = ~((1ULL << 8) - 1); // Mascara após o bit 8
    
    unsigned int dataB = maskB & instructionComplet;
    unsigned int dataA = maskA & instructionComplet;

    dataBus[0] = dataA;
    dataBus[1] = dataB;

    return dataBus;
}

static int WSM(volatile int instructionComplet) {
    unsigned int dataBus[2];

    unsigned int maskA = (1ULL << 18) - 1; // Mascara até o bit 9
    unsigned int maskB = ~((1ULL << 18) - 1); // Mascara após o bit 9
    
    unsigned int dataB = maskB & instructionComplet;
    unsigned int dataA = maskA & instructionComplet;

    dataBus[0] = dataA;
    dataBus[1] = dataB;

    return dataBus;
}

static int WBM(volatile int instructionComplet) {
    unsigned int dataBus[2];

    unsigned int maskA = (1ULL << 16) - 1; // Mascara até o bit 9
    unsigned int maskB = ~((1ULL << 16) - 1); // Mascara após o bit 9
    
    unsigned int dataB = maskB & instructionComplet;
    unsigned int dataA = maskA & instructionComplet;

    dataBus[0] = dataA;
    dataBus[1] = dataB;

    return dataBus;
}

static int DP(volatile int instructionComplet) {
    unsigned int dataBus[2];

    unsigned int maskA = (1ULL << 8) - 1; // Mascara até o bit 8
    unsigned int maskB = ~((1ULL << 8) - 1); // Mascara após o bit 8
    
    unsigned int dataB = maskB & instructionComplet;
    unsigned int dataA = maskA & instructionComplet;

    dataBus[0] = dataA;
    dataBus[1] = dataB;

    return dataBus;
}