#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "lib.c"
#include "visuais.c"
#include "board_devices.c"

// Dimensões da tela em blocos
#define BACKGROUND_WIDTH 80
#define BACKGROUND_HEIGHT 60
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Endereço dos botões
#define KEY_BASE 0x00000050

// Estados do jogo
#define MENU 0
#define PLAYING 1
#define PAUSE 2
#define GAMEOVER 3

int game_state = MENU;

// Posição do mouse
int mouse_pos_x = 320;
int mouse_pos_y = 100;

// Variáveis globais para armazenar movimentos e cliques do mouse
int mouse_dx = 0;
int mouse_dy = 0;
int botaoEsquerdo = 0;
int botaoDireito = 0;
int botaoMeio = 0;

const int posicaoXCentral = 240;
const int valorMaxVida = 4;

// Nave mãe
int vida = 4;

// Pontuação
int pontos = 0;

// Variáveis globais para armazenar os valores anteriores de movimento e cliques do mouse
int dxAnterior = 0;
int dyAnterior = 0;
int botaoEsquerdoAnterior = 0;
int botaoDireitoAnterior = 0;
int botaoMeioAnterior = 0;

// Variáveis globais para armazenar os valores dos botões da placa
int key0 = 0;
int key0Anterior = 0;

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

//////////////////////////////////////////////////////////
//JOGO AQUI

int *input_thread(void *arg) {
    int fd = 0; 

    char *mouse_device = "/dev/input/mice";
    signed char data[3];

    if ((fd = open(mouse_device, O_RDONLY)) == -1) {
        perror("Erro ao abrir o dispositivo do mouse");
        return 1;
    }

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

        if (mouse_pos_x > 620) {
            mouse_pos_x = 620;
        } else if (mouse_pos_x < 0) {
            mouse_pos_x = 0;
        } else {
            mouse_pos_x += mouse_dx;
        }
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

int main() {
    pthread_t threadMouse;

    // inicializa o display 7seg
    set_display(vida, 0);
    set_display(10, 1);
    set_display(10, 2);
    set_display(0, 3);
    set_display(0, 4);
    set_display(0, 5);

    // Criação da thread para monitorar o mouse
    if (pthread_create(&threadMouse, NULL, input_thread, NULL))
    {
        fprintf(stderr, "Erro ao criar a thread do mouse\n");
        return 1;
    }

    // Inicialização das caixas de colisão
    ColiderBox nave = {mouse_pos_x, 400, 20, 1, 0, 1, 1}; // Nave na posição inicial (0, 0)
    ColiderBox tiro = {mouse_pos_x, mouse_pos_y, 20, 0, 15, 2, 2}; // posiao inical
    ColiderBox meteoro1 = {rand() % 640, 0, 20, 1, 2, 3, 3};
    ColiderBox meteoro2 = {rand() % 640, 0, 20, 1, 3, 4, 4};
    ColiderBox meteoro3 = {rand() % 640, 0, 20, 1, 4, 5, 5};

    // Clear previous
    clear_smemory();
    limpar_sprites();
    limpa_background();

    // Load visuais
    load_nave();
    load_tiro();
    load_meteoros();

    // Draw first screen
    WBR_BG(0, 0, 0);
    draw_mainmenu();

    // Adiciona o meteoro na lista
    asteroids[0] = meteoro1;
    asteroids[1] = meteoro2;
    asteroids[2] = meteoro3;
    int quantidadeMeteoros = 3; // Tamanho atual do vetor asteroids

    int contador = 0;

    // Loop principal do programa
    while (1)
    {   

        // Hard reset
        key0Anterior = key0;
        key0 = get_key(0b1110);
        if (key0 != key0Anterior && key0) {

            vida = valorMaxVida;
            nave.pos_X = posicaoXCentral;
            nave.on_screen = 1;
            pontos = 0;

            int i;
            for (i = 0; i < quantidadeMeteoros; i++)
            {
                asteroids[i].pos_Y = 0;
                asteroids[i].on_screen = 1;
                asteroids[i].pos_X = rand() % 640;
            }

            // Reseta o display e sprites
            set_display(vida, 0);
            set_display(0, 3);
            set_display(0, 4);
            set_display(0, 5);
            limpar_sprites();

            game_state = MENU;
            draw_mainmenu();
        }

        // Menu principal
        if (game_state == MENU){
            draw_rbutton_blink();

            if ((botaoDireito != botaoDireitoAnterior && botaoDireito & 0x02)) {
                game_state = PLAYING;
                draw_ongame_background();
            }
        }

        // Jogando
        else if (game_state == PLAYING)
        {
            // Renderização da nave e tiro
            WBR_S(tiro.registrador, tiro.offset, tiro.pos_X, tiro.pos_Y, tiro.on_screen);
            WBR_S(nave.registrador, nave.offset, nave.pos_X, nave.pos_Y, 1);
            nave.pos_X = mouse_pos_x;

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
                        int increment = pontos / 15;
                        if (increment > 5) increment = 5;

                        asteroids[i].pos_Y = asteroids[i].pos_Y + asteroids[i].velocidade + increment; // Movimenta o meteoro para baixo
                    }
                    else
                    {
                        asteroids[i].pos_Y = 0; // Reinicia o meteoro no topo da tela
                        asteroids[i].pos_X = rand() % 640;

                        if (vida > 0 && asteroids[i].on_screen)
                        {
                            vida--;
                            set_display(vida, 0);
                            draw_earth_damage(vida);
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
                    set_display(vida, 0);
                };

                // Colisão tiro e meteoro
                if (check_colision(tiro, asteroids[i]))
                {
                    asteroids[i].on_screen = 0;
                    asteroids[i].pos_Y = 0;
                    tiro.on_screen = 0;
                    ++pontos;

                    int dig1;
                    int dig2;
                    int dig3;
                    dig1 = pontos % 10;
                    dig2 = pontos / 10;
                    dig3 = pontos / 100;
                    set_display(dig3, 5);
                    set_display(dig2, 4);
                    set_display(dig1, 3);
                };
            }

            // Gameover  
            if (vida == 0) {
                nave.pos_X = posicaoXCentral;
                nave.on_screen = 1;

                int i;
                for (i = 0; i < quantidadeMeteoros; i++)
                {
                    asteroids[i].pos_Y = 0;
                    asteroids[i].on_screen = 1;
                    asteroids[i].pos_X = rand() % 640;
                }

                // Reseta sprites
                limpar_sprites();

                game_state = GAMEOVER;
                draw_gameover();
            }
        
            contador++;
            if (contador == 1000) {
                contador = 0;
            }
        }

        // Gameover
        if (game_state == GAMEOVER) {
            draw_rbutton_blink();
            
            if ((botaoDireito != botaoDireitoAnterior && botaoDireito & 0x02)) {
                vida = valorMaxVida;
                pontos = 0;

                set_display(vida, 0);
                set_display(0, 3);
                set_display(0, 4);
                set_display(0, 5);

                game_state = MENU;
                draw_mainmenu();
            }
        }

        // Pausa
        if (game_state == PLAYING && (botaoDireito != botaoDireitoAnterior && (botaoDireito & 0x02))) {
            game_state = PAUSE;

            draw_pause(0, 23, 7, 0, 0);
            sleep(1);
        }

        // Tira o pause
        else if (game_state == PAUSE && (botaoDireito != botaoDireitoAnterior && (botaoDireito & 0x02))) {
            game_state = PLAYING;            
            
            draw_pause(0, 23, 0, 0, 0);
            sleep(1);
            
            // limpa_background();
            // draw_ongame_background();
            // draw_earth_damage(vida);
        }   
    }

    pthread_join(threadMouse, NULL); // Aguarda a thread do mouse encerrar
    return 0;
}
