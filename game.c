#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include "lib.c"

// Dimensões da tela em blocos
#define BACKGROUND_WIDTH 80
#define BACKGROUND_HEIGHT 60
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

void limpar_sprites()
{

    WBR_BG(0, 0, 0);
    WBR_S(1, 0, 0, 0, 0);
    WBR_S(2, 0, 0, 0, 0);
    WBR_S(3, 0, 0, 0, 0);
    WBR_S(4, 0, 0, 0, 0);
    WBR_S(5, 0, 0, 0, 0);
    WBR_S(6, 0, 0, 0, 0);
    WBR_S(7, 0, 0, 0, 0);
    WBR_S(8, 0, 0, 0, 0);
    WBR_S(9, 0, 0, 0, 0);
    WBR_S(10, 0, 0, 0, 0);
}

void limpa_background() {
    int i;
    for (i = 0; i < 4800; i++) {
        WBM(i, 7, 7, 7);
    }
}

// Função para desenhar uma matriz de caracteres na tela
void draw_text(int start_col, int start_row, const char** text, int height, int width, unsigned long long R, unsigned long long G, unsigned long long B) {
    int row, col;
    unsigned long long mem_address;

    // Desenha o texto na tela
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            // Calcula o endereço de memória para cada posição na tela
            mem_address = (((start_row + row) * BACKGROUND_WIDTH) + (start_col + col));

            // Verifica se estamos dentro dos limites da tela
            if (mem_address < BACKGROUND_WIDTH * BACKGROUND_HEIGHT) {
                // Verifica se o caractere na matriz não é um espaço em branco ou caractere não imprimível
                if (text[row][col] != ' ' && text[row][col] != '\0') {
                    // Desenha o caractere na tela usando a função WBM
                    WBM(mem_address, R, G, B);
                }
            }
        }
    }
}


// Exemplo de uso da função para desenhar "GAME" e "OVER"
void draw_game_over(int start_col, int start_row, unsigned long long R, unsigned long long G, unsigned long long B) {
    // Matriz de caracteres para "GAME" e "OVER"
    const char* game[] = {
        "  GGG   A   M   M EEEEE  ",
        " G     A A  MM MM E      ",
        " G  GG AAAAA M M M EEEE   ",
        " G   G A   A M   M E      ",
        "  GGG  A   A M   M EEEEE  "
    };

    const char* over[] = {
        " OOO  V   V EEEE RRR ",
        "O   O V   V E    R  R",
        "O   O V   V EEE  RRR ",
        "O   O  V V  E    R R ",
        " OOO    V   EEEE R  RR"
    };

    // Altura e largura das palavras "GAME" e "OVER"
    int game_height = 5; // número de linhas na matriz "GAME"
    int game_width = 25; // comprimento da linha mais longa na matriz "GAME"
    int over_height = 5; // número de linhas na matriz "OVER"
    int over_width = 25; // comprimento da linha mais longa na matriz "OVER"

    // Desenha "GAME" na tela
    draw_text(start_col, start_row, game, game_height, game_width, R, G, B);

    // Desenha "OVER" na tela, começando logo após "GAME"
    draw_text(start_col + game_width + 1, start_row + 10, over, over_height, over_width, R, G, B);
}


//////////////////////////////////////////////////////////
//JOGO AQUI

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
    limpa_background();
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
            limpar_sprites();
            limpa_background();
            draw_game_over(10, 23, 7, 0, 0);

            while (1)
            {

                if ((botaoDireito != botaoDireitoAnterior && botaoDireito & 0x02))
                {
                    sleep(1);
                    vida = valorMaxVida;
                    nave.pos_X = posicaoXCentral;
                    nave.on_screen = 1;

                    int i;
                    for (i = 0; i < quantidadeMeteoros; i++)
                    {
                        asteroids[i].pos_Y = 0;
                        asteroids[i].on_screen = 1;
                        asteroids[i].pos_X = rand() % 640;
                    }

                    limpa_background();
                    WBR_BG(7, 7, 7);
                    break;
                }
            }
        }
    }

    pthread_join(threadMouse, NULL); // Aguarda a thread do mouse encerrar

    return 0;
}
