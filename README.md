<div align="center">
  <h1>
      Problema 3: Defenders of the Earth - Jogo Eletrônico em hardware personalizado
  </h1>

  <h3>
    Gabriel Cordeiro Moraes, 
    Georgenes Caleo Silva Pinheiro, 
    João Gabriel Lima Almeida, 
    João Pedro Silva Bastos
  </h3>

  <p>
    Engenharia de Computação – Universidade Estadual de Feira de Santana (UEFS)
    Av. Transnordestina, s/n, Novo Horizonte
    Feira de Santana – BA, Brasil – 44036-900
  </p>

  <center>gcmorais66@gmail.com, caleosilva75@gmail.com, gabriel.lima.almeida@gmail.com, joaopedro.silvabastos.splash@gmail.com</center>

</div>

# 1. Introdução
A partir dos anos 90, a industria de jogos cresceu de forma vertiginosa e hoje em dia movimento mais dinehiro do que outras industrias consolidadas como cinema e música somadas. Desde de seu surgimento, os desenvolvedores sempre enfrentaram grandes desafios para criar experiencias divertidas e prazerosas com os recursos computacionais limitados. Hoje em dia o cenário é bem diferente, apenas de ainda existirem grandes desafios para extrair o máximo desempenho dos hardwares, as game engines e hardwares especializados para jogos oferecem muitos recursos para os desenvolvedores construirem suas obras, facilitando até mesmo para desenvolvedores independentes que querem se aventurar na área. A maioria dessas bases estão atreladas a sistemas específicos e hardwares bem estabelecidos no mercado, entretanto, quando o assunto são hardwares personalizados, experimentais ou independentes, aprender a utilizar os recuros disponíveis é essencial. 

Diante disso, o Problema 3 da disciplina Sistemas Digitais traz um desafio: a construção de um jogo eletrônico para sistema linux do Kit FPGA DE1-SoC que utilize a placa de vídeo em FPGA desenvolvida pelo aluno Gabriel Sá Barreto Alves em suas atividades de iniciação científica e em seu trabalho de conclusão de curso. Os requisitos do jogo são: utilizar novas sprites, controlar o ator pelo mouse, utilizar o display para mostrar a vida e pontuação, utilizar os botões da placa para ações do jogo. Vale ressaltar que esse problema é, de certa forma, uma continuação do problema 2, em que foi desenvolvido um modulo kernel que realize a interalção software-hardware com a GPU, ou seja, o jogo utiliza o kernel desenvolvido para controlar a GPU.

# 2. Metodologia
## 2.1 Conceito do jogo
"Space Invaders" é um clássico jogo de arcade criado por Tomohiro Nishikado e lançado em 1978 pela Taito. Considerado um dos jogos mais influentes e pioneiros da história dos videogames, ele ajudou a estabelecer o gênero de tiro e foi um marco na popularização dos videogames na cultura popular. No jogo original, o jogador controla uma nave espacial que se move horizontalmente na parte inferior da tela, disparando contra fileiras de alienígenas que descem lentamente. O objetivo é destruir todos os alienígenas antes que eles alcancem a parte inferior da tela.

Nesta versão do jogo, fizemos uma alteração interessante: substituímos os alienígenas por meteoros. O jogador ainda controla uma nave espacial, mas desta vez seu objetivo é destruir os meteoros que ameaçam a Terra. A jogabilidade mantém a essência do original, exigindo reflexos rápidos e precisão nos tiros, mas a nova temática adiciona traz mais originalidade e criatividade em relação conceito clássico. O jogador deve proteger a Terra destruindo todos os meteoros antes que eles atinjam a superfície, aumentando a tensão e a urgência em cada partida.

Assim como no "Space Invaders" original, a simplicidade e a dinâmica do jogo continuam a cativar os jogadores, oferecendo diversão e desafio em um formato que atravessa gerações. Nossa adaptação busca homenagear o legado do jogo original enquanto oferece uma experiência nova e emocionante.

## 2.2 Acesso ao hardware: mouse, display e GPU

## 2.3 Controle da nave e meteoros
O controle da nave e dos meteoros é realizado de forma integrada através do uso de um dispositivo de entrada, no caso, um mouse, e da lógica de movimentação e colisão implementada.
### Controle da Nave
Movimentação: A posição horizontal da nave é controlada pelo movimento do mouse. A variável mouse_pos_x é atualizada continuamente com base na leitura dos dados do mouse. A nave é posicionada na tela com base nesta variável.

Limites da Tela: A posição da nave é limitada pela largura da tela (de 0 a 620 pixels) para garantir que ela não saia dos limites visíveis.

Estrutura da Nave: A nave é representada por uma estrutura ColiderBox que armazena suas coordenadas, tamanho e outras propriedades.

###Controle dos Meteoros
Movimentação: Os meteoros são movimentados verticalmente para baixo na tela. Cada meteoro possui uma velocidade de descida que pode aumentar com a pontuação do jogador.

Reinicialização: Quando um meteoro atinge o final da tela, sua posição é reiniciada para o topo, em uma posição horizontal aleatória.

Estrutura dos Meteoros: Similar à nave, cada meteoro é representado por uma estrutura ColiderBox.

## 2.4 Colisão
O sistema de colisão implementado no código é crucial para a dinâmica do jogo, garantindo que as interações entre a nave, os tiros e os meteoros sejam detectadas e tratadas de maneira adequada. Este sistema permite que o jogo responda às ações do jogador e aos eventos no ambiente de jogo de forma realista.

Para representar os objetos que podem colidir, como a nave, os tiros e os meteoros, é utilizada a estrutura ColiderBox. Cada ColiderBox contém informações sobre a posição, o tamanho e o estado na tela do objeto.

A verificação de colisão é realizada pela função check_colision, que compara as posições de duas ColiderBox para determinar se elas se sobrepõem. Esta função retorna 1 se houver colisão e 0 caso contrário.

Este sistema de colisão é essencial para criar uma experiência de jogo envolvente, onde as ações do jogador têm consequências diretas e visíveis no jogo. Ele também adiciona um nível de desafio, exigindo que o jogador tenha reflexos rápidos e uma boa mira para destruir os meteoros e proteger a Terra.

## 2.5 Vida, pontuação e dificuldade

# 3. Resultados
## 3.1 Funcionamento do jogo, menus e etc.
Ao iniciar o jogo, o jogador é enviado para um menu principal com o titulo do jogo e uma indicação para apertar o botão direito do mouse. Apertando, o jogador começa a jogatina: Na tela há o espaço com estrelas, na parte de baixo fica a terra cubrindo toda a extensão da parte inferior da tela, um pouco acima dela está a nave do jogador que se move no eixo horizontal acompanhando o mouse, acima surgem e descem em velocidades diferentes os meteoros. Nessa tela é possivel apertar com o botão direito do mouse para pausar o jogo, congelando a execução e mostrando a palavra "pause" na tela, apertando o botão direito novamente o jogo volta e a palavra some. Apertando com o botão esquerdo do mouse o jogador realiza um disparo, que surge imediatamente na frente da nave e sobe em uma velocidade constante até atingir o topo da tela ou um meteoro, destruindo ambos. Os meteoros podem ser de 3 tipos, que possuem sprites diferentes e velocidades diferentes. Eles descem verticalmente na tela até atingirem a parte de baixo (a terra) subtraindo uma vida caso consigam. Caso atinjam o jogador ou as vidas zerem, o jogo termina e uma tela de gameover. A tela de gameover é simples, um desenho da terra explodindo, uma mensagem "GAMEOVER" e uma indicação para apertar o botão direito, que retorna o jogador ao menu principal reiniciando o ciclo.

## 3.2 Desafios enfrentados
Durante o desenvolvimento, alguns problemas não premeditados surgiram. Um deles está relacionado a inserção de sprites na memória de sprites da placa, um dos requisitos do problema 3. O kernel desenvolvido no problema 2 já possui a implementação da instrução responsável por esse processo, a WSM (Write Sprite Memory) que recebia como parametro o endereço de memória do pixel a ser alterado e as componentes R, G e B da cor que ele assumiria. O problema é que a memória de sprites é compartilhada para todas as 31 sprites, o que significa que era necessário saber onde começava e onde terminava os endereços dos pixels de uma sprite específica. Cada sprite é 20x20 pixels o que gera 400 pixels, logo, por lógica, a cada 400 endereços deveria ser uma sprite, entretando, não foi exatamente isso que aconteceu quando tentamos pintar todos os pixels de uma sprite específica. O primeiro problema que surgiu foi que uma linha de pixels da sprite alvo não era pintada e uma linha de outra sprite recebia a cor. De início, pensamos ser algum problema de offset, mas na verdade foi descoberto que cada sprite possui na verdade endereços para 21 linhas e que a primeira e a ultima linha são compartilhadas com a sprite anterior e a próxima. Isso limitou o tamanho de nossas sprites para 18 linhas apenas, além de termos que criar uma nova instrução WSM que já realizasse o tratamento para alterar apenas os pixels dos endereços corretos. A funcionalidade antiga da WSM foi passada para uma nova função WSM_RAW (Raw de "crú"), necessária pois as sprites default que já vem na GPU não obedecem a regra de não usar a linha 1 e 19, o que é um problema já que nossa nova WSM não altera essas linhas, então a WSM_RAW foi necessária para realizar uma "limpeza" de todos os endereços previamente antes de inserir as sprites personalizadas do jogo. 

Outro desafio enfrentado foi uma decição de design a respeito de indicação dos controles. O background do jogo é construido com a matriz de blocos que a GPU oferece, ou seja, é uma imagem de 80x60 pixels, o que é bem limitado para representar alguns elementros gráficos em especial textos. Na tela de Menu e Gameover isso não foi um problema por serem titúlos que em outras circunstâncias estariam grandes na tela de qualquer forma, mas no caso de um texto de indicação de botões o espaço fica bastante limitado. Pensando nisso adotamos uma indicação não verbal: um desenho do mouse com indicação de qual botão clickar. Para destacar ainda mais, fizemos com que o botão fique piscando, indicando que é para clickar.

Sobre a adição dos elementos gráficos no jogo, outro problema que surgiu foi de como traduzir os bitmaps das sprites e background para dentro do jogo. Devido a como as instruções WSM (Write Sprite Memory) e WBS (Write Background Memory) funcionam, é necessário enviar um instrução para cada pixel, o que rapiamente se torna um problema considerando que cada sprite tem 400 pixels e cada background tem 4.800. Fazer essas imagens e desenhos manualmente através de instruções seria uma tarefa quase impossivel, por isso pensamos uma forma de automatiza-la. A ideia é bem simples, um script python auxiliar foi desenvolvido para abrir uma imagem (seja background ou sprite) e interar pixel a pixel dela, gerando a instrução de escrita na memória com as cores daquele pixel. Vale ressaltar que a placa utiliza um sistema RGB de apenas 3 bits, ou seja, cada componente RGB da cor só pode ter 8 níveis (de 0 a 7), muito inferior aos 255 do padrão RGB usado em computadores. Para garantir que a cor seja traduzida corretamente, as cores escolhidas nos desenhos das sprites e fundos foram todas multiplas de 32 (256 / 8) e, durante a conversão, cada componente foi dividida em 32. Dessa forma a cor gerada pelas instruções fica exatamente igual a representada pela placa.

# 4. Conclusão
Ao fim do desenvolvlimento do projeto podemos ter como resutlado um jogo simples e de facil interação que utiliza recursos de mapeamento de memória para acessar o hardware e é um exemplo de que é possivel desenvolver jogos mesmo em sistemas mais limitados. Vale ressaltar que os conhecimentos adiquiridos de como interagir com o hardware, interpretação dos Bits e etc em um nivel mais baixo são bem importantes mesmo para o desenvolvimento em alto nível. Entender como essas bases são feitos ajuda a utilizar elas de forma mais eficiente e muitas vezes escolher quais bibliotecas ou formas de interagir com esse hardware atendem mais suas necessidades, seja de praticidade ou desempenho.

Com relação ao problema, cumprimos basicamente todos os requisitos: utilizando o mouse, botões e display da placa, com elementos ativos e passivos no jogo e sem utilziar nenhum outro recurso além dos presentes no kit. O kernel desenvolvido no problema anterior serviu perfeitamente sem problemas, necessitando apenas da edição de uma das instruções da biblioteca por motivos de comodidade devido a um problema na forma como as sprites funcionam que não havia sido detectado no problema anterior. Vale ressaltar que utilizamos alguns conceitos de desenvolvimento de jogos para construir nosso jogo, como o uso de maquina de estados, hitboxs colisoras, tratamento da entrada para capturar o click do botão (key down), etc.

O resultado do jogo foi satisfatório mas há espaço para melhorias, principalmente na gameplay do jogo que é bastante simples. Uma melhoria que foi inclusive cogitada durante o desenvolvimento mas não entrou devido ao tempo de denvolvimento escasso foi a implementação de power ups que iriam surgir ao destuir meteoros e iriam conferir ao jogador algumas vantagens como recuperar a vida do planeta, um escudo que deixa a nave invulnerável por um tempo, novos tipos de disparos (multiplos, mais rápidos, etc), destruir todos os metoros na tela naquele momento e etc. Outra melhoria pensada seria representar os objetos utilizando mais de uma sprite (2x2 por exemplo) para que eles fiquem maior na tela e mais visíveis, já que o tamanho das sprites é bastante pequeno. Outra ideia seria um sistema de records, onde o jogo iria guardar a pontuação mais alta das partidas jogadas e mostrar uma indicação visual de que o record foi batido caso o jogador supere a ultima pontuação mais alta.

# Referências
