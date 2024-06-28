#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include "lib.c"

// Posição do mouse
int mouse_pos_x = 100;
int mouse_pos_y = 100;

// Variáveis globais para armazenar movimentos e cliques do mouse
int mouse_dx = 0;
int mouse_dy = 0;
int botaoEsquerdo = 0;
int botaoDireito = 0;
int botaoMeio = 0;

// Variáveis globais para armazenar os valores anteriores de movimento e cliques do mouse
int dxAnterior = 0;
int dyAnterior = 0;
int botaoEsquerdoAnterior = 0;
int botaoDireitoAnterior = 0;
int botaoMeioAnterior = 0;

typedef struct
{
    int pos_X;      // Canto superior esquerdo
    int pos_Y;
    int length;     // Tamanho em pixels
    int on_screen;  // Está ou não na tela
} ColiderBox;

ColiderBox asteroids[10];
ColiderBox blasts[3];
ColiderBox nave;

int *monitorarMouse(void *arg)
{
    int fd;
    char *mouse_device = "/dev/input/mice";
    signed char data[3];

    fd = open(mouse_device, O_RDONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o dispositivo do mouse");
        return 1;
    }

    WBR_BG(7, 7, 7);

    while (1)
    {
        read(fd, data, sizeof(data));
        mouse_dx = data[1];
        mouse_dy = data[2];

        botaoEsquerdoAnterior = botaoEsquerdo;
        botaoEsquerdo = data[0] & 0x01;

        botaoDireitoAnterior = botaoDireito;
        botaoDireito = data[0] & 0x02;

        botaoMeioAnterior = botaoMeio;
        botaoMeio = data[0] & 0x04;

        mouse_pos_x += mouse_dx;
        mouse_pos_y -= mouse_dy;

    }

    close(fd);
    return 0;
}

int check_colision(ColiderBox costant, ColiderBox optional) {
    // Calcula as coordenadas dos cantos das caixas
    int left1 = costant.pos_X;
    int right1 = costant.pos_X + costant.length;
    int top1 = costant.pos_Y;
    int bottom1 = costant.pos_Y + costant.length;

    int left2 = optional.pos_X;
    int right2 = optional.pos_X + optional.length;
    int top2 = optional.pos_Y;
    int bottom2 = optional.pos_Y + optional.length;

    // Verifica se não há colisão
    if (bottom1 <= top2 || top1 >= bottom2 || right1 <= left2 || left1 >= right2) {
        return 0;
    }

    // Há colisão
    return 1;
};

int main()
{
    pthread_t threadMouse;

    if (pthread_create(&threadMouse, NULL, monitorarMouse, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do mouse\n");
        return 1;
    }

    ColiderBox box1 = {100, 100, 20, 1};
    ColiderBox box2 = {0, 0, 20, 1};

    while (1)
    {
        WBR_S(6, 6, mouse_pos_x, mouse_pos_y, 1);
        box2.pos_X = mouse_pos_x;
        box2.pos_Y = mouse_pos_y;
        
        WBR_S(1, 1, 100, 100, 1);
        if (check_colision(box1, box2)) {
            printf("colidiu");
        };
    }

    pthread_join(threadMouse, NULL);

    return 0;
}
