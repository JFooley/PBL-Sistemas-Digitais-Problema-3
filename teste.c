#include "lib.c"
#include <stdio.h>

// Dimensões da tela em blocos
#define BACKGROUND_WIDTH 80
#define BACKGROUND_HEIGHT 60
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void main(){
    WBR_BG(0, 0, 0);

    int j;
    for (j = 0; j < 380; j++) {
        WSM(5, j, 7, 7, 0);
    }
    
    WBR_S(1, 0, 100, 100, 1);
    WBR_S(2, 1, 100, 150, 1);
    WBR_S(3, 2, 100, 200, 1);
    WBR_S(4, 3, 100, 250, 1);
    WBR_S(5, 4, 100, 300, 1);
    WBR_S(6, 5, 100, 350, 1);
}