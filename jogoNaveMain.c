#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

//código de Nate Robins, que escreveu a GLUT Win32: http://www.xmission.com/~nate/glut.html
//carrega modelos a partir de arquivos OBJ.
#include "glm.h"
#include "GerenciadorGrafico.h"
#include "GerenciadorAudio.h"
#include "Colisao.h"

#define NUM_MAX_TIROS 10 //número máximo de tiros que podem ser disparados
#define NUM_MAX_METEOROS 20 //número máximo de meteoros que podem vir
#define FALSE 0 //booleano falso
#define TRUE 1//booleano verdadeiro

#define MAXIMO_DESENHO_TIRO -30//distancia maxima para tratar um tiro, no eixo Z
#define MAXIMO_DESENHO_METEORO 20//distancia máxima que o meteoro se movimenta no eixo Z, vindo em direção à nave.
#define Z_INICIAL_METEORO -28//posição inicial dos meteoros.

#define CAMERA_Z 8//distancia da camera no eixo Z
#define VAR_CAMERA 4//inclinação da camera

//para definir o 'campo' do jogo, representando cada canto
#define MINIMO_X -14
#define MAXIMO_X 14
#define MINIMO_Y -14
#define MAXIMO_Y 14

#define AUMENTO_VELOCIDADE_METEORO 0.05//a variação de velocidade do meteoro.
#define ESCALA_AVIAO 2//o objeto avião será redimensionado.
#define ESCALA_METEORO 1//redimensionar o meteoro..
#define ESCALA_TIRO 1//redimensionar o tiro.

//definicoes de evento de timer
#define NOVO_METEORO 88//para representar um evento para enviar novo meteoro
#define DIMINUI_EXPLOSAO 99//para o timer, diminuir o tamanho da explosao.
#define VALOR_PONTO 10//valor da pontuacao para ser somado.
#define MODELO_NAVE 1//para desenhar o modelo da nave.
#define MODELO_METEORO 2//para desenhar o modelo do meteoro.
#define MODELO_TIRO 3//para desenhar o modelo do tiro.
#define MODELO_EXPLOSAO 4//para desenhar o modelo da explosao.
#define EXPLOSAO_DECRESCIMENTO 0.1//será diminuida de 0.1 a cada intervalo de tempo.
//configurações da janela, etc.
#define GAME_OVER 666//para representar que selecionou o "botao" para game over.
#define CONTINUAR 777//para representar que selecionou o "botao" para game CONTINUE.



typedef struct
{
	GLubyte Header[12];									// TGA File Header
} TGAHeader;//cabeçalho TGA.


typedef struct
{
	GLubyte		header[6];								// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;							// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;								// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;									// Temporary Variable
	GLuint		type;
	GLuint		Height;									//Height of Image
	GLuint		Width;									//Width ofImage
	GLuint		Bpp;									// Bits Per Pixel
} TGA;//imagem tga.


TGAHeader tgaheader;									// TGA header
TGA tga;												// TGA image data

GLubyte uTGAcompare[12] = {0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header

/**
* Protótipos de funções.
*/
void desenhaCaixaColisao(caixaColisao *);
void criaCaixaColisao(GLMmodel *, caixaColisao *);
void inicializaObjetos(void);
void enviaMeteoro(void);
void setaCaixaColisao(caixaColisao *, ponto);
void atualizaCaixaColisao(itemDeJogo *);
void atualizaCaixaColisaoTiro(itemDeJogo *);
void desenhaGameOver(void);
void atualizarEstados(void);
int verificaColisao(caixaColisao , caixaColisao );
int posicaoVaziaMeteoros(void);
int posicaoVaziaTiros(void);
int posicaoVaziaExplosoes(void);
void configuraTexturas(void);
void desenhaMeteoros(void);
void desenhaExplosoes();
void desenhaTiros(void);
void DesenhaTexto(char *,int , int);
void trataTeclas(void);
void teclaEspecial(int , int , int );
void teclaEspecialSolta(int, int , int );
void aumentaDificuldade(int );
void controla(unsigned char , int , int );
void desenhaHUD();
void desenha();
void redimensiona(int , int);
void timerExplosao(int );
void timer(int );
void configura();
void configuraIluminacao();
int LoadTGA(Texture * , char * );
int LoadUncompressedTGA(Texture * , char * , FILE * );
int LoadCompressedTGA(Texture * , char * , FILE * );
void configuraAudio();
void reconfigura();
void teclaGameOverEspecial(int, int, int);
void teclaGameOverNormal(unsigned char , int, int);
extern void mixaudio(void *unused, Uint8 *stream, int len);


/**
dados globais
*/
SDL_AudioSpec fmt;
itemDeJogo nave;
//os modelos do tiro e do aviao sao carregados somente 1 vez, pois, para todos é o mesmo modelo!
GLMmodel *meteoro = NULL;
GLMmodel *tiro = NULL;
GLMmodel *explosao = NULL;
//vetores com os tiros e meteoros e explosoes que podem ser invocados.
itemDeJogo meteoros[NUM_MAX_METEOROS];
itemDeJogo tiros[NUM_MAX_TIROS];
itemDeJogo explosoes[NUM_MAX_METEOROS];//é uma explosao por meteoro no máximo.


//caixas de colisao para os meteoros e tiros, quando estao na posicao <0,0,0>
//usadas para recalcular a posicao destas ao enviar um novo objeto.
caixaColisao colisaoMeteoroDefault;
caixaColisao colisaoTiroDefault;

//texturas
Texture texturaAviao;
Texture texturaMetoro;
Texture texturaTiro;
Texture texturaFundo;
Texture texturaExplosao;
Texture texturaGameOver;
Texture continuaJogoVerde;
Texture continuaJogoVermelho;
Texture fimJogoVerde;
Texture fimJogoVermelho;

//sinaliza se as teclas foram pressionadas.
int esquerdaPressionada = FALSE;
int direitaPressionada = FALSE;
int cimaPressionada = FALSE;
int baixoPressionada = FALSE;
int atirar = FALSE;


int meteorosEnviar;//número de meteoros para enviar a cada leva.
int pausado = FALSE;//se está em pause ou não.
int tempoMeteoro ;//os meteoros serão criados neste intervalo de tempo.
int vida;//número meteoros podem bater na nave.
unsigned int pontos ;//pontuação
int gameOverSelecionado;//se selecionou continuar ou game over, após perder as vidas.


Mix_Music *musica;//música de fundo

Mix_Chunk *somTiro;//som SFX do tiro.

Mix_Chunk *somExplosao;

int audio_rate = 22050;			//Frequency of audio playback
Uint16 audio_format = AUDIO_S16; 	//Format of the audio we're playing
int audio_channels = 2;			//2 channels = stereo
int audio_buffers = 4096;


int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    configura();
    glutMainLoop();
    return 0;
}

//inicializa os objetos, variáveis e reinicializa estes ao dar um "continue" no jogo.
void inicializaObjetos(void)
{
    meteorosEnviar = 10;
    tempoMeteoro = 2000;
    pontos = 0;
    vida = 3;
    gameOverSelecionado = CONTINUAR;

    nave.posicao.x = 0;
    nave.posicao.y = 0;
    nave.posicao.z = 0;
    nave.rotX = 0;
    nave.rotY = 0;
    nave.rotZ = 0;
    nave.posicaoAnterior.x = 0;
    nave.posicaoAnterior.y = 0;
    nave.posicaoAnterior.z = 0;
    nave.aceleracao = 0.03;
    nave.visivel = TRUE;
    int scala = 0;

    if (!nave.modelo)//se o modelo não está carregado, carrega este.
    {
            nave.modelo = glmReadOBJ("data/aviao/jato.obj");
            if (!nave.modelo) exit(0);
            glmUnitize(nave.modelo);//redimensiona para a matrix unitdade..
            criaCaixaColisao(nave.modelo, &nave.colisao);
            //calcula as normais.
            glmFacetNormals(nave.modelo);
            glmVertexNormals(nave.modelo, 90.0);
    }
     for(scala = 0; scala < 8; scala++)//aplica a escala na colision box.
    {
        nave.colisao.pontos[scala].x*=ESCALA_AVIAO;
        nave.colisao.pontos[scala].y*=ESCALA_AVIAO;
        nave.colisao.pontos[scala].z*=ESCALA_AVIAO;
    }

        //carrega o meteoro
    if (! meteoro)
    {
            meteoro= glmReadOBJ("data/meteoro/meteoro.obj");

            if (!meteoro)
            {
                printf("\n\nErro carregando meteoro.obj");
                exit(0);
            }
            glmUnitize(meteoro);//redimensiona para a matrix unitdade..
            glmFacetNormals(meteoro);
            glmVertexNormals(meteoro, 90.0);
    }

    if (! explosao)
    {
            explosao= glmReadOBJ("data/explosao/explosao.obj");

            if (!explosao)
            {
                printf("\n\nErro carregando explosao.obj");
                exit(0);
            }
            glmUnitize(explosao);//redimensiona para a matrix unitdade..
            glmFacetNormals(explosao);
            glmVertexNormals(explosao, 90.0);
    }

    if (! tiro)
    {
        tiro= glmReadOBJ("data/tiro/tiro.obj");

        if (!tiro)
        {
            printf("\n\nErro carregando tiro.obj");
            exit(0);
        }
        //calcula as normais.
        glmUnitize(tiro);//redimensiona para a matrix unitdade..
        glmFacetNormals(tiro);
        glmVertexNormals(tiro, 90.0);
    }

    criaCaixaColisao(meteoro,&colisaoMeteoroDefault);
    int c;
    for(c =0; c< 8; c++)
    {
        colisaoMeteoroDefault.pontos[c].x*=ESCALA_METEORO;
        colisaoMeteoroDefault.pontos[c].y*=ESCALA_METEORO;
        colisaoMeteoroDefault.pontos[c].z*=ESCALA_METEORO;
    }
    int i =0;
    for(i =0; i < NUM_MAX_METEOROS;i++)
    {
        meteoros[i].aceleracao = 0.02;
        explosoes[i].tamanho = 1;
    }
    criaCaixaColisao(tiro,&colisaoTiroDefault);

    for(c =0; c< 8; c++)
    {
        colisaoTiroDefault.pontos[c].x*=ESCALA_TIRO;
        colisaoTiroDefault.pontos[c].y*=ESCALA_TIRO;
        colisaoTiroDefault.pontos[c].z*=ESCALA_TIRO;
    }
    for(i =0; i < NUM_MAX_TIROS;i++)
    {
        tiros[i].aceleracao = -0.1;
    }
}

void enviaMeteoro(void)
{
    int i =0;
    for(i = 0; i < meteorosEnviar; i++)//envia meteorosEnviar meteoros
    {
       int pos = posicaoVaziaMeteoros();
        if(pos >=0)//se tem uma pos disponivel..
        {
            meteoros[pos].visivel = TRUE;
            meteoros[pos].posicao.z = (Z_INICIAL_METEORO-10) - rand()%(Z_INICIAL_METEORO - -20);
            meteoros[pos].posicao.x = MINIMO_X + rand()%(MAXIMO_X-MINIMO_X);
            meteoros[pos].posicao.y = MINIMO_Y + rand()%(MAXIMO_Y-MINIMO_Y) ;
            //guarda a posição anterior para calcular a caixa de colisão para o metoro
            //em uma posição diferente da <0,0,0>
            meteoros[pos].posicaoAnterior.z = meteoros[pos].posicao.z;
            meteoros[pos].posicaoAnterior.x = meteoros[pos].posicao.x;
            meteoros[pos].posicaoAnterior.y = meteoros[pos].posicao.y;

            int c =0;
            for(c =0; c< 8; c++)
            //configura a caixa de colisão, para um meteoro na posição <0,0,0>
            {
                meteoros[pos].colisao.pontos[c].x = colisaoMeteoroDefault.pontos[c].x;
                meteoros[pos].colisao.pontos[c].y = colisaoMeteoroDefault.pontos[c].y;
                meteoros[pos].colisao.pontos[c].z = colisaoMeteoroDefault.pontos[c].z;
            }
            //calcula a posição da caixa para posição do meteoro.
            setaCaixaColisao(&meteoros[pos].colisao, meteoros[pos].posicao);
        }
    }

}

void desenhaGameOver()
{
    //desativa as callbacks de teclado do jogo..
    glutSpecialFunc(NULL);
    glutKeyboardFunc(NULL);

    //ativa callbacks de teclado para selecionar se quer continuar ou nao.
    glutSpecialFunc(teclaGameOverEspecial);
    glutKeyboardFunc(teclaGameOverNormal);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( 0,0,14,
               0,0,0 ,
               0,1,0);
    //desenha um plano de fundo e dois 'botões' para continuar ou abandonar o jogo.
    glPushMatrix();
        glTranslatef(0,0,0);
        glBindTexture(GL_TEXTURE_2D, texturaGameOver.texID);
        glScalef(0.6,0.6,0.6);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0,1.0);
            glVertex3i(MINIMO_X,MAXIMO_Y,0);
            glTexCoord2f(0.0,0.0);
            glVertex3i(MINIMO_X,MINIMO_Y,0);
            glTexCoord2f(1.0,0.0);
            glVertex3i(MAXIMO_X,MINIMO_Y,0);
            glTexCoord2f(1.0,1.0);
            glVertex3i(MAXIMO_X,MAXIMO_Y, 0);
        glEnd();
    glPopMatrix();
    glTranslatef(0,0,1);
    //botão continuar
    glPushMatrix();
        glTranslatef(-3,-2,0);
        if(gameOverSelecionado == CONTINUAR)
        {
            glBindTexture(GL_TEXTURE_2D, continuaJogoVerde.texID);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, continuaJogoVermelho.texID);
        }

        glBegin(GL_QUADS);
            glTexCoord2f(1.0,1.0);
            glVertex3f(1,1,1);

            glTexCoord2f(1.0,0.0);
            glVertex3f(1,-1,1);

            glTexCoord2f(0.0,0.0);
            glVertex3f(-1,-1,1);

            glTexCoord2f(0.0,1.0);
            glVertex3f(-1,1,1);

        glEnd();
    //botão sair.
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3,-2,0);
        if(gameOverSelecionado == GAME_OVER)
        {
            glBindTexture(GL_TEXTURE_2D, fimJogoVerde.texID);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, fimJogoVermelho.texID);
        }
        glBegin(GL_QUADS);
            glTexCoord2f(1.0,1.0);
            glVertex3f(1,1,1);

            glTexCoord2f(1.0,0.0);
            glVertex3f(1,-1,1);

            glTexCoord2f(0.0,0.0);
            glVertex3f(-1,-1,1);

            glTexCoord2f(0.0,1.0);
            glVertex3f(-1,1,1);

        glEnd();
    glPopMatrix();
}
void atualizarEstados(void)
{
    int i =0;
    for (i = 0; i < NUM_MAX_TIROS; i++)
    {
        if(tiros[i].visivel)
        {
            tiros[i].posicaoAnterior.z = tiros[i].posicao.z;
            tiros[i].posicao.z+= tiros[i].aceleracao;
            atualizaCaixaColisaoTiro(&tiros[i]);
            //o tiro irá se mover no eixo Z, indo para o valor
            //negativo deste. quando o valor z for menor que o valor
            //máximo, o tiro deixará de ser visto.
            if(tiros[i].posicao.z < MAXIMO_DESENHO_TIRO)
            {
                tiros[i].visivel = FALSE;
            }
            int m;
            for(m = 0; m < NUM_MAX_METEOROS; m++)
            {
                if(meteoros[m].visivel)
                {
                    if(verificaColisao(tiros[i].colisao,meteoros[m].colisao))
                    {
                        PlaySound(MODELO_EXPLOSAO,somExplosao);
                        meteoros[m].visivel = FALSE;
                        tiros[i].visivel = FALSE;
                        int explos = posicaoVaziaExplosoes();
                        pontos+= VALOR_PONTO;
                        if(explos>=0)
                        {
                            explosoes[explos].posicao.x = meteoros[m].posicao.x;
                            explosoes[explos].posicao.y = meteoros[m].posicao.y;
                            explosoes[explos].posicao.z = meteoros[m].posicao.z;
                            explosoes[explos].tamanho = 1;
                            explosoes[explos].visivel = TRUE;
                        }
                    }
                }
            }
        }
    }
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        if(meteoros[i].visivel)
        {
            meteoros[i].posicaoAnterior.z = meteoros[i].posicao.z;
            meteoros[i].posicao.z+= meteoros[i].aceleracao;
            atualizaCaixaColisao(&meteoros[i]);
            if(verificaColisao(meteoros[i].colisao,nave.colisao))
            {
                PlaySound(MODELO_EXPLOSAO,somExplosao);
                meteoros[i].visivel = FALSE;
                vida--;
                int explos = posicaoVaziaExplosoes();
                if(explos>=0)
                {
                    explosoes[explos].posicao.x = meteoros[i].posicao.x;
                    explosoes[explos].posicao.y = meteoros[i].posicao.y;
                    explosoes[explos].posicao.z = meteoros[i].posicao.z;
                    explosoes[explos].tamanho = 1;
                    explosoes[explos].visivel = TRUE;
                }
                if(vida<0)
                {
                    nave.visivel = FALSE;//morreu
                }
            }
            if(meteoros[i].posicao.z > MAXIMO_DESENHO_METEORO)
            {
                meteoros[i].visivel = FALSE;
            }
        }
    }
}

int posicaoVaziaMeteoros(void)
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        //encontra o primeiro meteoro vazio e o retorna.
        if(!meteoros[i].visivel)
            return i;
    }
    return -1;//caso não encontra retorna -1;
}

int posicaoVaziaTiros(void)
{
    int i =0;
    for(i = 0; i < NUM_MAX_TIROS; i++)
    {
        //encontra o primeiro tiro vazio e o retorna.
        if(!tiros[i].visivel)
            return i;
    }
    return -1;//caso não encontra retorna -1;
}

int posicaoVaziaExplosoes(void)
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        if(!explosoes[i].visivel)
            return i;
    }
    return -1;//caso não encontra retorna -1;
}
//trata as teclas de game over, para teclas especiais ESQUERDA e DIREITA. ao pressionar uma destes
//seleciona se quer continuar ou não o game.
void teclaGameOverEspecial(int tecla, int x, int y)
{
    switch(tecla)
    {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
        {
            if(gameOverSelecionado == CONTINUAR)
                gameOverSelecionado = GAME_OVER;
            else
                gameOverSelecionado = CONTINUAR;
            glutPostRedisplay();
            break;
        }
    }

}

void teclaGameOverNormal(unsigned char tecla, int x, int y)
{
    switch(tecla)
    {
        case ' ':
        case 13://13 é o ENTER
        {
            if(gameOverSelecionado == GAME_OVER)
                exit(0);//se está selecionado game over então sai do game
            else
                reconfigura();//recarrega o game..
            glutPostRedisplay();
            break;
        }
        case 27:
        {
            exit(0);
            break;
        }

    }

}
void configuraTexturas(void)//carrega as texturas para variáveis globais.
{
    if(LoadTGA(&texturaAviao,"data/aviao/textura_jato.tga"))
    {
        glGenTextures(1,&texturaAviao.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaAviao.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaAviao.bpp / 8, texturaAviao.width,
                     texturaAviao.height, 0, texturaAviao.type, GL_UNSIGNED_BYTE, texturaAviao.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do jato.");
    }

    if(LoadTGA(&texturaMetoro,"data/meteoro/meteoro_Sphere.tga"))
    {
        glGenTextures(1,&texturaMetoro.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaMetoro.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaMetoro.bpp / 8, texturaMetoro.width,
                     texturaMetoro.height, 0, texturaMetoro.type, GL_UNSIGNED_BYTE, texturaMetoro.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do meteoro");
    }

    if(LoadTGA(&texturaTiro,"data/tiro/tiro_Cube.tga"))
    {
        glGenTextures(1,&texturaTiro.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaTiro.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaTiro.bpp / 8, texturaTiro.width,
                     texturaTiro.height, 0, texturaTiro.type, GL_UNSIGNED_BYTE, texturaTiro.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do tiro");
    }

    if(LoadTGA(&texturaFundo,"data/fundo/fundo.tga"))
    {
        glGenTextures(1,&texturaFundo.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaFundo.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaFundo.bpp / 8, texturaFundo.width,
                     texturaFundo.height, 0, texturaFundo.type, GL_UNSIGNED_BYTE, texturaFundo.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fundo");
    }

    if(LoadTGA(&texturaExplosao,"data/explosao/explosao.tga"))
    {
        glGenTextures(1,&texturaExplosao.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaExplosao.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturaExplosao.width,
                     texturaExplosao.height, 0, texturaExplosao.type, GL_UNSIGNED_BYTE, texturaExplosao.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura da explosao");
    }

    if(LoadTGA(&texturaGameOver,"data/gameover/gameover.tga"))
    {
        glGenTextures(1,&texturaGameOver.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaGameOver.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturaGameOver.width,
                     texturaGameOver.height, 0, texturaGameOver.type, GL_UNSIGNED_BYTE, texturaGameOver.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do gameover");
    }

    if(LoadTGA(&continuaJogoVerde,"data/gameover/continuarVerde.tga"))
    {
        glGenTextures(1,&continuaJogoVerde.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, continuaJogoVerde.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, continuaJogoVerde.width,
                     continuaJogoVerde.height, 0, continuaJogoVerde.type, GL_UNSIGNED_BYTE, continuaJogoVerde.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do continuarVerde");
    }
    if(LoadTGA(&continuaJogoVermelho,"data/gameover/continuarVermelho.tga"))
    {
        glGenTextures(1,&continuaJogoVermelho.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, continuaJogoVermelho.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, continuaJogoVermelho.width,
                     continuaJogoVermelho.height, 0, continuaJogoVermelho.type, GL_UNSIGNED_BYTE, continuaJogoVermelho.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do continuarVermelho");
    }
    if(LoadTGA(&fimJogoVerde,"data/gameover/fimJogoVerde.tga"))
    {
        glGenTextures(1,&fimJogoVerde.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, fimJogoVerde.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fimJogoVerde.width,
                     fimJogoVerde.height, 0, fimJogoVerde.type, GL_UNSIGNED_BYTE, fimJogoVerde.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fimJogoVerde");
    }

    if(LoadTGA(&fimJogoVermelho,"data/gameover/fimJogoVermelho.tga"))
    {
        glGenTextures(1,&fimJogoVermelho.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, fimJogoVermelho.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fimJogoVermelho.width,
                     fimJogoVermelho.height, 0, fimJogoVermelho.type, GL_UNSIGNED_BYTE, fimJogoVermelho.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fimJogoVermelho");
    }
}

void desenhaMeteoros(void)//desenha os meteoros visíveis.
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        if(meteoros[i].visivel)
        {
            glPushMatrix();
               // desenhaCaixaColisao( &meteoros[i].colisao);
                glTranslatef(meteoros[i].posicao.x, meteoros[i].posicao.y, meteoros[i].posicao.z);
                desenhaModelo(MODELO_METEORO,texturaMetoro,meteoro);
            glPopMatrix();
        }
    }
}

void desenhaExplosoes()//desenha as explosões visíveis.
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        if(explosoes[i].visivel)
        {
            glColor3ub(255,255,255);
            glPushMatrix();

                glTranslatef(explosoes[i].posicao.x,explosoes[i].posicao.y,explosoes[i].posicao.z);
                glScalef(explosoes[i].tamanho,explosoes[i].tamanho,explosoes[i].tamanho);
                desenhaModelo(MODELO_EXPLOSAO,texturaExplosao,explosao);
            glPopMatrix();
        }
    }
}
void desenhaTiros(void)//desenha os tiros disparados.
{
    int i =0;
    for(i = 0; i < NUM_MAX_TIROS; i++)
    {
        if(tiros[i].visivel)
        {
            glPushMatrix();
                   // desenhaCaixaColisao( &tiros[i].colisao);
                    glTranslatef(tiros[i].posicao.x,tiros[i].posicao.y,tiros[i].posicao.z);
                    desenhaModelo(MODELO_TIRO,texturaTiro,tiro);
            glPopMatrix();
        }
    }
}
void DesenhaTexto(char *string,int posx, int posy)//desenha texto em uma posição do mundo 3d.
{
  	glPushMatrix();
        // Posição no universo onde o texto será colocado
        glRasterPos2f(nave.posicao.x-posx,nave.posicao.y-posy);
        // Exibe caracter a caracter
       // glScalef(2,2,2);

        while(*string)
             glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++);
	glPopMatrix();
}

void trataTeclas()
{
    //se for true vai ser somado com a aceleração, senão, soma com ZERO, pois aceleracao * 0 = 0!
    //faz aqui pois caso duas setas sejam pressionadas a posição é calculada e só faz um movimento.
    //a posicao da camera é alterada para seguir a nave.
    nave.posicaoAnterior.x = nave.posicao.x;
    nave.posicaoAnterior.y = nave.posicao.y;
    if(nave.posicao.x >MINIMO_X)
    {
        nave.posicao.x-=(nave.aceleracao * esquerdaPressionada);
        glLoadIdentity();
        gluLookAt( nave.posicao.x,nave.posicao.y+VAR_CAMERA,CAMERA_Z,
        //gluLookAt( posicaoNave[0],posicaoNave[1],20,
              nave.posicao.x,nave.posicao.y,nave.posicao.z ,
              0,1,0);
    }
    if(nave.posicao.x < MAXIMO_X)
    {
        nave.posicao.x+=(nave.aceleracao * direitaPressionada);
        glLoadIdentity();
        gluLookAt( nave.posicao.x,nave.posicao.y+VAR_CAMERA,CAMERA_Z,
        //gluLookAt( posicaoNave[0],posicaoNave[1],20,
              nave.posicao.x,nave.posicao.y,0,
              0,1,0);
    }

    if(nave.posicao.y < MAXIMO_Y)
    {

        nave.posicao.y+= (nave.aceleracao  * cimaPressionada);
        glLoadIdentity();
        gluLookAt( nave.posicao.x,nave.posicao.y+VAR_CAMERA,CAMERA_Z,
        //gluLookAt( posicaoNave[0],posicaoNave[1],20,
              nave.posicao.x,nave.posicao.y,0,
              0,1,0);
    }
    if(nave.posicao.y > MINIMO_Y)
    {

        nave.posicao.y-= (nave.aceleracao  * baixoPressionada);
        glLoadIdentity();
        gluLookAt( nave.posicao.x,nave.posicao.y+VAR_CAMERA,CAMERA_Z,
        //gluLookAt( posicaoNave[0],posicaoNave[1],20,
              nave.posicao.x,nave.posicao.y,nave.posicao.z ,
              0,1,0);
    }
    atualizaCaixaColisao(&nave);
    //se apertou para atirar começa a tratar um disparo.
    if(atirar)
    {
        PlaySound(MODELO_TIRO,somTiro);
        atirar = FALSE;
        int tiro = posicaoVaziaTiros();//pega a primeira pos vazia.

        if(tiro >=0)
        {
            tiros[tiro].posicao.x = nave.posicao.x-1;
            tiros[tiro].posicao.y = nave.posicao.y+0.9;
            tiros[tiro].posicao.z = nave.posicao.z;
            tiros[tiro].visivel = TRUE;//passa a ser 'ocupado'

            tiros[tiro].posicaoAnterior.x = nave.posicao.x-1;
            tiros[tiro].posicaoAnterior.y = nave.posicao.y+0.9;
            tiros[tiro].posicaoAnterior.z = nave.posicao.z;

            int c =0;
            for(c =0; c< 8; c++)
            {
                tiros[tiro].colisao.pontos[c].x = colisaoTiroDefault.pontos[c].x;

                tiros[tiro].colisao.pontos[c].y = colisaoTiroDefault.pontos[c].y;

                tiros[tiro].colisao.pontos[c].z = colisaoTiroDefault.pontos[c].z;

            }
            setaCaixaColisao(&tiros[tiro].colisao, tiros[tiro].posicao);
        }
    }

    //pede para a OpenGL redesenhar a tela, agora a nave terá novas coordenadas.
    glutPostRedisplay();

}

/**
* Controla a ação para teclas especiais, como F*, SETAS, CTRL, quando estas são pressionadas.
*/
void teclaEspecial(int tecla, int x, int y)
{
    //ao pressionar uma tecla armazena seu estado para baixo. Assim a nave se movimenta enquanto
    //a tecla estiver pressionada.
    switch (tecla)
    {
        case GLUT_KEY_LEFT:
        {
                esquerdaPressionada = TRUE;
                nave.rotacao = 30;
                nave.rotZ = 1;
                break;
        }
        case GLUT_KEY_RIGHT:
        {
                direitaPressionada = TRUE;
                nave.rotacao = -30;
                nave.rotZ = 1;
                break;

        }
        case GLUT_KEY_UP:
        {
                cimaPressionada = TRUE;
                break;

        }
        case GLUT_KEY_DOWN:
        {
                baixoPressionada = TRUE;
                break;
        }
    }
}

/**
* Controla a ação para teclas especiais, como F*, SETAS, CTRL, quando estas são soltas.
*/
void teclaEspecialSolta(int tecla, int x, int y)
{
    //ao soltar uma tecla, armazena o estado que esta foi solta.
    switch (tecla)
    {
        case GLUT_KEY_LEFT:
        {
                esquerdaPressionada = FALSE;
                nave.rotacao = 0;
                nave.rotZ = 0;
                break;
        }
        case GLUT_KEY_RIGHT:
        {
                direitaPressionada = FALSE;
                nave.rotacao = 0;
                nave.rotZ = 0;
                break;
        }
        case GLUT_KEY_UP:
        {
                cimaPressionada = FALSE;
                break;
        }
        case GLUT_KEY_DOWN:
        {
                baixoPressionada = FALSE;
                break;
        }
    }
}
/**
* Controla a função de teclas normais no jogo. Teclas normais como letras, ESC, etc.
*/
void aumentaDificuldade(int t)//a cada tempo esta funcao é chamada e a dificuldade aumenta.
{
    if(nave.visivel)
    {
        if(!pausado)
        {
            int i =0;
            for(i =0; i < NUM_MAX_METEOROS; i++)
            {
                meteoros[i].aceleracao+= AUMENTO_VELOCIDADE_METEORO;
            }
            if(meteorosEnviar<NUM_MAX_METEOROS)
                meteorosEnviar++;
            tempoMeteoro-=10;
            glutTimerFunc(tempoMeteoro,timer,0);
            glutTimerFunc(10000,aumentaDificuldade,0);
        }
    }
}
void controla(unsigned char tecla, int x, int y)
{
    switch (tecla)
    {
        case 27:
        {
            limpa(musica);
            exit(0) ;
            break;
        }
        case ' ':
        {
            if( (!pausado) && (nave.visivel))
                atirar = TRUE;
            break;
        }
        case 'p':
        {
            if(pausado)
            {
                pausado = FALSE;
                glutSpecialFunc(teclaEspecial);
                glutSpecialUpFunc(teclaEspecialSolta);
                glutReshapeFunc(redimensiona);
               glutPostRedisplay();
               glutTimerFunc(tempoMeteoro,  timer,  NOVO_METEORO);
               glutTimerFunc(200, timerExplosao, 0);
               glutTimerFunc(10000,aumentaDificuldade,0);
            }
            else
            {
                glutSpecialFunc(NULL);
                glutSpecialUpFunc(NULL);
                glutReshapeFunc(NULL);
                pausado = TRUE;
                glutPostRedisplay();
            }
            break;
        }
    }
}
void desenhaHUD()//desenha as informações na tela.
{
    char texto[20];
    sprintf( texto,"Vida: %d",vida);
    DesenhaTexto(texto,5,5);
    char texto2[20];
    sprintf(texto2,"Pontuacao: %d",pontos);
    DesenhaTexto(texto2,6,6);
}
void desenha()//desenha os itens do jogo.
{
    //limpa os buffers de cor e profundidade.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(!pausado)
    {
        if(nave.visivel)
        {
            desenhaFundo(texturaFundo);
            //desenha a nave na posição atual dela.
            glPushMatrix();
            glTranslatef(nave.posicao.x,nave.posicao.y,nave.posicao.z);
            glRotatef( nave.rotacao,0,0,nave.rotZ);
            desenhaModelo(MODELO_NAVE,texturaAviao,nave.modelo);
            glPopMatrix();
            desenhaTiros();
            desenhaMeteoros();
            desenhaExplosoes();
            desenhaHUD();
        }
        else
            desenhaGameOver();
        atualizarEstados();
        trataTeclas();
    }
    else
    {
        DesenhaTexto("PAUSE",  5,5);
    }
    glutSwapBuffers();
}

void redimensiona(int larg, int alt)
{
    //altera a projeção
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,larg/alt,0.5,110);

    //reseta as alterações do MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //'olha'
    gluLookAt(nave.posicao.x,nave.posicao.y+20,20,
              0,0,0,
              0,1,0);
    //calcula a nova viewport
    glViewport(0,0,larg,alt);
    //habilita o teste de profundidade.
    glEnable(GL_DEPTH_TEST);
}


void timerExplosao(int t)//chamado a cada tempo e diminui o tamanho da explosao, para dar
//efeito que ela desapareça aos poucos.
{
    if(nave.visivel)
    {
        if(!pausado)
        {
            int i;
            for(i = 0; i < NUM_MAX_METEOROS; i++)
            {
                if(explosoes[i].visivel)
                {
                    explosoes[i].tamanho-= EXPLOSAO_DECRESCIMENTO;
                    if(explosoes[i].tamanho <= 0.3)
                        explosoes[i].visivel = FALSE;
                }
            }
            glutTimerFunc(200, timerExplosao, t);
        }
    }
}


void timer(int t)//timer para invocar um/uns novo/novos metoro/meteoros
{
    if(nave.visivel)
    {
        if(!pausado)
        {
            printf("novo meteoro");
            if(t == NOVO_METEORO)
            {
                enviaMeteoro();
                glutTimerFunc(tempoMeteoro,  timer,  NOVO_METEORO);
            }
        }
    }
}

void configura()//configuração inicial do game.
{
    inicializaObjetos();
	iniciaGlut();
    glutDisplayFunc(desenha);//configura a função de desenho
    glutKeyboardFunc(controla);
    glutSpecialFunc(teclaEspecial);
    glutSpecialUpFunc(teclaEspecialSolta);
    glutReshapeFunc(redimensiona);

	iniciaCamera();
    configuraTexturas();
    glutTimerFunc(1000,  timer,  NOVO_METEORO);
    glutTimerFunc(500, timerExplosao, 0);
    glutTimerFunc(20000,aumentaDificuldade,0);


	iniciaAudio(audio_rate,audio_format,audio_channels,audio_buffers);
    musica = Mix_LoadMUS("data/musica/musica.ogg");
    somTiro = Mix_LoadWAV("data/audio/tiro.wav");
    somExplosao = Mix_LoadWAV("data/audio/explosao.wav");
    tocaMusica(musica);
}
void reconfigura()//chamado ao recomeçar um novo game.
{
    inicializaObjetos();
    criaCaixaColisao(nave.modelo, &nave.colisao);
    glClearColor(0,0,0,0);//cor de limpeza, ou seja, do fundo
    glutKeyboardFunc(controla);
    glutSpecialFunc(teclaEspecial);
    glutSpecialUpFunc(teclaEspecialSolta);
    glutTimerFunc(1000,  timer,  NOVO_METEORO);
    glutTimerFunc(500, timerExplosao, 0);
    glutTimerFunc(20000,aumentaDificuldade,0);
	int i;
	for( i =0; i < NUM_MAX_METEOROS; i ++)
	{
	    explosoes[i].visivel = FALSE;
	    meteoros[i].visivel = FALSE;
	}
}
void configuraIluminacao()
{
    float corAmbienteLuz[] = {1,1,1,1};//uma luz branca.
    float corDifusaLuz[] = {1,1,1,1};
    float corSpecularLuz[] = {1,1,1,1};
    float posicaoLuz[] = {0,0,0 ,0.2};//posicao da luz.

    glLightfv(GL_LIGHT0, GL_AMBIENT, corAmbienteLuz);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
    float materialAmbiente[] = {0.2,0.2,0.2,1};
    float materialDifusa[] = {1,1,1,1};
    float materialSpecular[] = {1,1,1,1};
    float materialEmissao[] = {0,0,0,1};
    float brilho = 70;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDifusa);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);

    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,brilho);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

/**
* O Código abaixo abre uma imagem do tipo TGA e a carrega. Este código foi obtido no endereço
    http://www.3dcodingtutorial.com/Textures/Loading-Textures.html

*/
int LoadTGA(Texture * texture, char * filename)				// Load a TGA file
{
	FILE * fTGA;												// File pointer to texture file
	fTGA = fopen(filename, "rb");								// Open file for reading

	if(fTGA == NULL)											// If it didn't open....
	{
		//MessageBox(NULL, "Could not open texture file", "ERROR", MB_OK);	// Display an error message
		return FALSE;														// Exit function
	}

	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)					// Attempt to read 12 byte header from file
	{
		//MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);		// If it fails, display an error message
		if(fTGA != NULL)													// Check to seeiffile is still open
		{
			fclose(fTGA);													// If it is, close it
		}
		return FALSE;														// Exit function
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)				// See if header matches the predefined header of
	{																		// an Uncompressed TGA image
		LoadUncompressedTGA(texture, filename, fTGA);						// If so, jump to Uncompressed TGA loading code
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		// See if header matches the predefined header of
	{																		// an RLE compressed TGA image
		LoadCompressedTGA(texture, filename, fTGA);							// If so, jump to Compressed TGA loading code
	}
	else																	// If header matches neither type
	{
		//MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK);	// Display an error
		fclose(fTGA);
		return FALSE;																// Exit function
	}
	return TRUE;															// All went well, continue on
}

int LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)	// Load an uncompressed TGA (note, much of this code is based on NeHe's
{																			// TGA Loading code nehe.gamedev.net)
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Read TGA header
	{
		//MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);		// Display error
		if(fTGA != NULL)													// if file is still open
		{
			fclose(fTGA);													// Close it
		}
		return FALSE;														// Return failular
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	texture->bpp	= tga.header[4];										// Determine the bits per pixel
	tga.Width		= texture->width;										// Copy width into local structure
	tga.Height		= texture->height;										// Copy height into local structure
	tga.Bpp			= texture->bpp;											// Copy BPP into local structure

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	// Make sure all information is valid
	{
		//MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	// Display Error
		if(fTGA != NULL)													// Check if file is still open
		{
			fclose(fTGA);													// If so, close it
		}
		return FALSE;														// Return failed
	}

	if(texture->bpp == 24)													// If the BPP of the image is 24...
		texture->type	= GL_RGB;											// Set Image type to GL_RGB
	else																	// Else if its 32 BPP
		texture->type	= GL_RGBA;											// Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute the total amout ofmemory needed to store data
	texture->imageData	= (GLubyte *)malloc(tga.imageSize);					// Allocate that much memory

	if(texture->imageData == NULL)											// If no space was allocated
	{
		//MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	// Display Error
		fclose(fTGA);														// Close the file
		return FALSE;														// Return failed
	}

	if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	// Attempt to read image data
	{
		//MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// Display Error
		if(texture->imageData != NULL)										// If imagedata has data in it
		{
			free(texture->imageData);										// Delete data from memory
		}
		fclose(fTGA);														// Close file
		return FALSE;														// Return failed
	}

	// Byte Swapping Optimized By Steve Thomas
	GLuint cswap;
	for( cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
		texture->imageData[cswap] ^= texture->imageData[cswap+2];
	}

	fclose(fTGA);															// Close file
	return TRUE;															// Return success
}

int LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)		// Load COMPRESSED TGAs
{
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Attempt to read header
	{
		//MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);		// Display Error
		if(fTGA != NULL)													// If file is open
		{
			fclose(fTGA);													// Close it
		}
		return FALSE;														// Return failed
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	texture->bpp	= tga.header[4];										// Determine Bits Per Pixel
	tga.Width		= texture->width;										// Copy width to local structure
	tga.Height		= texture->height;										// Copy width to local structure
	tga.Bpp			= texture->bpp;											// Copy width to local structure

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	//Make sure all texture info is ok
	{
		//MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	// If it isnt...Display error
		if(fTGA != NULL)													// Check if file is open
		{
			fclose(fTGA);													// Ifit is, close it
		}
		return FALSE;														// Return failed
	}

	if(texture->bpp == 24)													// If the BPP of the image is 24...
		texture->type	= GL_RGB;											// Set Image type to GL_RGB
	else																	// Else if its 32 BPP
		texture->type	= GL_RGBA;											// Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
	texture->imageData	= (GLubyte *)malloc(tga.imageSize);					// Allocate that much memory

	if(texture->imageData == NULL)											// If it wasnt allocated correctly..
	{
		//MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	// Display Error
		fclose(fTGA);														// Close file
		return FALSE;														// Return failed
	}

	GLuint pixelcount	= tga.Height * tga.Width;							// Nuber of pixels in the image
	GLuint currentpixel	= 0;												// Current pixel being read
	GLuint currentbyte	= 0;												// Current byte
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			// Storage for 1 pixel

	do
	{
		GLubyte chunkheader = 0;											// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				// Read in the 1 byte header
		{
			//MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);	// Display Error
			if(fTGA != NULL)												// If file is open
			{
				fclose(fTGA);												// Close file
			}
			if(texture->imageData != NULL)									// If there is stored image data
			{
				free(texture->imageData);									// Delete image data
			}
			return FALSE;													// Return failed
		}

		if(chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																	// that follow the header
			chunkheader++;
			short counter;											// add 1 to get number of following color values
			for(counter = 0; counter < chunkheader; counter++)		// Read RAW color values
			{
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
				{
					//MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// IF we cant, display an error

					if(fTGA != NULL)													// See if file is open
					{
						fclose(fTGA);													// If so, close file
					}

					if(colorbuffer != NULL)												// See if colorbuffer has data in it
					{
						free(colorbuffer);												// If so, delete it
					}

					if(texture->imageData != NULL)										// See if there is stored Image data
					{
						free(texture->imageData);										// If so, delete it too
					}

					return FALSE;														// Return failed
				}
																						// write to memory
				texture->imageData[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												// if its a 32 bpp image
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
				}

				currentbyte += tga.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{
					//MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(texture->imageData != NULL)										// If there is Image data
					{
						free(texture->imageData);										// delete it
					}

					return FALSE;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
			{
				//MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);			// If attempt fails.. Display error (again)

				if(fTGA != NULL)														// If thereis a file open
				{
					fclose(fTGA);														// Close it
				}

				if(colorbuffer != NULL)													// If there is data in the colorbuffer
				{
					free(colorbuffer);													// delete it
				}

				if(texture->imageData != NULL)											// If thereis image data
				{
					free(texture->imageData);											// delete it
				}

				return FALSE;															// return failed
			}
            short counter;
			for( counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated
			{																			// by the header
				texture->imageData[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												// If TGA images is 32 bpp
				{
					texture->imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
				}

				currentbyte += tga.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{
					//MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(texture->imageData != NULL)										// If there is Image data
					{
						free(texture->imageData);										// delete it
					}

					return FALSE;														// Return failed
				}
			}
		}
	}

	while(currentpixel < pixelcount);													// Loop while there are still pixels left
	fclose(fTGA);																		// Close the file
	return TRUE;																		// return success
}
