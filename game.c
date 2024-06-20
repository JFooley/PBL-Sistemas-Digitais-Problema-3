#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include "lib.c"

// Posição do mouse
int pos_x = 100;
int pos_y = 100;

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

        pos_x += mouse_dx;
        pos_y -= mouse_dy;

    }

    close(fd);
    return 0;
}

int main()
{
    pthread_t threadMouse;

    if (pthread_create(&threadMouse, NULL, monitorarMouse, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do mouse\n");
        return 1;
    }

    while (1)
    {
        WBR_S(6, 6, pos_x, pos_y, 1);
    }

    pthread_join(threadMouse, NULL);

    return 0;
}
