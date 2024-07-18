#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include "lib.c"

// Posição do mouse
int mouse_pos_x = 240;
int mouse_pos_y = 240;

// Variáveis globais para armazenar movimentos e cliques do mouse
int mouse_dx = 0;
int mouse_dy = 0;
int botaoEsquerdo = 0;
int botaoDireito = 0;
int botaoMeio = 0;

const int posicaoXCentral = 240;
const int valorMaxVida = 10;
// Nave mãe
int vida = 10;

// Variáveis globais para armazenar os valores anteriores de movimento e cliques do mouse
int dxAnterior = 0;
int dyAnterior = 0;
int botaoEsquerdoAnterior = 0;
int botaoDireitoAnterior = 0;
int botaoMeioAnterior = 0;

typedef struct
{
    int pos_X; // Canto superior esquerdo
    int pos_Y;
    int length;     // Tamanho em pixels
    int on_screen;  // Está ou não na tela
    int velocidade; // Em pixels
    int registrador;
    int offset; // Qual sprite é
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

int check_colision(ColiderBox costant, ColiderBox optional)
{
    if (optional.on_screen)
    {
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
        if (bottom1 <= top2 || top1 >= bottom2 || right1 <= left2 || left1 >= right2)
        {
            return 0;
        }

        // Há colisão
        return 1;
    }

    return 0;
};

int main()
{
    pthread_t threadMouse;

    // Criação da thread para monitorar o mouse
    if (pthread_create(&threadMouse, NULL, monitorarMouse, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do mouse\n");
        return 1;
    }

    // Inicialização das caixas de colisão
    ColiderBox meteoro1 = {rand() % 640, 0, 20, 1, 1, 5, 1};
    ColiderBox meteoro2 = {rand() % 640, 0, 20, 1, 2, 3, 2};
    ColiderBox meteoro3 = {rand() % 640, 0, 20, 1, 3, 4, 3};

    ColiderBox nave = {mouse_pos_x, 400, 20, 1}; // Nave na posição inicial (0, 0)

    ColiderBox tiro = {mouse_pos_x, mouse_pos_y, 20, 0, 8, 2, 11}; // posiao inical

    WBR_BG(7, 7, 7);

    // Adiciona o meteoro na lista;
    asteroids[0] = meteoro1;
    asteroids[1] = meteoro2;
    asteroids[2] = meteoro3;
    int quantidadeMeteoros = 3; // Tamanho atual do vetor asteroids

    // Imprimir os valores de cada elemento da lista asteroids

    // Imprimir o tamanho da lista asteroids
    // printf("Tamanho da lista asteroids: %lu\n", sizeof(asteroids) / sizeof(asteroids[0]));

    int jogoPausado = 0;
    int contador = 0;
    // Loop principal do programa
    while (1)
    {
        if (!jogoPausado)
        {
            // Renderização da nave e tiro
            WBR_S(tiro.registrador, tiro.offset, tiro.pos_X, tiro.pos_Y, tiro.on_screen);
            WBR_S(1, 5, nave.pos_X, nave.pos_Y, 1);
            nave.pos_X = mouse_pos_x;
            // nave.pos_Y = mouse_pos_y;

            // Limita a posição da nave no eixo X entre 0 e 680 pixels
            if (nave.pos_X < 0)
            {
                nave.pos_X = 0;
            }
            else if (nave.pos_X > 600)
            {
                nave.pos_X = 600;
            }

            // Verifica input de tiro
            if (botaoEsquerdo != botaoEsquerdoAnterior)
            {
                if (botaoEsquerdo & 0x01)
                {
                    // Adicionar o tiro: Se não tiver na tela
                    if (tiro.on_screen == 0)
                    {
                        tiro.on_screen = 1;
                        tiro.pos_X = nave.pos_X;
                        tiro.pos_Y = nave.pos_Y - 20;
                    }
                }
            }

            // Movimentação do meteoro e tiro
            int i;
            if (contador % 40 == 0)
            {
                // Movimenta o tiro na tela
                if (tiro.on_screen)
                {
                    if (tiro.pos_Y < 0)
                    {
                        tiro.on_screen = 0;
                    }

                    tiro.pos_Y = tiro.pos_Y - tiro.velocidade;
                }

                // Movimenta os meteoros na tela
                for (i = 0; i < quantidadeMeteoros; i++)
                {
                    if (asteroids[i].pos_Y < 480 && asteroids[i].on_screen)
                    {
                        asteroids[i].pos_Y = asteroids[i].pos_Y + asteroids[i].velocidade; // Movimenta o meteoro para baixo
                    }
                    else
                    {
                        asteroids[i].pos_Y = 0; // Reinicia o meteoro no topo da tela
                        asteroids[i].pos_X = rand() % 640;

                        printf("vida: %d\n", vida);
                        if (vida > 0 && asteroids[i].on_screen)
                        {
                            vida--;
                        }
                        else if (vida == 0)
                        {
                            printf("GAME OVER VIDA ZERO\n");
                        }
                    }
                }
            }

            // Colisão e spawn de metoros
            for (i = 0; i < quantidadeMeteoros; i++)
            {
                WBR_S(asteroids[i].registrador, asteroids[i].offset, asteroids[i].pos_X, asteroids[i].pos_Y, asteroids[i].on_screen);

                // Spawner
                if (asteroids[i].on_screen == 0 && contador == 0)
                {
                    asteroids[i].on_screen = 1;
                }
                // Colisão entre nave e meteoro
                if (check_colision(nave, asteroids[i]))
                {

                    asteroids[i].on_screen = 0;
                    vida = 0;
                    // printf("vida: %d\n", vida);
                    printf("GAME OVER NAVE ATINGIDA\n");
                };

                // Colisão tiro e meteoro
                if (check_colision(tiro, asteroids[i]))
                {
                    asteroids[i].on_screen = 0;
                    asteroids[i].pos_Y = 0;

                    tiro.on_screen = 0;
                };
            }

            contador++;
            if (contador == 1000)
            {
                contador = 0;
            }
        }

        // Verifica se a tecla 'P' foi pressionada para pausar o jogo
        if (!jogoPausado && (botaoDireito != botaoDireitoAnterior && botaoDireito & 0x02))
        {
            jogoPausado = 1;
            sleep(1);
            //>>>>>>>>>>>>>CHAMAR A ARTE DE PAUSAR O GAME AQUI<<<<<<<
        }
        else if (jogoPausado && (botaoDireito != botaoDireitoAnterior && botaoDireito & 0x02))
        {
            jogoPausado = 0;
            sleep(1);
            //>>>>>>>>>>>>>CHAMAR A ARTE DE TIRAR PAUSE O GAME AQUI<<<<<<<
        }

        if (vida == 0)
        {

            while (1)
            {

                //>>>>>>>>>>>>>CHAMAR A ARTE DE GAME OVER AQUI<<<<<<<
                if ((botaoDireito != botaoDireitoAnterior && botaoDireito & 0x02))
                {
                    sleep(1);
                    vida = valorMaxVida;
                    nave.pos_X = posicaoXCentral;

                    int i;
                    for (i = 0; i < quantidadeMeteoros; i++)
                    {
                        asteroids[i].pos_Y = 0;
                        asteroids[i].on_screen = 1;
                        asteroids[i].pos_X = rand() % 640;
                    }

                    break;
                }
            }
        }
    }

        pthread_join(threadMouse, NULL); // Aguarda a thread do mouse encerrar

    return 0;
}
