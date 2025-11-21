#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>

using namespace std;


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINICOES DE TAMANHO E MAPA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


const int celula = 42;    // tamanho da celula do mapa
const int larguramapa = 19; // largura do mapa
const int alturamapa = 21; // altura do mapa
const int alturacabecalho = celula; //altura do cabecalho(caminho no mapa que é desenhado por cima)
const int ajustevertical = alturacabecalho; //ajuste na vertical do mapa para ficar coerente ao cabecalho

// tamanho da janela de acordo com o tamanho da celula
const int largurajanela = larguramapa * celula;
const int alturajanela = alturamapa * celula + alturacabecalho;

const char paredec = '1';    //parede do mapa
const char fantasma = '2'; //porta da casa dos fantasmas do mapa
const char barreiracasinha = '3'; //parede da casinha dos fantasmas
const char pilulanormal = '0'; //pilula normal no mapa
const char pilulagrande = '5';    //pilula grande no mapa
const char fora = '4'; //lugar fora do mapa

int pontuacao = -1; // pontuação do score inicia com -1 (porque tem uma pilula debaixo de onde o finn spawna que eu não consegui tirarkkkk)
int vidas = 3;  // 3 vidas e chances para zerar
int contadorpilula = 0;   // conta o numero de pilulas comidas

//tamanho do mapa e a sua matriz
char grademapa[alturamapa][larguramapa + 1];

const char mapafixo[alturamapa][larguramapa + 1] = {
    "1111111111111111111",     //1
    "1000000001000000001",     //2
    "1511011101011101151",     //3
    "1000000000000000001",     //4
    "1011010111110101101",     //5
    "1000010001000100001",     //6
    "1111011101011101111",     //7
    "4441010000000101444",     //8
    "1111010112110101111",     //9
    "0000000133310000000",     //10
    "1111010111110101111",     //11
    "4441010000000101444",     //12
    "1111010111110101111",     //13
    "1000000001000000001",     //14
    "1011011101011101101",     //15
    "1501000000000001051",     //16
    "1101010111110101011",     //17
    "1000010001000100001",     //18
    "1011111101011111101",     //19
    "1000000000000000001",     //21
    "1111111111111111111"      //22
};




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STRUCT E FUNCOES PARA OS FANTASMAS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




struct FANTASMAS {
    int id; // id de cada fantasma
    int x, y;  // posição no mapa de cada um
    int prevx, prevy; //posição anterior de cada fantasma
    int inicialx, inicialy; //posicao inicial de cada fantasma

    //estados booleanos para cada fantasma

    bool nacasinha; // quando estao na casinha
    bool saindo;  //quando estao saindo da casa
    bool andandoaleatorio;  //quando estao andando aleatoriamente
    bool perseguindo;  //quandoe estao perseguindo
    bool assustado; // para implementar futuramente

    sf::Clock temporizadorestado; // temporizador para controlar duração do atraso antes de sair da casinha ou do modo aleatorio/perseguindo

    //variaveis para os fantasmas sairem cada um de uma vez da casa
    double atrasosaida; // atraso antes de sair da casinha (implementando para sair um fantasma de cada vez)
    int alvosaidax, alvosaiday; // ponto alvo para onde o fantasma deve ir ao sair da casinha
    int ultimadirx, ultimadiry; // ultima direção para movimento aleatório
};

FANTASMAS cadafantasma[3];

// controle de modo dos fantasmas (se está aleatorio ou perseguindo)
int contadormodofantasma = 0;
const int comprimentociclomodofantasma = 100; // alterna a cada 100 ciclos de movimento dos fantasmas
bool modoperseguicao = true; // true = perseguir, false = aleatório





//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCOES DE MAPA E POSICAO  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// copia o mapa fixo para a grade de mapa modificavel
void iniciarmapa() {
    for (int i = 0; i < alturamapa; ++i)
    {
        strcpy(grademapa[i], mapafixo[i]);
    }


// inicia os dados dos fantasmas no array de structs

    // Fantasma 0
    cadafantasma[0] = {0, 9, 9, 9, 9, 9, 9,     //id = 0 e posicoes no mapa
                     true, false, false, false, false, // nacasinha = true
                     sf::Clock(), 0.0f, 9, 8, 0, 0};
    // Fantasma 1
    cadafantasma[1] = {1, 8, 9, 8, 9, 8, 9,     //id = 1 e posicoes no mapa
                     true, false, false, false, false, // nacasinha = true
                     sf::Clock(), 2.0f, 9, 8, 0, 0};
    // Fantasma 2
    cadafantasma[2] = {2, 10, 9, 10, 9, 10, 9,    //id = 2 e posicoes no mapa
                     true, false, false, false, false, // nacasinha = true
                     sf::Clock(), 4.0f, 9, 8, 0, 0};
}



// função para verificar se a posição é válida para o Finn
bool verificarposicao(int x, int y)
{
    if (x < 0 || x >= larguramapa || y < 0 || y >= alturamapa)
    {
        return false;
    }
    char celula = grademapa[y][x];

    // o Finn não pode ir em parede, fantasma, barreira da casinha ou área fora do mapa
    return celula != paredec && celula != fantasma &&
           celula != barreiracasinha && celula != fora;
}


// funcao para verificar se a celula contem uma pilula
bool tempilula(int x, int y)
{
    return grademapa[y][x] == pilulanormal;
}

// funcao para verificar se a celula contem uma superpilula
bool temsuperpilula(int x, int y)
{
    return grademapa[y][x] == pilulagrande;
}

// remove uma plula da posicao e atualiza a pontuacao
void removerpilula(int x, int y)
{
    contadorpilula++;
    if (grademapa[y][x] == pilulanormal) {
        pontuacao += 1; //aumenta 1 ponto no score se for a normal
        grademapa[y][x] = ' ';
    }
}

// remove a superplula da posicao e atualiza a pontuacao
void removersuperpilula(int x, int y)
{
    contadorpilula++;
    if (grademapa[y][x] == pilulagrande) {
        pontuacao += 5; //aumenta 5 pontos no score se for a grande
        grademapa[y][x] = ' ';
    }
}




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ VARIAVEIS DE PERSONAGENS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



int pacx = 9;   // Posição X do Finn
int pacy = 15;   // Posição Y do Finn

int pacxanterior = pacx; // Posição X anterior do Finn
int pacyanterior = pacy; // Posição Y anterior do Finn

// Variáveis de intenção de movimento do jogador
bool intencaocima = false;
bool intencaobaixo = false;
bool intencaoesquerda = false;
bool intencaodireita = false;

// Variáveis de direção de movimento atual do personagem
bool atualcima = false;
bool atualbaixo = false;
bool atualesquerda = false;
bool atualdireita = false;



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCOES DE MOVIMENTO DO PERSONAGEM ~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



// função para mover o Finn
void moverpersonagem(int& posx, int& posy,
                      bool& ircima, bool& irbaixo, bool& iresquerda, bool& irdireita,
                      bool& atualc, bool& atualb, bool& atuale, bool& atuald,
                      int larg, int alt)
{
    int proxintx = posx;
    int proxinty = posy;
    bool intencaovalida = false;

    // Calcula a posição de intenção
    if (ircima) {
        proxinty--;
    } else if (irbaixo) {
        proxinty++;
    } else if (iresquerda) {
        proxintx--;
    } else if (irdireita) {
        proxintx++;
    }

    // verifica se a posição de intenção é válida
    if (proxintx >= 0 && proxintx < larg && proxinty >= 0 && proxinty < alt)
    {
        if (verificarposicao(proxintx, proxinty)) {
            intencaovalida = true;
        }


    } else
    {
        // lógica para túneis (os fantasmas tambem poderao atravessar)
        if ((iresquerda && proxintx < 0) || (irdireita && proxintx >= larg)) {
            intencaovalida = true;
        }
    }

    // atualiza a direção de movimento atual se a intenção for válida ou se o personagem estiver parado
    bool parado = !(atualc || atualb || atuale || atuald);
    if (intencaovalida || parado) {
        atualc = ircima;
        atualb = irbaixo;
        atuale = iresquerda;
        atuald = irdireita;
    }
//outras variaveis para receber a proxima direcao x,y
    int proxatux = posx;
    int proxatuy = posy;

    // calcula a próxima posição
    if (atualc) {
        proxatuy--;
    } else if (atualb) {
        proxatuy++;
    } else if (atuale) {
        proxatux--;
    } else if (atuald) {
        proxatux++;
    }

    // verifica se a próxima posição é válida (mesmo processo)
    if (proxatux >= 0 && proxatux < larg && proxatuy >= 0 && proxatuy < alt)
    {
        if (verificarposicao(proxatux, proxatuy)) {
            posx = proxatux;
            posy = proxatuy;
        } else {
            // para o personagem se ele bateu em uma parede
            atualc = atualb = atuale = atuald = false;
        }
    } else {
        // lógica para túneis na movimentação
        if (atuale && proxatux < 0) {
            posx = larg - 1;
        } else if (atuald && proxatux >= larg) {
            posx = 0;
        } else {
            // Para o personagem se ele bateu em uma parede no túnel ou borda inválida
            atualc = atualb = atuale = atuald = false;
        }
    }
}





//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCOES DE FANTASMAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<





// Função para verificar se a posição é válida para um fantasma incluindo colisao com outros fantasmas

// usa 'saindo' para implementar a logica da barreira da casinha
bool verificarmapafantasma(int x, int y, int larg, int alt, bool estasaindo, int idfantasmamovendo = -1) {
    if (x < 0 || x >= larg || y < 0 || y >= alt)
{
        return false;
    }
    char celula = grademapa[y][x];

    // se o fantasma estiver saindo, ele pode passar pela barreira '3'
    if (estasaindo && celula == barreiracasinha) {
        return true;
    }

    // se a posição é ocupada por OUTRO fantasma
    for (int i = 0; i < 3; ++i) {
        if (i == idfantasmamovendo) { // ignora a própria posição do fantasma que está se movendo
            continue;
        }
        if (cadafantasma[i].x == x && cadafantasma[i].y == y) {
            return false; // posição ocupada por outro fantasma
        }
    }

    // Fantasmas não podem ir em paredes ou na barreira '3' se não estiverem saindo
    return celula != paredec && celula != barreiracasinha;
}


// move o fantasma aleatoriamente
void moverfantasmaaleatoriamente(FANTASMAS& fantasma, int larg, int alt)
{

//variaveis temp para calcular a próxima posição antes de confirmar
    int nextx = fantasma.x;
    int nexty = fantasma.y;


    int direcoespoisiveis[4]; // guarda os indices das direções válidas
    int numdirecoespoisiveis = 0; //conta quantas direcoes validas foram encontradas

// calcula a direção oposta a ultima direção que o fantasma foi
// usado para evitar que o fantasma vire bruscamente no próximo movimento
    int diropostax = -fantasma.ultimadirx;
    int diropostay = -fantasma.ultimadiry;
    // Prioriza direções que não o façam voltar para evitar travamentos

//encontrar direções válidas que NÃO façam o fantasma voltar percorrendo as 4 direções possíveis
    for (int i = 0; i < 4; ++i) {
        int tempx = fantasma.x;
        int tempy = fantasma.y;
        int dircorrentex = 0, dircorrentey = 0;

//calcula as coordenadas temp e o vetor de direcao para cada uma das 4 opções
        if (i == 0) { tempy--; dircorrentey = -1; }
        else if (i == 1) { tempy++; dircorrentey = 1; }
        else if (i == 2) { tempx--; dircorrentex = -1; }
        else if (i == 3) { tempx++; dircorrentex = 1; }

        // Passa 'saindo'
        if (verificarmapafantasma(tempx, tempy, larg, alt, fantasma.saindo, fantasma.id)) {
            if (!((dircorrentex == diropostax) && (dircorrentey == diropostay))) {
                     direcoespoisiveis[numdirecoespoisiveis++] = i;
            }
        }
    }

    // se ele não encontrou direcoes que nao o façam voltar, faz as que o fazem voltar se forem validas
    // garantindo que o fantasma sempre tenha uma opção de movimento para evitar bugar
    if (numdirecoespoisiveis == 0) {
        for (int i = 0; i < 4; ++i) {
            int tempx = fantasma.x;
            int tempy = fantasma.y;
            if (i == 0) tempy--;
            else if (i == 1) tempy++;
            else if (i == 2) tempx--;
            else if (i == 3) tempx++;

            // Passa 'saindo'
            if (verificarmapafantasma(tempx, tempy, larg, alt, fantasma.saindo, fantasma.id)) {
                direcoespoisiveis[numdirecoespoisiveis++] = i;
            }
        }
    }

    int direcaoescolhida = -1;
    if (numdirecoespoisiveis > 0) {
        direcaoescolhida = direcoespoisiveis[rand() % numdirecoespoisiveis]; // escolhe aleatoriamente uma direcao
    } else {
        return; //fantasma não se move se nao tiver nenhum movimento valido
    }

    // guarda a posição atual antes de mover para usar como posicao anterior
    fantasma.prevx = fantasma.x;
    fantasma.prevy = fantasma.y;

    // calcula a nova posição baseada na direção escolhida
    if (direcaoescolhida == 0) nexty--;
    else if (direcaoescolhida == 1) nexty++;
    else if (direcaoescolhida == 2) nextx--;
    else if (direcaoescolhida == 3) nextx++;

    // logica para túneis (agora que eles tambem usamkkk)
    if (nextx < 0) {
        fantasma.x = larg - 1;
    } else if (nextx >= larg) {
        fantasma.x = 0;
    } else {
        fantasma.x = nextx;
        fantasma.y = nexty;
    }
    // atualiza a última direção baseada no movimento
    fantasma.ultimadirx = fantasma.x - fantasma.prevx;
    fantasma.ultimadiry = fantasma.y - fantasma.prevy;
}


// move o fantasma perseguindo o Finn

void moverfantasmaperseguindo(FANTASMAS& fantasma, int larg, int alt, int px, int py)
{

// variaveis temp que seriam usadas para a próxima posicao
// mas a logica de movimento é aplicada diretamente a 'fantasma.x' e 'fantasma.y'
    int nextx = fantasma.x;
    int nexty = fantasma.y;

    int direcoesdeltax[4] = {0, 0, -1, 1}; // mudanças em X para cada direcao
    int direcoesdeltay[4] = {-1, 1, 0, 0}; // mudanças em Y para cada direcao

 // uma struct simples para agrupar o indice da direcao e a distancia quadratica até o Finn para cada movimento
    struct movimentovalido {
        int indicedirecao;
        int distanciaquadrada; // distancia euclidiana ao quadrado ate o Finn
    };
    movimentovalido movimentosvalidos[4];
    int nummovimentosvalidos = 0;

    // Verifica todas as 4 direções possíveis
    for (int i = 0; i < 4; ++i) {
        int alvox = fantasma.x + direcoesdeltax[i];
        int alvoy = fantasma.y + direcoesdeltay[i];

        // aplica logica de tunel para o calculo da posicao alvo
        int alvoxreal = alvox;
        if (alvoxreal < 0) alvoxreal = larg - 1;
        else if (alvoxreal >= larg) alvoxreal = 0;

        // passa 'saindo'
        if (verificarmapafantasma(alvoxreal, alvoy, larg, alt, fantasma.saindo, fantasma.id)) {
            // calcula a distancia quadrastica para evitar operacoes de raiz quadrada mais lentas
            int distanciaquadradaatual = (alvoxreal - px) * (alvoxreal - px) + (alvoy - py) * (alvoy - py);
            movimentosvalidos[nummovimentosvalidos++] = {i, distanciaquadradaatual};
        }
    }

    if (nummovimentosvalidos == 0) {
        // se o fantasma estiver totalmente preso, ou seja, nao ter movimentos validos, ele tenta um movimento aleatório
        moverfantasmaaleatoriamente(fantasma, larg, alt);
        return;
    }

    int melhordistancia = 999999; // um valor grande para começar a encontrar a menor distancia
    int melhoresdirecoes[4]; // armazena os indices das direcoes que tem a menor distancia
    int nummelhoresdirecoes = 0;

    // encontra a menor distancia e todas as direcoes que a alcancam
    for (int i = 0; i < nummovimentosvalidos; ++i)
    {
        if (movimentosvalidos[i].distanciaquadrada < melhordistancia)
        {
            melhordistancia = movimentosvalidos[i].distanciaquadrada;
            nummelhoresdirecoes = 0; // reinicia se encontrou uma distância ainda menor
            melhoresdirecoes[nummelhoresdirecoes++] = movimentosvalidos[i].indicedirecao;

        } else if (movimentosvalidos[i].distanciaquadrada == melhordistancia) {
            melhoresdirecoes[nummelhoresdirecoes++] = movimentosvalidos[i].indicedirecao;
        }
    }

    int indicemelhoracao = -1;
    if (nummelhoresdirecoes > 0) {
        indicemelhoracao = melhoresdirecoes[rand() % nummelhoresdirecoes]; // escolhe aleatoriamente entre as melhores direcoes
    } else {
        // caso tudo der erradokkkk
        moverfantasmaaleatoriamente(fantasma, larg, alt);
        return;
    }

    // guarda a posição atual antes de mover
    fantasma.prevx = fantasma.x;
    fantasma.prevy = fantasma.y;

    // aplica o melhor movimento
    fantasma.x += direcoesdeltax[indicemelhoracao];
    fantasma.y += direcoesdeltay[indicemelhoracao];

    // logica para túneis apos o movimento de novo
    if (fantasma.x < 0) {
        fantasma.x = larg - 1;
    } else if (fantasma.x >= larg) {
        fantasma.x = 0;
    }

    // atualiza a altima direcao baseada no movimento
    fantasma.ultimadirx = fantasma.x - fantasma.prevx;
    fantasma.ultimadiry = fantasma.y - fantasma.prevy;
}

// move o fantasma fugindo do Finn
void moverfantasmaassustado(FANTASMAS& fantasma, int larg, int alt, int px, int py) {
    int direcoesdeltax[4] = {0, 0, -1, 1}; // mudanças em X para cada direcao
    int direcoesdeltay[4] = {-1, 1, 0, 0}; // mudanças em Y para cada direcao

    struct movimentovalido {
        int indicedirecao;
        int distanciaquadrada;
    };
    movimentovalido movimentosvalidos[4];
    int nummovimentosvalidos = 0;

    // Encontra a direção que maximiza a distância ao Finn
    int maiordistancia = -1;
    int melhoresdirecoes[4];
    int nummelhoresdirecoes = 0;

    for (int i = 0; i < 4; ++i) {
        int alvox = fantasma.x + direcoesdeltax[i];
        int alvoy = fantasma.y + direcoesdeltay[i];

        // Lógica de túnel para o cálculo da posição alvo
        int alvoxreal = alvox;
        if (alvoxreal < 0) alvoxreal = larg - 1;
        else if (alvoxreal >= larg) alvoxreal = 0;

        // Fantasmas assustados podem entrar na casinha e passar pela barreira '2' e '3'
        // A barreira da casinha '3' é importante para eles conseguirem retornar
        char alvo = grademapa[alvoy][alvoxreal];
        bool mover = (alvo != paredec && alvo != fora);

        // Se o fantasma estiver assustado, ele pode tentar ir para a casinha dos fantasmas
        if (fantasma.assustado && (alvo == '2' || alvo == '3')) { 
             mover = true;
        }

        if (mover) {
            // Verifica se a posição não está ocupada por outro fantasma
            bool temoutrofantasma = false;
            for (int j = 0; j < 3; ++j) {
                if (j == fantasma.id) continue;
                if (cadafantasma[j].x == alvoxreal && cadafantasma[j].y == alvoy) {
                    temoutrofantasma = true;
                    break;
                }
            }
            if (!temoutrofantasma) {
                int distanciaquadradaatual = (alvoxreal - px) * (alvoxreal - px) + (alvoy - py) * (alvoy - py);
                if (distanciaquadradaatual > maiordistancia) {
                    maiordistancia = distanciaquadradaatual;
                    nummelhoresdirecoes = 0;
                    melhoresdirecoes[nummelhoresdirecoes++] = i;
                } else if (distanciaquadradaatual == maiordistancia) {
                    melhoresdirecoes[nummelhoresdirecoes++] = i;
                }
            }
        }
    }

    if (nummelhoresdirecoes == 0) {
        // Se não houver direções para fugir, tenta um movimento aleatório
        moverfantasmaaleatoriamente(fantasma, larg, alt);
        return;
    }

    int indicemelhoracao = melhoresdirecoes[rand() % nummelhoresdirecoes];

    fantasma.prevx = fantasma.x;
    fantasma.prevy = fantasma.y;

    fantasma.x += direcoesdeltax[indicemelhoracao];
    fantasma.y += direcoesdeltay[indicemelhoracao];

    // Lógica para túneis
    if (fantasma.x < 0) {
        fantasma.x = larg - 1;
    } else if (fantasma.x >= larg) {
        fantasma.x = 0;
    }

    fantasma.ultimadirx = fantasma.x - fantasma.prevx;
    fantasma.ultimadiry = fantasma.y - fantasma.prevy;
}


// Retorna verdadeiro se o personagem esta parado
bool personagemparado() {
    return !(atualcima || atualbaixo || atualesquerda || atualdireita);
}

bool finnalterado = false;
sf::Clock tempodeefeito;
bool musicadafruta = false;
void comeuocogu() {
    finnalterado = true;
    tempodeefeito.restart();
    musicadafruta = true;

    // NOVO: Colocar todos os fantasmas no estado assustado
    for (int i = 0; i < 3; ++i) {
        cadafantasma[i].assustado = true;
        cadafantasma[i].nacasinha = false;     // Certifica que não estão presos na casinha
        cadafantasma[i].saindo = false;        // Não estão saindo
        cadafantasma[i].andandoaleatorio = false; // Não estão aleatórios
        cadafantasma[i].perseguindo = false;      // Não estão perseguindo
        cadafantasma[i].temporizadorestado.restart(); // Reinicia o temporizador para o estado assustado
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MUSICAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

sf::Music musica;
sf::Music efeitosonoro;
sf::Music efeitosonorocogao;
sf::Music efeitosonorofruta;
sf::Music musicafruta;
sf::Music musicainicio;
sf::SoundBuffer fantasmacomidobuffer; // Buffer para carregar os dados do som
sf::Sound fantasmacomido;

void tocarmusica();
void tocarmusicafruta();

bool iniciarmusicas() {
    if (!musica.openFromFile("soundtrack.ogg")) {
        return false;
    }
    if (!efeitosonoro.openFromFile("soundeffect.ogg")) {
        return false;
    }
    if (!efeitosonorocogao.openFromFile("efeitocogao.ogg")) {
        return false;
    }
    if (!efeitosonorofruta.openFromFile("efeitofruta.ogg")) {
        return false;
    }
    if (!musicafruta.openFromFile("musicaefeito.ogg")) {
        return false;
    }
    if (!musicainicio.openFromFile("musicainicio.ogg")) {
        return false;
    }
    if (!fantasmacomidobuffer.loadFromFile("fantasmacomido.ogg")) { 
        std::cout << "Erro ao carregar fantasma_comido.ogg\n";
        return false;
    }

    fantasmacomido.setBuffer(fantasmacomidobuffer); // Associar o buffer ao objeto de som
    fantasmacomido.setVolume(70);

    musica.setLoop(true);
    return true;
}

void tocarmusicainicio() {
    musicainicio.play();
}

void tocarmusica() {
    musicainicio.stop();
    musica.play();
}

void tocarmusicafruta() {
    musica.stop();
    musicafruta.play();
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCAO PRINCIPAL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



int main() {
    sf::RenderWindow janela(sf::VideoMode(largurajanela, alturajanela), "POOOC-MOOON", sf::Style::Fullscreen);
    sf::View view(sf::FloatRect(0.f,0.f,798.f,925.f));
    janela.setView(view);
    iniciarmapa(); // inicializa o mapa do jogo e os dados dos fantasmas

    if (!iniciarmusicas()) {
        std::cout << "Erro ao carregar uma ou mais musicas. Verifique os arquivos de audio." << std::endl;
        return 1;   // retorna codigo de erro
    }

    tocarmusica();
    bool musicaativada = true;
    bool sonsativados = true;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ CARREGAMENTO DE TEXTURAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CABECALHO E CAMINHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    sf::RectangleShape cabecalho(sf::Vector2f(largurajanela, alturacabecalho)); //para o cabecalho do jogo
    cabecalho.setFillColor(sf::Color(255, 192, 203));
    cabecalho.setPosition(0, 0);

    sf::Texture texturacaminho;
    if (!texturacaminho.loadFromFile("caminho.png")) {
        std::cout << "Erro lendo imagem caminho.png\n";
        return 0;
    }
    sf::Sprite spritecaminho;   //para o caminho do jogo
    spritecaminho.setTexture(texturacaminho);
    spritecaminho.setScale(static_cast<float>(celula) / texturacaminho.getSize().x,
                           static_cast<float>(celula) / texturacaminho.getSize().y);

    sf::RectangleShape retangulocaminho(sf::Vector2f(celula, celula));
    retangulocaminho.setFillColor(sf::Color(0,0,0));


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FONTE E SCORE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    sf::Font fonte; // para a fonte que escreve o score
    if (!fonte.loadFromFile("fontetexto.ttf")) {
        std::cout << "Erro ao carregar a fonte\n";
        return 0;
    }

    sf::Text textopontuacao; //para escrever o score
    textopontuacao.setFont(fonte); //usa a fonte
    textopontuacao.setCharacterSize(50); //tamanho do texto
    textopontuacao.setFillColor(sf::Color::Black); //cor
    textopontuacao.setPosition(10, (alturacabecalho - textopontuacao.getGlobalBounds().height) / 40 - 5); // posicao


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ VIDAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    sf::Texture texturacoracao; // para as vidas
    if (!texturacoracao.loadFromFile("coracao.png")) { // carrega cada imagem dos coracoes
        std::cout << "Erro lendo imagem coracao.png\n";
        return 1;
    }

    sf::Sprite spritecoracao; //sprite para carregar as texturas da vida e onde vai aparecer
    spritecoracao.setTexture(texturacoracao);
    float escalacoracao = static_cast<float>(alturacabecalho - 10) / texturacoracao.getSize().y;
    spritecoracao.setScale(escalacoracao, escalacoracao);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PAREDE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    sf::Texture parede;
    if (!parede.loadFromFile("parede.png")) {
        std::cout << "Erro lendo imagem parede.png\n";
        return 0;
    }
    sf::Sprite spriteparede(parede);
    spriteparede.setScale(static_cast<float>(celula) / parede.getSize().x,
                           static_cast<float>(celula) / parede.getSize().y);


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PILULAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


sf::Texture texturapilulanormal;
    if (!texturapilulanormal.loadFromFile("coguinho.png")) {
        return 0;
    }

    sf::Sprite spritepilulanormal(texturapilulanormal);
    spritepilulanormal.setScale(0.55f, 0.55f);

    sf::Texture texturapilulagrande;
    if (!texturapilulagrande.loadFromFile("cogao.png")) {
        return 0;
    }
    sf::Sprite spritepilulagrande(texturapilulagrande);
    spritepilulagrande.setScale(0.8f, 0.8f);


    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FRUTA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    sf::Texture texturafruta;
    if (!texturafruta.loadFromFile("cogoloko.png")) {
        return 0;
    }
    sf::Sprite spritefruta(texturafruta);
    spritefruta.setScale(1.0f, 1.0f);

    int frutaX = -1; // posicoes da fruta
    int frutaY = -1; // enquanto nao for ativada, ela fica fora do mapa
    bool frutaagora = false; // determina se esta na hora de mostrar a fruta

    sf::RectangleShape filtrocolorido(sf::Vector2f(largurajanela, alturajanela));
    sf::Clock tempofiltro;



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PERSONAGEM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



// Texturas para andando de lado, frente e costas
sf::Texture finntextlado[11];
sf::Texture finntextfrente[11];
sf::Texture finntextcostas[11];


sf::Texture* stevetexturas = finntextfrente; // comeca com frente

// carrega as texturas da animacao lateral
for (int i = 0; i <= 10; i++) {
    if (!finntextlado[i].loadFromFile("finnlado" + to_string(i) + ".png")) {
        cout << "Erro ao carregar finnlado" << i << ".png\n";
    }
}

// carrega as texturas da animacao de frente
for (int i = 1; i <= 10; i++) {
    if (!finntextfrente[i].loadFromFile("finnfrente" + to_string(i) + ".png")) {
        cout << "Erro ao carregar finnfrente" << i << ".png\n";
    }
}

// carrega as texturas da animacao de costas
for (int i = 1; i <= 8; i++) {
    if (!finntextcostas[i].loadFromFile("finncostas" + to_string(i) + ".png")) {
        cout << "Erro ao carregar finncostas" << i << ".png\n";
    }
}


//sprite Finn
sf::Sprite finnSprite;
finnSprite.setTexture(finntextfrente[1]); // textura inicial de frente
finnSprite.setScale(static_cast<float>(celula) / 65.f, static_cast<float>(celula) / 90.f);

// variáveis de controle de animacao
int frameAtual = 1;
sf::Clock relogioanimacao;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SPRITES DE CADA FANTASMA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ fantasma 1
    sf::Texture fantasma1TexturasLado[8]; // vetor para 8 frames
    for (int i = 0; i < 8; ++i) {
        if (!fantasma1TexturasLado[i].loadFromFile("fantasma1lado" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma1lado" << i + 1 << ".png\n";
            return 0; // sai do programa se a textura não carregar
        }
    }
// mesma coisa para as texturas de frente
    sf::Texture fantasma1TexturasFrente[8];
    for (int i = 0; i < 8; ++i) {
        if (!fantasma1TexturasFrente[i].loadFromFile("fantasma1frente" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma1frente" << i + 1 << ".png\n";
            return 0;
        }
    }

// mesma coisa para as texturas de costas
    sf::Texture fantasma1TexturasCostas[8];
    for (int i = 0; i < 8; ++i) {
        if (!fantasma1TexturasCostas[i].loadFromFile("fantasma1costas" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma1costas" << i + 1 << ".png\n";
            return 0;
        }
    }


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ fantasma 2
   //mesma coisa do 1
    sf::Texture fantasma2TexturasLado[8];
    for (int i = 0; i < 8; ++i) {
        if (!fantasma2TexturasLado[i].loadFromFile("fantasma2lado" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma2lado" << i + 1 << ".png\n";
            return 0;
        }
    }

    sf::Texture fantasma2TexturasFrente[8];
    for (int i = 0; i < 8; ++i) {
        if (!fantasma2TexturasFrente[i].loadFromFile("fantasma2frente" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma2frente" << i + 1 << ".png\n";
            return 0;
        }
    }

    sf::Texture fantasma2TexturasCostas[8];
    for (int i = 0; i < 8; ++i) {
        if (!fantasma2TexturasCostas[i].loadFromFile("fantasma2costas" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma2costas" << i + 1 << ".png\n";
            return 0;
        }
    }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ fantasma 3

   sf::Texture fantasma3TexturasLado[6];
    for (int i = 0; i < 6; ++i) {
        if (!fantasma3TexturasLado[i].loadFromFile("fantasma3lado" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma3lado" << i + 1 << ".png\n";
            return 0;
        }
    }

    sf::Texture fantasma3TexturasFrente[6];
    for (int i = 0; i < 6; ++i) {
        if (!fantasma3TexturasFrente[i].loadFromFile("fantasma3frente" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma3frente" << i + 1 << ".png\n";
            return 0;
        }
    }

    sf::Texture fantasma3TexturasCostas[6];
    for (int i = 0; i < 6; ++i) {
        if (!fantasma3TexturasCostas[i].loadFromFile("fantasma3costas" + std::to_string(i + 1) + ".png")) {
            std::cout << "Erro ao carregar fantasma3costas" << i + 1 << ".png\n";
            return 0;
        }
    }


    sf::Sprite spritefantasma[3];

//relogio principal para o finn
    sf::Clock relogioprincipal;
    bool viradodireita = false;

    sf::Clock relogiomovimentofantasma;
    int fantasma2FrameAtual = 0; //comeca o primeiro frame
    sf::Clock relogioAnimacaoFantasma2;

    int fantasma1FrameAtual = 0;
        sf::Clock relogioAnimacaoFantasma1;

    int fantasma3FrameAtual = 0;
    sf::Clock relogioAnimacaoFantasma3;




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ LOOP PRINCIPAL DO JOGO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


    while (janela.isOpen()) {
        sf::Event evento;
        while (janela.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                janela.close();


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ MOVIMENTO FINN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


            if (evento.type == sf::Event::KeyPressed) {
                intencaocima = intencaobaixo = intencaoesquerda = intencaodireita = false;

                if (evento.key.code == sf::Keyboard::Escape) { // aperta esc para sair do jogo
                    janela.close();
                }

                if (evento.key.code == sf::Keyboard::Left) {
                    viradodireita = false; // define a orientacao visual do sprite para a esquerda
                    intencaoesquerda = true; // ativa a intencao de mover para a esquerda
                } else if (evento.key.code == sf::Keyboard::Right) {
                    viradodireita = true;   // define a orientacao visual do sprite para a direita
                    intencaodireita = true;   // ativa a intencao de mover para a direita
                } else if (evento.key.code == sf::Keyboard::Up) {
                    intencaocima = true;
                } else if (evento.key.code == sf::Keyboard::Down) {
                    intencaobaixo = true;
                } else if (evento.key.code == sf::Keyboard::B) { //botao para o o teleporte
                    int xaleatorio, yaleatorio;
                    do {
                        xaleatorio = rand() % larguramapa;   // gera uma coordenada X aleatoria para o teleporte
                        yaleatorio = rand() % alturamapa;   // gera uma coordenada Y aleatoria para o teleporte
                    } while (!verificarposicao(xaleatorio, yaleatorio) || grademapa[yaleatorio][xaleatorio] == fora);

                    // teletransporta o finn para a nova posição aleatória
                    pacx = xaleatorio;
                    pacy = yaleatorio;
                    pontuacao -= 20; // finn perde 20 pontos ao teleportar

                // zera todas as intencoes de movimento e a direcao atual do finn
                // Isso faz ele parar imediatamente apos o teletransporte e o jogador precisa dar outra intencao
                    intencaocima = intencaobaixo = intencaoesquerda = intencaodireita = false;
                    atualcima = atualbaixo = atualesquerda = atualdireita = false;
                }
                else if (evento.key.code == sf::Keyboard::M) {
                    musicaativada = !musicaativada; // alterna o estado ativado/inativado da musica
                    if (musicaativada) {
                        tocarmusica(); // toca a musica principal se tiver ativado
                    } else {
                        musicainicio.stop(); // para as musicas se tiver desativado
                        musica.stop();
                        musicafruta.stop();
                    }
                }
                else if (evento.key.code == sf::Keyboard::S) {
                    sonsativados = !sonsativados; // alterna o estado dos efeitos sonoros
                    if (sonsativados) {
                        std::cout << "Efeitos sonoros ativados." << std::endl;
                    } else {
                        efeitosonoro.stop();
                        efeitosonorocogao.stop();
                        efeitosonorofruta.stop();
                    }
                }
            }

            //NEW FOR ARCADE
        // Os valores dos eixos são entre -100 e 100
            float eixoXJoystick0 = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
            float eixoYJoystick0 = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
            bool botao0Pressionado = sf::Joystick::isButtonPressed(0, 0); // Botão 0
            bool botao1Pressionado = sf::Joystick::isButtonPressed(0, 1); // Botão 1

            // Um pequeno "dead zone" para evitar movimentos acidentais
            const float joystickDeadZone = 25.0f; 

            if (eixoXJoystick0 < -joystickDeadZone) {
                viradodireita = false;
                intencaoesquerda = true;
            } else if (eixoXJoystick0 > joystickDeadZone) {
                viradodireita = true;
                intencaodireita = true;
            }

            if (eixoYJoystick0 < -joystickDeadZone) {
                intencaocima = true;
            } else if (eixoYJoystick0 > joystickDeadZone) {
                intencaobaixo = true;
            }

            // Lógica para o botão de teleporte do joystick
            if (botao1Pressionado) { // Usando o botão 1 como teleporte, ou o que você preferir
                 int xaleatorio, yaleatorio;
                 do {
                     xaleatorio = rand() % larguramapa;
                     yaleatorio = rand() % alturamapa;
                 } while (!verificarposicao(xaleatorio, yaleatorio) || grademapa[yaleatorio][xaleatorio] == fora);

                 pacx = xaleatorio;
                 pacy = yaleatorio;
                 pontuacao -= 20;

                 intencaocima = intencaobaixo = intencaoesquerda = intencaodireita = false;
                 atualcima = atualbaixo = atualesquerda = atualdireita = false;
            }
}

        if (relogioprincipal.getElapsedTime().asSeconds() > 0.2f) {

// guarda o estado atual de movimento do personagem antes de tentar mover ele
            //   usado para detectar se a direcao mudou para resetar a animação
            bool parado = personagemparado();
            bool estaatualcima = atualcima;
            bool estaatualbaixo = atualbaixo;
            bool estaatualesq = atualesquerda;
            bool estaatualdir = atualdireita;

        // guarda as coordenadas X e Y anteriores do finn
            pacxanterior = pacx;
            pacyanterior = pacy;
    // decide a posição real do Finn com base na sua intenção
            // e sua direção atual lidando com colisoes e tuneis
            moverpersonagem(pacx, pacy, intencaocima, intencaobaixo, intencaoesquerda, intencaodireita,
                            atualcima, atualbaixo, atualesquerda, atualdireita,
                            larguramapa, alturamapa);

        // verifica se o finn esta sobre uma pilula na nova posicao
            if (tempilula(pacx, pacy)) {
                removerpilula(pacx, pacy); // se estiver remove a pilula e atualiza a pontuacao
            }
            if (temsuperpilula(pacx, pacy)) {
                if(sonsativados) {
                    efeitosonorocogao.play();
                }
                removersuperpilula(pacx, pacy);
            }
        // aparecimento da fruta a cada 50 pilulas comidas
            if (frutaagora == false && contadorpilula > 0 && contadorpilula % 50 == 0) {
                int xrand, yrand;
                do {    //encontra posicao valida pra fruta aparecer
                    xrand = rand() % larguramapa;
                    yrand = rand() % alturamapa;
                } while (!verificarposicao(xrand, yrand) || grademapa[yrand][xrand] == fora || tempilula(xrand, yrand));
                frutaX = xrand;
                frutaY = yrand;
                frutaagora = true;      //faz a fruta aparecer!!
            }
        //comeu a fruta
            if (frutaagora && pacx == frutaX && pacy == frutaY) {
                if(sonsativados) {
                    efeitosonorofruta.play();
                }
                pontuacao += 40; // se comer a fruta ganha 42 pontos
                frutaagora = false;
                frutaX = -1; // a fruta volta pra fora do mapa
                frutaY = -1;
                comeuocogu();
                tocarmusicafruta();
            }

            if (finnalterado) {
                if (tempodeefeito.getElapsedTime().asSeconds() > 15.0f) {   //o efeito dura 15 s
                    finnalterado = false;
                    musicadafruta = false;
                    tocarmusica();
                }

                if (tempofiltro.getElapsedTime().asSeconds() > 1.5f) {
                    filtrocolorido.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255, 80)); // cor aleatoria com 80 de transparencia
                    filtrocolorido.setPosition(0,0);
                    tempofiltro.restart();
                }

            }

           // se o personagem estiver parado, reseta a animação para o primeiro frame
            if (personagemparado()) {
                finnSprite.setTexture(finntextlado[0]); // define a textura do Finn parado
                frameAtual = 0;         // reinicia o frame da animacao
                relogioanimacao.restart();
            } else {
            // verifica se a direção do movimento mudou ou se estava parado e começou a mover
                bool direcaomudou = (estaatualcima != atualcima) || (estaatualbaixo != atualbaixo) ||
                                         (estaatualesq != atualesquerda) || (estaatualdir != atualdireita) ||
                                         parado; // aqui ele acabou de sair do estado parado


        // controla a troca de frames da animação e se passou um tempo OU a direção mudou, aí atualiza o frame da animação
                if (relogioanimacao.getElapsedTime().asSeconds() > 0.10f || direcaomudou) {
                    relogioanimacao.restart();

                    if (direcaomudou) {
                        frameAtual = 1;
                    } else {
                        frameAtual++;
                    }

                    if (atualcima) {
                        if (frameAtual > 8) {
                            frameAtual = 1;
                        }
                        finnSprite.setTexture(finntextcostas[frameAtual]);
                    } else if (atualbaixo) {
                        if (frameAtual > 10) {
                            frameAtual = 1;
                        }
                        finnSprite.setTexture(finntextfrente[frameAtual]);
                    } else if (atualesquerda || atualdireita) {
                        if (frameAtual > 10) {
                            frameAtual = 1;
                        }
                        finnSprite.setTexture(finntextlado[frameAtual]);
                    }
                }
            }
        // reinicia o relógio principal do jogo para controlar a próxima atualizacao da lógica
            relogioprincipal.restart();
        }



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ MOVIMENTO FANTASMAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


        // logica de movimento dos fantasmas com velocidade própria
if (relogiomovimentofantasma.getElapsedTime().asSeconds() > 0.3f) {
            relogiomovimentofantasma.restart();

            // Atualiza o contador de modo para todos os fantasmas
            contadormodofantasma++;
    if (contadormodofantasma >= comprimentociclomodofantasma)
    {
                modoperseguicao = !modoperseguicao; // alterna o modo
                contadormodofantasma = 0;
    }

// logica de movimento dos Fantasmas
    for (int i = 0; i < 3; ++i) {
    // acessa a struct FANTASMA diretamente
    FANTASMAS& fantasmaatual = cadafantasma[i];

    if (fantasmaatual.assustado) {
        //chamamos uma nova função para mover o fantasma assustado
        moverfantasmaassustado(fantasmaatual, larguramapa, alturamapa, pacx, pacy);

        // Verifica se o tempo de assustado acabou
        if (fantasmaatual.temporizadorestado.getElapsedTime().asSeconds() > 15.0f) {
            fantasmaatual.assustado = false;
            // Volta para o modo normal
            if (modoperseguicao) {
                fantasmaatual.perseguindo = true;
            } else {
                fantasmaatual.andandoaleatorio = true;
            }
            fantasmaatual.temporizadorestado.restart(); // Reinicia para o novo estado
        }
    }
    else if (fantasmaatual.nacasinha)
    {
      if (fantasmaatual.temporizadorestado.getElapsedTime().asSeconds() >= fantasmaatual.atrasosaida)
      {
    // verifica se o tempo de atraso para sair da casinha já passou
        fantasmaatual.nacasinha = false;
        fantasmaatual.saindo = true;

    // reseta todos os outros estados para garantir que apenas 'saindo' esteja ativo
        fantasmaatual.andandoaleatorio = false;
        fantasmaatual.perseguindo = false;
         fantasmaatual.assustado = false;
      }

    } else if (fantasmaatual.saindo) {
    // se ja chegou no ponto de saída
    if (fantasmaatual.x == fantasmaatual.alvosaidax && fantasmaatual.y == fantasmaatual.alvosaiday) {
    // o fantasma está no portão e precisa fazer um movimento de "dispersão"
    // para entrar no labirinto de forma controlada

    // tenta um movimento de dispersão
    int direcaotentada = rand() % 4;
    int tempxdisp = fantasmaatual.x;
    int tempydisp = fantasmaatual.y;
    int movidoxdisp = 0, movidoydisp = 0;
    bool dispersou = false;

// tenta as 4 direções em uma ordem aleatória para dispersar
        for (int k = 0; k < 4; ++k) {
        int diraatentar = (direcaotentada + k) % 4; // calcula a próxima direção a tentar
        tempxdisp = fantasmaatual.x; // reseta as posições temporárias
        tempydisp = fantasmaatual.y;
        movidoxdisp = 0; movidoydisp = 0; // reseta as variáveis de movimento

//calcula as coordenadas temp e o vetor de direcao para a direção a ser tentada
        if (diraatentar == 0) { tempydisp--; movidoydisp = -1; }
        else if (diraatentar == 1) { tempydisp++; movidoydisp = 1; }
        else if (diraatentar == 2) { tempxdisp--; movidoxdisp = -1; }
        else if (diraatentar == 3) { tempxdisp++; movidoxdisp = 1; }

        // verifica se a posição de dispersão é válida no mapa
        // também verifica se não há outro fantasma já ocupando essa posição
          if (tempxdisp >= 0 && tempxdisp < larguramapa && tempydisp >= 0 && tempydisp < alturamapa &&
           grademapa[tempydisp][tempxdisp] != paredec && grademapa[tempydisp][tempxdisp] != barreiracasinha) {
            bool podemover = true;
            for (int j = 0; j < 3; ++j) {
             if (j == fantasmaatual.id)
               continue;
             if (cadafantasma[j].x == tempxdisp && cadafantasma[j].y == tempydisp) {
              podemover = false;
                break;
             }
        }
            if (podemover) {
    // se pode mover vai atualizar a posição do fantasma e sua última direção
              fantasmaatual.prevx = fantasmaatual.x;
              fantasmaatual.prevy = fantasmaatual.y;
              fantasmaatual.x = tempxdisp;
              fantasmaatual.y = tempydisp;
              fantasmaatual.ultimadirx = movidoxdisp;
              fantasmaatual.ultimadiry = movidoydisp;

    // Muda para estado "aleatorio"
              fantasmaatual.nacasinha = false; fantasmaatual.saindo = false;
              fantasmaatual.andandoaleatorio = true; fantasmaatual.perseguindo = false; fantasmaatual.assustado = false;

              fantasmaatual.temporizadorestado.restart(); // reinicia o temporizador do estado
              dispersou = true; // indica que o fantasma conseguiu se dispersar
              break;
            }
        }
    }
// depois de tentar todas as direções e se não conseguiu se dispersar,
    // ele tenta um movimento aleatório
    if (!dispersou) {
    moverfantasmaaleatoriamente(fantasmaatual, larguramapa, alturamapa);

      // Mudar para estado "aleatorio"
      fantasmaatual.nacasinha = false; fantasmaatual.saindo = false;
      fantasmaatual.andandoaleatorio = true; fantasmaatual.perseguindo = false; fantasmaatual.assustado = false;
      fantasmaatual.temporizadorestado.restart();
    }
    } else { // ele ainda não chegou no ponto de saída então continua se movendo pra ele
        int diffx = fantasmaatual.alvosaidax - fantasmaatual.x;
        int diffy = fantasmaatual.alvosaiday - fantasmaatual.y;

        int xantigo = fantasmaatual.x, yantigo = fantasmaatual.y; // guarda a posição antiga para 'prevx/prevy'

        int proximoxsaida = fantasmaatual.x;
        int proximoysaida = fantasmaatual.y;
        bool moveuparaasaida = false;// para indicar se o fantasma conseguiu se mover

    //prioriza o movimento na direção com maior diferença x,y para o alvo
        if (abs(diffx) > abs(diffy)) {
           proximoxsaida += (diffx > 0 ? 1 : -1);// tenta mover horizontalmente

    //verifica se o movimento horizontal é válido para o fantasma
        if (verificarmapafantasma(proximoxsaida, fantasmaatual.y, larguramapa, alturamapa, fantasmaatual.saindo, fantasmaatual.id)) {
            fantasmaatual.x = proximoxsaida;
            moveuparaasaida = true;
        } else { // se não pode mover horizontalmente entao tenta mover verticalmente como alternativa
            proximoxsaida = fantasmaatual.x;
            if (abs(diffy) > 0) {
                proximoysaida += (diffy > 0 ? 1 : -1);
                if (verificarmapafantasma(fantasmaatual.x, proximoysaida, larguramapa, alturamapa, fantasmaatual.saindo, fantasmaatual.id))
                {
                    fantasmaatual.y = proximoysaida;
                    moveuparaasaida = true;
                }
            }
    }

    } else { // prioriza o movimento vertical
    proximoysaida += (diffy > 0 ? 1 : -1); // tenta mover verticalmente
      if (verificarmapafantasma(fantasmaatual.x, proximoysaida, larguramapa, alturamapa, fantasmaatual.saindo, fantasmaatual.id)) {
          fantasmaatual.y = proximoysaida;
          moveuparaasaida = true;
    } else {
      proximoysaida = fantasmaatual.y;
    if (abs(diffx) > 0) {
      proximoxsaida += (diffx > 0 ? 1 : -1); // se não pode mover horizontalmente entao tenta mover horizontalmete como alternativa
      if (verificarmapafantasma(proximoxsaida, fantasmaatual.y, larguramapa, alturamapa, fantasmaatual.saindo, fantasmaatual.id)) {
          fantasmaatual.x = proximoxsaida;
          moveuparaasaida = true;
        }
    }
  }
}
// se o fantasma conseguir se mover em direção ate a saída, atualiza prevx/prevy
        if (moveuparaasaida) {
          fantasmaatual.prevx = xantigo;
          fantasmaatual.prevy = yantigo;
        } else {
          moverfantasmaaleatoriamente(fantasmaatual, larguramapa, alturamapa);
        }
    }

// se o fantasma está no modo de movimento aleatório
        } else if (fantasmaatual.andandoaleatorio) { // Estado aleatório
          moverfantasmaaleatoriamente(fantasmaatual, larguramapa, alturamapa);
        if (fantasmaatual.temporizadorestado.getElapsedTime().asSeconds() > 10.0f && (rand() % 2) == 0) {
              fantasmaatual.andandoaleatorio = false;
              fantasmaatual.perseguindo = true; // mudar para estado "perseguindo"
              fantasmaatual.temporizadorestado.restart();
    }

// se o fantasma está no modo de perseguição
        } else if (fantasmaatual.perseguindo) { // Estado perseguindo
              moverfantasmaperseguindo(fantasmaatual, larguramapa, alturamapa, pacx, pacy);
        if (fantasmaatual.temporizadorestado.getElapsedTime().asSeconds() > 5.0f && (rand() % 2) == 0) {
            fantasmaatual.perseguindo = false;
            fantasmaatual.andandoaleatorio = true; // Mudar para estado "aleatorio"
            fantasmaatual.temporizadorestado.restart();
       }
        }
    }
}

    //Para o Fantasma 1
        if (relogioAnimacaoFantasma1.getElapsedTime().asSeconds() > 0.10f) {
            fantasma1FrameAtual = (fantasma1FrameAtual + 1) % 8; // Avança para o próximo frame, volta ao início após o último
            relogioAnimacaoFantasma1.restart();
        }
    //Para o Fantasma 2
          if (relogioAnimacaoFantasma2.getElapsedTime().asSeconds() >  0.10f) {
            fantasma2FrameAtual = (fantasma2FrameAtual + 1) % 8; // Avança para o próximo frame, volta ao início após o último
            relogioAnimacaoFantasma2.restart();
          }
    //Para o Fantasma 3
           if (relogioAnimacaoFantasma3.getElapsedTime().asSeconds() > 0.10f) {
            fantasma3FrameAtual = (fantasma3FrameAtual + 1) % 6; // Avança para o próximo frame, volta ao início após o último
            relogioAnimacaoFantasma3.restart();
        }


    janela.clear(sf::Color(255, 0, 0)); // limpa a janela
    janela.draw(cabecalho); //desenha o cabecalho
    textopontuacao.setString("Score: " + std::to_string(pontuacao));
    janela.draw(textopontuacao); //desenha o score e a contagem

    // determina a posicao das vidas e desenha os coracoes
        for (int i = 0; i < vidas; ++i) {
            double posxcoracao = largurajanela - (i + 1) * (spritecoracao.getGlobalBounds().width + 5);
            double posycoracao = (alturacabecalho - spritecoracao.getGlobalBounds().height) / 2;
            spritecoracao.setPosition(posxcoracao, posycoracao);
            janela.draw(spritecoracao);
        }
// aqui percorre cada célula do mapa para desenhar os elementos cenário e as pílulas
        for (int i = 0; i < alturamapa; i++) {
            for (int j = 0; j < larguramapa; j++) {
                double posxcelula = static_cast<double>(j * celula);
                double posycelula = static_cast<double>(i * celula + ajustevertical);
                char tipocelula = grademapa[i][j];

                if (tipocelula == paredec) {
                    spriteparede.setPosition(posxcelula, posycelula);
                    janela.draw(spriteparede);
                } else {
                    spritecaminho.setPosition(posxcelula, posycelula);
                    janela.draw(spritecaminho);
                }
                    if (tipocelula == pilulanormal) {
                        spritepilulanormal.setPosition(posxcelula + (celula - spritepilulanormal.getGlobalBounds().width) / 3.0f,
                                                       posycelula + (celula - spritepilulanormal.getGlobalBounds().height) / 3.0f);
                        janela.draw(spritepilulanormal);
                    } else if (tipocelula == pilulagrande) {
                        spritepilulagrande.setPosition(posxcelula + (celula - spritepilulagrande.getGlobalBounds().width) / 2.0f,
                                                       posycelula + (celula - spritepilulagrande.getGlobalBounds().height) / 2.0f);
                        janela.draw(spritepilulagrande);
                    }
                    if (frutaagora && j == frutaX && i == frutaY) {
                        spritefruta.setPosition(posxcelula + (celula - spritefruta.getGlobalBounds().width) / 2.0f,
                                                         posycelula + (celula - spritefruta.getGlobalBounds().height) / 2.0f);
                        janela.draw(spritefruta);
                    }
                }
            }



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ DESENHO DE PERSONAGENS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



//~~~~~~~~~~~~~~~~~~~~~~~~~~ FINN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 // Define a escala horizontal de acordo com a direcao
double fatorEscala = 0.85f;
double escalafinn = (static_cast<double>(celula) / 65.f) * fatorEscala;
 // Ajuste baseado na largura da imagem nova

double finnlargura = 65.f * escalafinn;
double finnaltura  = 90.f * escalafinn;

// Define a escala e dimensões baseadas no tamanho do quadrado do mapa
fatorEscala = 0.85f;
escalafinn = (static_cast<float>(celula) / 65.f) * fatorEscala;
finnlargura = 65.f * escalafinn;
finnaltura  = 90.f * escalafinn;


// Define posição e escala conforme a direção do movimento
bool parado = !(atualcima || atualbaixo || atualesquerda || atualdireita);

if (parado) {
    finnSprite.setTexture(finntextlado[0]);
    finnSprite.setScale(escalafinn, escalafinn);
    finnSprite.setPosition(static_cast<float>(pacx * celula), static_cast<float>(pacy * celula + ajustevertical));

} else if (atualcima) {
    finnSprite.setTexture(finntextcostas[frameAtual]); // Usa a animacao de costas
    finnSprite.setScale(escalafinn, escalafinn); // Não espelha para cima
    finnSprite.setPosition(static_cast<float>(pacx * celula), static_cast<float>(pacy * celula + ajustevertical));

} else if (atualbaixo) {
    finnSprite.setTexture(finntextfrente[frameAtual]); // Usa a animacao de frente
    finnSprite.setScale(escalafinn, escalafinn); // Não espelha para baixo
    finnSprite.setPosition(static_cast<float>(pacx * celula), (static_cast<float>(pacy *celula) + ajustevertical));

} else if (atualesquerda) {
    finnSprite.setTexture(finntextlado[frameAtual]); // Usa a animacao lateral
    finnSprite.setScale(-escalafinn, escalafinn); // Espelha na horizontal
    // Ajusta a posição X para compensar o espelhamento
    finnSprite.setPosition(static_cast<float>(pacx * celula + finnlargura), static_cast<float>(pacy * celula + ajustevertical)); // linha modificada
} else if (atualdireita) {
    finnSprite.setTexture(finntextlado[frameAtual]); // Usa a animacabo lateral
    finnSprite.setScale(escalafinn, escalafinn); // Escala normal
    finnSprite.setPosition(static_cast<float>(pacx * celula), static_cast<float>(pacy * celula + ajustevertical));
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~ FANTASMAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i = 0; i < 3; ++i) {
        sf::Texture* texturaFantasmaAtual = nullptr;
        double escalaXFantasma = 1.0f; // para espelhar

        // Lógica para selecionar a textura do fantasma com base na direção
        if (cadafantasma[i].id == 0)
        { // fantasma 1
            if (cadafantasma[i].ultimadiry < 0) { // Movendo para cima
                texturaFantasmaAtual = &fantasma1TexturasCostas[fantasma1FrameAtual];
            } else if (cadafantasma[i].ultimadiry > 0) { // Movendo para baixo
                texturaFantasmaAtual = &fantasma1TexturasFrente[fantasma1FrameAtual];
            } else if (cadafantasma[i].ultimadirx < 0) { // Movendo para esquerda
                texturaFantasmaAtual = &fantasma1TexturasLado[fantasma1FrameAtual];
                escalaXFantasma = -1.0f; // Espelha
            } else if (cadafantasma[i].ultimadirx > 0) { // Movendo para direita
                texturaFantasmaAtual = &fantasma1TexturasLado[fantasma1FrameAtual];
                escalaXFantasma = 1.0f; // Não espelha
            } else { // parado ou direção inicial
                texturaFantasmaAtual = &fantasma1TexturasLado[0];
            }
        } else if (cadafantasma[i].id == 1)
        { // Fantasma 2
            if (cadafantasma[i].ultimadiry < 0) { // Movendo para cima
                texturaFantasmaAtual = &fantasma2TexturasCostas[fantasma2FrameAtual];
            } else if (cadafantasma[i].ultimadiry > 0) { // Movendo para baixo
                texturaFantasmaAtual = &fantasma2TexturasFrente[fantasma2FrameAtual];
            } else if (cadafantasma[i].ultimadirx < 0) { // Movendo para esquerda
                texturaFantasmaAtual = &fantasma2TexturasLado[fantasma2FrameAtual];
                escalaXFantasma = -1.0f;
            } else if (cadafantasma[i].ultimadirx > 0) { // Movendo para direita
                texturaFantasmaAtual = &fantasma2TexturasLado[fantasma2FrameAtual];
                escalaXFantasma = 1.0f;
            } else {
                texturaFantasmaAtual = &fantasma2TexturasLado[0];
            }
        } else if (cadafantasma[i].id == 2)
        {
            if (cadafantasma[i].ultimadiry < 0) {
                texturaFantasmaAtual = &fantasma3TexturasCostas[fantasma3FrameAtual];
            } else if (cadafantasma[i].ultimadiry > 0) {
                texturaFantasmaAtual = &fantasma3TexturasFrente[fantasma3FrameAtual];
            } else if (cadafantasma[i].ultimadirx < 0) {
                texturaFantasmaAtual = &fantasma3TexturasLado[fantasma3FrameAtual];
                escalaXFantasma = -1.0f;
            } else if (cadafantasma[i].ultimadirx > 0) {
                texturaFantasmaAtual = &fantasma3TexturasLado[fantasma3FrameAtual];
                escalaXFantasma = 1.0f;
            } else {
                texturaFantasmaAtual = &fantasma3TexturasLado[0];
            }
        }

        spritefantasma[i].setTexture(*texturaFantasmaAtual);
// ASSUSTADO
        // aplica tonalidade se o fantasma estiver assustado
        if (cadafantasma[i].assustado) {
            // o nivel da transparencia é o ultimo numero
            spritefantasma[i].setColor(sf::Color(0, 0, 255, 180)); // rosinha
        } else {
            // volta a cor normal se não estiver assustado
            spritefantasma[i].setColor(sf::Color::White);
        }


        // aplicar escala e posição
        double larguraTextura = spritefantasma[i].getTexture()->getSize().x;
        double alturaTextura = spritefantasma[i].getTexture()->getSize().y;

        double escalax = (static_cast<double>(celula) / larguraTextura) * abs(escalaXFantasma); // garante que o fator de escala seja positivo
        double escalay = static_cast<double>(celula) / alturaTextura;

        spritefantasma[i].setScale(escalax * escalaXFantasma, escalay); // aplica o espelhamentouuu

        double posxFantasma = static_cast<double>(cadafantasma[i].x * celula);
        if (escalaXFantasma < 0) { // Se estiver espelhado, ajuste a posição X
            posxFantasma += celula;
        }
        spritefantasma[i].setPosition(posxFantasma, static_cast<float>(cadafantasma[i].y * celula + ajustevertical));
        janela.draw(spritefantasma[i]);
    }




//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//~~~~~~~~~~~~~~~~~~~~~~~~~~ COLISAO PAC-MAN E FANTASMAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



// percorre cada um dos 3 fantasmas para verificar se houve colisão com o Finn
        for (int i = 0; i < 3; ++i) {
            // Finn e fantasma estão na mesma célula
            bool colisaodireta = (pacx == cadafantasma[i].x && pacy == cadafantasma[i].y);

            // Finn e fantasma trocaram de posições
            bool colisaocruzamento = (pacxanterior == cadafantasma[i].x && pacyanterior == cadafantasma[i].y &&
                                       pacx == cadafantasma[i].prevx && pacy == cadafantasma[i].prevy);

            if (colisaodireta || colisaocruzamento) {
                if(sonsativados) { // verifica se os sons estão ativados
                        fantasmacomido.play(); // toca o som do fantasma sendo comido
                    }
                // lógica de colisão para fantasmas assustados
                if (cadafantasma[i].assustado) {
                    pontuacao += 10; // Pontos por comer fantasma assustado
                    std::cout << "Fantasma comido!" << std::endl;

                    // Fantasma retorna para a casinha
                    cadafantasma[i].x = cadafantasma[i].inicialx;
                    cadafantasma[i].y = cadafantasma[i].inicialy;
                    cadafantasma[i].prevx = cadafantasma[i].inicialx;
                    cadafantasma[i].prevy = cadafantasma[i].inicialy;
                    cadafantasma[i].nacasinha = true;
                    cadafantasma[i].saindo = false;
                    cadafantasma[i].andandoaleatorio = false;
                    cadafantasma[i].perseguindo = false;
                    cadafantasma[i].assustado = false; // não está mais assustado
                    cadafantasma[i].temporizadorestado.restart();
                    cadafantasma[i].ultimadirx = 0;
                    cadafantasma[i].ultimadiry = 0;

                } else { // colisão com fantasma normal
                    if(sonsativados) {
                        efeitosonoro.play();
                    }
                    vidas--;
                    std::cout << "Vida perdida! Vidas restantes: " << vidas << std::endl;

                    if (vidas <= 0) {
                        std::cout << "GAME OVER\n";
                        janela.close();
                    } else {
                        pacx = 9;
                        pacy = 15; //coloca o finn na posicao inicial

                        // Reinicia todos os fantasmas para suas posições iniciais
                        for (int j = 0; j < 3; ++j) {
                            cadafantasma[j].x = cadafantasma[j].inicialx;
                            cadafantasma[j].y = cadafantasma[j].inicialy;
                            cadafantasma[j].prevx = cadafantasma[j].inicialx;
                            cadafantasma[j].prevy = cadafantasma[j].inicialy;

                            // reinicializa os estados
                            cadafantasma[j].nacasinha = true;
                            cadafantasma[j].saindo = false;
                            cadafantasma[j].andandoaleatorio = false;
                            cadafantasma[j].perseguindo = false;
                            cadafantasma[j].assustado = false;

                            cadafantasma[j].temporizadorestado.restart();
                            cadafantasma[j].ultimadirx = 0;
                            cadafantasma[j].ultimadiry = 0;
                        }
                        intencaocima = intencaobaixo = intencaoesquerda = intencaodireita = false;
                        atualcima = atualbaixo = atualesquerda = atualdireita = false;
                        sf::sleep(sf::milliseconds(1000));
                    }
                }
                break;
            }
        }

        janela.draw(finnSprite);
        if (finnalterado) {
            janela.draw(filtrocolorido);
        }
        janela.display();
    }

    return 0;
}