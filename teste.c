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

int fd = -1;               // used to open /dev/mem for access to physical addresses
    void *LW_virtual;          // used to map physical addresses for the light-weight bridge
    
   // Create virtual memory access to the FPGA light-weight bridge
    if ((fd = open_physicall (fd)) == -1)
        return (-1);
    if ((LW_virtual = map_physicall (fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL)
        return (-1);


        
    volatile int * chave;
    chave = (unsigned int *) (LW_virtual + KEYS_BASE);
    color_t back = {0,0,7};
    color_t back1 = {7,0,0};
    ssize_t n;
    int x, y;
    int estado;
    estado=1;
    int state;
    while (1) {

        printf("\n %d",parar);
        if(*chave==14 && estado ==1){
            setBackground(back);
          estado=0; 
          if (parar == 1) {
            parar = 0;
          } else {
            parar = 1;
          }
          //parar=1;
        }

        if(*chave==15 && estado==0){
            setBackground(back1);
            estado=1;


        }

    }