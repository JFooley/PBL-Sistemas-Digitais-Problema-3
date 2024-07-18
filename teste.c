#include "lib.c"
#include "visuais.c"
#include <stdio.h>

void main(){
    clear_smemory();

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