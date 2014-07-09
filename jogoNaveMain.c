#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

//código de Nate Robins, que escreveu a GLUT Win32: http://www.xmission.com/~nate/glut.html
//carrega modelos a partir de arquivos OBJ.
#include "glm.h"
#include "GerenciadorGrafico.h"
#include "GerenciadorAudio.h"
#include "Colisao.h"
#include "Meteoro.h"
#include "Tiros.h"
#include "Explosao.h"
#include "Game.h"
#include "Nave.h"

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




/**
* Protótipos de funções.
*/
void inicializaObjetos(void);
void desenhaGameOver(void);
void atualizarEstados(void);
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
int LoadTGA(Texture * , char * );
int LoadUncompressedTGA(Texture * , char * , FILE * );
int LoadCompressedTGA(Texture * , char * , FILE * );
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
                        int explos = posicaoVaziaExplosoes(explosoes);
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
                int explos = posicaoVaziaExplosoes(explosoes);
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
	texturaAviao = carregaTexturaNave(texturaAviao,"data/aviao/textura_jato.tga");

	texturaMetoro=carregaTexturaMeteoro(texturaMetoro,"data/meteoro/meteoro_Sphere.tga");

	texturaTiro =carregaTexturaTiro(texturaTiro,"data/tiro/tiro_Cube.tga");

	texturaFundo =carregaTexturaFundo(texturaFundo,"data/fundo/fundo.tga");

	texturaExplosao =carregaTexturaExplosao(texturaExplosao,"data/explosao/explosao.tga");

	texturaGameOver = carregaTexturaGameOver(texturaGameOver,"data/gameover/gameover.tga");

	continuaJogoVerde = carregaTexturaJogo(continuaJogoVerde,"data/gameover/continuarVerde.tga");

	continuaJogoVermelho = carregaTexturaJogo(continuaJogoVermelho,"data/gameover/continuarVermelho.tga");

	fimJogoVerde = carregaTexturaFimJogo(fimJogoVerde,"data/gameover/fimJogoVerde.tga");

	fimJogoVermelho = carregaTexturaFimJogo(fimJogoVermelho,"data/gameover/fimJogoVermelho.tga");
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
        int tiro = posicaoVaziaTiros(tiros);//pega a primeira pos vazia.

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
                enviaMeteoro(meteoros,meteorosEnviar,colisaoMeteoroDefault);
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

