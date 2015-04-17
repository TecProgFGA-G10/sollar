/*
* Solar 3D Técnicas de Programação
* Esse programa é a raiz principal do jogo,
* que realiza todas as chamadas das funções dos outros arquivos
* para que o jogo funcione, assim como também desenha objetos 
* e menus do jogo, configura e reconfigura objetos, calcula tempo 
* e atualiza os estados
*/


#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "glm.h"
#include "Display_Manager.h"
#include "Audio_Manager.h"
#include "Collision.h"
#include "Meteor.h"
#include "Shots.h"
#include "Explosion.h"
#include "Game.h"
#include "Ship.h"

void desenhaGameOver(void);
void atualizarEstados(void);
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
void reconfigura();
void teclaGameOverEspecial(int, int, int);
void teclaGameOverNormal(unsigned char , int, int);
extern void mixaudio(void *unused, Uint8 *stream, int len);

SDL_AudioSpec fmt;
game_item nave;

/* the shot's and planes' models are loaded only 1 time */
GLMmodel *meteoro = NULL;
GLMmodel *tiro = NULL;
GLMmodel *explosao = NULL;

/* vector with shots, meteors and explosions wich can be summoned */
game_item meteoros[NUM_MAX_METEOROS];
game_item tiros[NUM_MAX_TIROS];
/* one explosion per meteor at maximum */
game_item explosions[NUM_MAX_METEOROS];

/* 
* collision boxes to meteors and shots, when in position <0,0,0>
* used to recalculate the new position and send a new object
*/
collision_box colisaoMeteoroDefault;
collision_box colisaoTiroDefault;

/* textures */
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

/* sinalize if keys were pushed */
int esquerdaPressionada = FALSE;
int direitaPressionada = FALSE;
int cimaPressionada = FALSE;
int baixoPressionada = FALSE;
int atirar = FALSE;

/* meteors numbers to send each wave */
int meteorosEnviar;
/* if its on pause or not */
int pausado = FALSE;
/* meteors will be created in this time lapse */
int tempoMeteoro;
/* meteors numbers to hit the ship */
int vida;
/* pontuation */
unsigned int pontos;
/* selected continue or game over, after lost the lives */
int gameOverSelecionado;

/* background music */
Mix_Music *musica;
/* shot's SFX sound */
Mix_Chunk *somTiro;
/* explosion's SFX sound */
Mix_Chunk *somExplosao;

/* Frequency of audio playback */
int audio_rate = 22050;
/* Format of the audio we're playing */
Uint16 audio_format = AUDIO_S16;

/* 2 channels = stereo */
int audio_channels = 2;
int audio_buffers = 4096;


int main(int argc, char **argv)
{
	configura(argc,argv);
	glutMainLoop();
	return 0;
}

void desenhaGameOver()
{
	glutSpecialFunc(NULL);
	glutKeyboardFunc(NULL);

	glutSpecialFunc(teclaGameOverEspecial);
	glutKeyboardFunc(teclaGameOverNormal);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( 0, 0, 14,
			   0, 0, 0 ,
			   0, 1, 0);
	/* draws background and two buttons to continue or leave the game */
	desenhaPlanoGameOver(texturaGameOver);
	/* continue button */
	desenhaBotaoContinuar(gameOverSelecionado,
						  continuaJogoVerde,
						  continuaJogoVermelho);
	/* leave button */
	desenhaBotaoSair(gameOverSelecionado,
					 fimJogoVerde,
					 fimJogoVermelho);
}

/* updates the state */
void atualizarEstados(void)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		if (tiros[i].visivel) {
			tiros[i].posicaoAnterior.z = tiros[i].position.z;
			tiros[i].position.z += tiros[i].aceleracao;
			atualizaCaixaColisaoTiro(&tiros[i]);
			/* 
			* Shot will move at Z axis, goint to the
			* negative value of it. When z value is less then the maximum, 
			* the shot will dissapear.
			*/
			if (tiros[i].position.z < MAXIMO_DESENHO_TIRO) {
				tiros[i].visivel = FALSE;
			}
			else {
				/* nothing to do */
			}
			int m;
			for (m = 0; m < NUM_MAX_METEOROS; m++) {
				if (meteoros[m].visivel) {
					if (verificaColisao(tiros[i].colisao, meteoros[m].colisao)) {
						PlaySound(MODELO_EXPLOSAO,somExplosao);
						meteoros[m].visivel = FALSE;
						tiros[i].visivel = FALSE;
						int explos = posicaoVaziaExplosoes(explosions);
						pontos += VALOR_PONTO;
						if (explos >= 0) {
							explosoes[explos].position.x = meteoros[m].position.x;
							explosoes[explos].position.y = meteoros[m].position.y;
							explosoes[explos].position.z = meteoros[m].position.z;
							explosoes[explos].tamanho = 1;
							explosoes[explos].visivel = TRUE;
						}
						else {
							/* nothing to do */
						}
					}
					else {
						/* nothing to do */
					}
				}
				else {
					/* nothing to do */
				}
			}
		}
		else {
			/* nothing to do */
		}
	}
	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		if (meteoros[i].visivel) {
			meteoros[i].posicaoAnterior.z = meteoros[i].position.z;
			meteoros[i].position.z += meteoros[i].aceleracao;
			atualizaCaixaColisao(&meteoros[i]);
			if (verificaColisao(meteoros[i].colisao, nave.colisao)) {
				PlaySound(MODELO_EXPLOSAO,somExplosao);
				meteoros[i].visivel = FALSE;
				vida--;
				int explos = posicaoVaziaExplosoes(explosions);
				if (explos >= 0) {
					explosoes[explos].position.x = meteoros[i].position.x;
					explosoes[explos].position.y = meteoros[i].position.y;
					explosoes[explos].position.z = meteoros[i].position.z;
					explosoes[explos].tamanho = 1;
					explosoes[explos].visivel = TRUE;
				}
				else {
					/* nothing to do */
				}
				if (vida < 0) {
					nave.visivel = FALSE;
				}
				else {
					/* nothing to do */
				}
			}
			else {
				/* nothing to do */				
			}
			if (meteoros[i].position.z > MAXIMO_DESENHO_METEORO) {
				meteoros[i].visivel = FALSE;
			}
			else {
				/* nothing to do */
			}
		}
		else {
			/* nothing to do */
		}
	}
}

/*
 * Related to the game over keys, to especial keys LEFT and RIGHT.
 * When pressing one of these, selects if you want to continue or not.
 */
void teclaGameOverEspecial(int tecla, int x, int y)
{
	switch (tecla) {
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
		{
			if(gameOverSelecionado == CONTINUAR) {
				gameOverSelecionado = GAME_OVER;
			}
			else {
				gameOverSelecionado = CONTINUAR;
			}
			glutPostRedisplay();
			break;
		}
	}

}

/* normal game over key */
void teclaGameOverNormal(unsigned char tecla, int x, int y)
{
	switch(tecla) {
		case ' ':

		case 13: /* ENTER */
			if(gameOverSelecionado == GAME_OVER) {
				exit(0);
			}
			else {
				reconfigura();
			}
			glutPostRedisplay();
			break;
		case 27: /* ESC */
			exit(0);
			break;
	}

}

/* draws text */
void DesenhaTexto(char *string, int posx, int posy)
{
		glPushMatrix();
			glRasterPos2f(nave.position.x-posx,nave.position.y-posy);

			while (*string) {
				 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++);
			}
		glPopMatrix();
}

/* treats keys */
void trataTeclas()
{
	nave.posicaoAnterior.x = nave.position.x;
	nave.posicaoAnterior.y = nave.position.y;
	if (nave.position.x > MINIMO_X) {
		nave.position.x -= (nave.aceleracao * esquerdaPressionada);
		glLoadIdentity();
		gluLookAt(nave.position.x,
				  nave.position.y+VAR_CAMERA,
				  CAMERA_Z,
				  nave.position.x,
				  nave.position.y,
				  nave.position.z,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (nave.position.x < MAXIMO_X) {
		nave.position.x += (nave.aceleracao * direitaPressionada);
		glLoadIdentity();
		gluLookAt(nave.position.x,
				  nave.position.y+VAR_CAMERA,
				  CAMERA_Z,
				  nave.position.x,
				  nave.position.y,
				  0,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (nave.position.y < MAXIMO_Y) {

		nave.position.y += (nave.aceleracao * cimaPressionada);
		glLoadIdentity();
		gluLookAt(nave.position.x,
				  nave.position.y + VAR_CAMERA,CAMERA_Z,
				  nave.position.x,
				  nave.position.y,
				  0,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (nave.position.y > MINIMO_Y) {

		nave.position.y-= (nave.aceleracao * baixoPressionada);
		glLoadIdentity();
		gluLookAt(nave.position.x,
				  nave.position.y+VAR_CAMERA,
				  CAMERA_Z,
				  nave.position.x,
				  nave.position.y,
				  nave.position.z ,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	atualizaCaixaColisao(&nave);

	if (atirar) {
		PlaySound(MODELO_TIRO, somTiro);
		atirar = FALSE;
		int tiro = posicaoVaziaTiros(tiros);

		if (tiro >= 0) {
			tiros[tiro].position.x = nave.position.x - 1;
			tiros[tiro].position.y = nave.position.y + 0.9;
			tiros[tiro].position.z = nave.position.z;
			tiros[tiro].visivel = TRUE;

			tiros[tiro].posicaoAnterior.x = nave.position.x - 1;
			tiros[tiro].posicaoAnterior.y = nave.position.y + 0.9;
			tiros[tiro].posicaoAnterior.z = nave.position.z;

			int c = 0;

			for (c = 0; c < 8; c++) {
				tiros[tiro].colisao.pontos[c].x = colisaoTiroDefault.pontos[c].x;
				tiros[tiro].colisao.pontos[c].y = colisaoTiroDefault.pontos[c].y;
				tiros[tiro].colisao.pontos[c].z = colisaoTiroDefault.pontos[c].z;
			}
			setaCaixaColisao(&tiros[tiro].colisao, tiros[tiro].position);
		}
		else {
			/* nothing to do */
		}
	}
	else {
		/* nothing to do */
	}

	/* asks the OpenGL to redraw the screen, the ship now have new coordinates */
	glutPostRedisplay();
}

/* 
* When pressing a key keep it state to below. 
* This way the ship moves while the key is pressed
* a tecla estiver pressionada 
*/
void teclaEspecial(int tecla, int x, int y)
{
	switch (tecla) {
		case GLUT_KEY_LEFT:
			esquerdaPressionada = TRUE;
			nave.rotacao = 30;
			nave.rotZ = 1;
			break;
		case GLUT_KEY_RIGHT:
			direitaPressionada = TRUE;
			nave.rotacao = -30;
			nave.rotZ = 1;
			break;
		case GLUT_KEY_UP:
			cimaPressionada = TRUE;
			break;
		case GLUT_KEY_DOWN:
			baixoPressionada = TRUE;
			break;
	}
}

/* When a key is released, keeps it state when released */
void teclaEspecialSolta(int tecla, int x, int y)
{
	switch (tecla) {
		case GLUT_KEY_LEFT:
			esquerdaPressionada = FALSE;
			nave.rotacao = 0;
			nave.rotZ = 0;
			break;
		case GLUT_KEY_RIGHT:
			direitaPressionada = FALSE;
			nave.rotacao = 0;
			nave.rotZ = 0;
			break;
		case GLUT_KEY_UP:
			cimaPressionada = FALSE;
			break;
		case GLUT_KEY_DOWN:
			baixoPressionada = FALSE;
			break;
	}
}

/* raises the difficult */
void aumentaDificuldade(int t)
{
	if (nave.visivel) {
		if (!pausado) {
			int i = 0;
			for (i = 0; i < NUM_MAX_METEOROS; i++) {
				meteoros[i].aceleracao += AUMENTO_VELOCIDADE_METEORO;
			}
			if (meteorosEnviar < NUM_MAX_METEOROS) {
				meteorosEnviar++;
			}
			else {
				/* nothing to do */
			}
			tempoMeteoro -= 10;
			glutTimerFunc(tempoMeteoro, timer, 0);
			glutTimerFunc(10000, aumentaDificuldade, 0);
		}
		else {
			/* nothing to do */
		}
	}
	else {
		/* nothing to do */
	}
}

/* controls */
void controla(unsigned char tecla, int x, int y)
{
	switch (tecla){
		case 27:
			limpa(musica);
			exit(0) ;
			break;
		case ' ':
			if ((!pausado) && (nave.visivel)) {
				atirar = TRUE;
			}
			else {
				/* nothing to do */
			}
			break;
		case 'p':
			if (pausado) {
				pausado = FALSE;
				glutSpecialFunc(teclaEspecial);
				glutSpecialUpFunc(teclaEspecialSolta);
				glutReshapeFunc(redimensiona);
				glutPostRedisplay();
				glutTimerFunc(tempoMeteoro, timer, NOVO_METEORO);
				glutTimerFunc(200, timerExplosao, 0);
				glutTimerFunc(10000, aumentaDificuldade, 0);
			}
			else {
				glutSpecialFunc(NULL);
				glutSpecialUpFunc(NULL);
				glutReshapeFunc(NULL);
				pausado = TRUE;
				glutPostRedisplay();
			}
			break;
		default:
			break;
	}
}

/* draws HUD */
void desenhaHUD()
{
	char texto[20];
	sprintf(texto, "Vida: %d", vida);
	DesenhaTexto(texto, 5, 5);
	char texto2[20];
	sprintf(texto2, "Pontuacao: %d", pontos);
	DesenhaTexto(texto2, 6, 6);
}

/* draws */
void desenha()
{
	/* cleans the color and depth buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!pausado) {
		if (nave.visivel) {
			desenhaFundo(texturaFundo);
			glPushMatrix();
				glTranslatef(nave.position.x, nave.position.y, nave.position.z);
				glRotatef(nave.rotacao, 0, 0, nave.rotZ);
				desenhaModelo(MODELO_NAVE, texturaAviao, nave.modelo);
			glPopMatrix();
			desenhaTiros(tiros, texturaTiro, tiro);
			desenhaMeteoros(meteoros, texturaMetoro, meteoro);
			desenhaExplosoes(explosions, texturaExplosao, explosao);
			desenhaHUD();
		}
		else {
			desenhaGameOver();
		}
		atualizarEstados();
		trataTeclas();
	}
	else {
		DesenhaTexto("PAUSE",  5,5);
	}
	glutSwapBuffers();
}

/* redimensions */
void redimensiona(int larg, int alt)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, larg/alt, 0.5, 110);

	/* resets the MODELVIEW alterations */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(nave.position.x, nave.position.y + 20, 20,
			  0, 0, 0,
			  0, 1, 0);
	glViewport(0, 0, larg,alt);
	glEnable(GL_DEPTH_TEST);
}

/* explosion timer */
void timerExplosao(int t)
{
	if (nave.visivel) {
		if (!pausado) {
			int i;
			for (i = 0; i < NUM_MAX_METEOROS; i++) {
				if (explosions[i].visivel) {
					explosions[i].tamanho -= EXPLOSAO_DECRESCIMENTO;
					if (explosions[i].tamanho <= 0.3) {
						explosions[i].visivel = FALSE;
					}
					else {
						/* nothing to do */
					}
				}
				else {
					/* nothing to do */
				}
			}
			glutTimerFunc(200, timerExplosao, t);
		}
		else {
			/* nothing to do */
		}
	}
	else {
		/* nothing to do */
	}
}

/* timer to new meteor */
void timer(int t)
{
	if (nave.visivel) {
		if (!pausado) {
			printf("novo meteoro");
			if (t == NOVO_METEORO) {
				enviaMeteoro(meteoros, meteorosEnviar, colisaoMeteoroDefault);
				glutTimerFunc(tempoMeteoro, timer, NOVO_METEORO);
			}
			else {
				/* nothing to do */
			}
		}
		else {
			/* nothing to do */
		}
	}
	else {
		/* nothing to do */
	}
}

/* configurs meteors */
void configura(int argc, char **argv)
{
	meteorosEnviar = 10;
	tempoMeteoro = 2000;
	pontos = 0;
	vida = 3;
	gameOverSelecionado = CONTINUAR;

	inicializaObjetos(&nave,
					  &meteoro,
					  &explosao,
					  &tiro,
					  &colisaoMeteoroDefault,
					  meteoros,
					  explosions,
					  &colisaoTiroDefault,
					  tiros);
	iniciaGlut(argc, argv);

	glutDisplayFunc(desenha);
	glutKeyboardFunc(controla);
	glutSpecialFunc(teclaEspecial);
	glutSpecialUpFunc(teclaEspecialSolta);
	glutReshapeFunc(redimensiona);

	iniciaCamera();

	Texture *texturas[] = {&texturaAviao,
						   &texturaMetoro,
						   &texturaTiro,
						   &texturaFundo,
						   &texturaExplosao,
						   &texturaGameOver,
						   &continuaJogoVerde,
						   &continuaJogoVermelho,
						   &fimJogoVerde,
						   &fimJogoVermelho};
	configuraTexturas(texturas);
	glutTimerFunc(1000, timer, NOVO_METEORO);
	glutTimerFunc(500, timerExplosao, 0);
	glutTimerFunc(20000, aumentaDificuldade, 0);

	iniciaAudio(audio_rate, audio_format, audio_channels, audio_buffers);
	musica = Mix_LoadMUS("data/musica/musica.ogg");
	somTiro = Mix_LoadWAV("data/audio/tiro.wav");
	somExplosao = Mix_LoadWAV("data/audio/explosao.wav");

	tocaMusica(musica);
}

/* reconfigurs objects */
void reconfigura()
{
	inicializaObjetos(&nave,
					  &meteoro,
					  &explosao,
					  &tiro,
					  &colisaoMeteoroDefault,
					  meteoros,
					  explosions,
					  &colisaoTiroDefault,
					  tiros);
	criaCaixaColisao(nave.modelo, &nave.colisao);
	glClearColor(0, 0, 0, 0);
	glutKeyboardFunc(controla);
	glutSpecialFunc(teclaEspecial);
	glutSpecialUpFunc(teclaEspecialSolta);
	glutTimerFunc(1000, timer, NOVO_METEORO);
	glutTimerFunc(500, timerExplosao, 0);
	glutTimerFunc(20000, aumentaDificuldade, 0);
	int i;
	for (i = 0; i < NUM_MAX_METEOROS; i++ ) {
		explosions[i].visivel = FALSE;
		meteoros[i].visivel = FALSE;
	}
}
