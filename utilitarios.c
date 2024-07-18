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

void teste()
{
    WBR_BG(2, 4, 5);

    int j;
    for (j = 0; j < 380; j++)
    {
        WSM(5, j, 7, 7, 0);
    }

    WBR_S(1, 0, 100, 100, 1);
    WBR_S(2, 1, 100, 150, 1);
    WBR_S(3, 2, 100, 200, 1);
    WBR_S(4, 3, 100, 250, 1);
    WBR_S(5, 4, 100, 300, 1);
    WBR_S(6, 5, 100, 350, 1);
}

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

void draw_ongame_background(){
    WBR_BG(0, 0, 0);
    WBM(178, 1, 1, 1);
    WBM(1128, 0, 0, 1);
    WBM(1205, 0, 0, 1);
    WBM(1206, 3, 3, 4);
    WBM(1207, 6, 6, 6);
    WBM(1208, 4, 4, 5);
    WBM(1209, 0, 0, 3);
    WBM(1210, 1, 1, 3);
    WBM(1211, 0, 0, 3);
    WBM(1212, 0, 0, 2);
    WBM(1213, 0, 0, 2);
    WBM(1214, 0, 0, 1);
    WBM(1283, 0, 0, 1);
    WBM(1284, 0, 0, 2);
    WBM(1285, 1, 1, 3);
    WBM(1286, 4, 4, 4);
    WBM(1287, 3, 3, 4);
    WBM(1288, 0, 0, 2);
    WBM(1289, 0, 0, 2);
    WBM(1290, 3, 3, 4);
    WBM(1291, 6, 6, 6);
    WBM(1292, 5, 5, 6);
    WBM(1293, 2, 2, 4);
    WBM(1294, 0, 1, 2);
    WBM(1295, 0, 0, 2);
    WBM(1296, 0, 0, 1);
    WBM(1362, 1, 1, 2);
    WBM(1363, 3, 3, 4);
    WBM(1364, 3, 3, 5);
    WBM(1365, 1, 1, 3);
    WBM(1366, 0, 0, 2);
    WBM(1367, 1, 1, 2);
    WBM(1368, 0, 0, 2);
    WBM(1369, 1, 1, 3);
    WBM(1370, 5, 5, 5);
    WBM(1371, 7, 7, 7);
    WBM(1372, 7, 7, 7);
    WBM(1373, 7, 7, 7);
    WBM(1374, 3, 4, 3);
    WBM(1375, 1, 2, 2);
    WBM(1376, 2, 3, 3);
    WBM(1377, 1, 1, 1);
    WBM(1441, 0, 0, 2);
    WBM(1442, 4, 4, 5);
    WBM(1443, 6, 6, 7);
    WBM(1444, 3, 4, 3);
    WBM(1445, 2, 2, 1);
    WBM(1446, 3, 4, 3);
    WBM(1447, 7, 7, 7);
    WBM(1448, 5, 4, 5);
    WBM(1449, 1, 1, 3);
    WBM(1450, 2, 2, 4);
    WBM(1451, 3, 3, 4);
    WBM(1452, 5, 5, 5);
    WBM(1453, 4, 4, 4);
    WBM(1454, 4, 4, 3);
    WBM(1455, 4, 5, 4);
    WBM(1456, 6, 6, 6);
    WBM(1457, 5, 5, 5);
    WBM(1458, 1, 2, 1);
    WBM(1520, 0, 0, 2);
    WBM(1521, 1, 1, 3);
    WBM(1522, 3, 3, 4);
    WBM(1523, 2, 2, 2);
    WBM(1524, 2, 2, 1);
    WBM(1525, 3, 3, 2);
    WBM(1526, 7, 7, 7);
    WBM(1527, 7, 7, 6);
    WBM(1528, 6, 6, 6);
    WBM(1529, 3, 3, 4);
    WBM(1530, 0, 0, 2);
    WBM(1531, 0, 0, 2);
    WBM(1532, 1, 2, 2);
    WBM(1533, 2, 2, 1);
    WBM(1534, 2, 3, 2);
    WBM(1535, 5, 5, 5);
    WBM(1536, 7, 7, 7);
    WBM(1537, 7, 7, 7);
    WBM(1538, 4, 4, 4);
    WBM(1539, 0, 0, 1);
}

// void main()
// {
//     limpar_sprites();
//     limpa_background();
//     draw_game_over(10, 23, 7, 0, 0);
// }
