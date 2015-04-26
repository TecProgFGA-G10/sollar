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
game_item aircraft;

/* the shot's and planes' models are loaded only 1 time */
GLMmodel *meteor = NULL;
GLMmodel *shot = NULL;
GLMmodel *explosion = NULL;

/* vector with shots, meteors and explosions wich can be summoned */
game_item meteors[MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR];
game_item shots[MAX_NUMBER_OF_SHOTS];
/* one explosion per meteor at maximum */
game_item explosions[MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR];

/*
* collision boxes to meteors and shots, when in position <0,0,0>
* used to recalculate the new position and send a new object
*/
collision_box dafault_meteor_collision;
collision_box dafault_collision_shot;

/* textures */
Texture texturaAviao;
Texture texturaMetoro;
Texture texturaTiro;
Texture background_texture;
Texture explosion_texture;
Texture game_over_texture;
Texture game_continue_green_texture;
Texture game_continue_red_texture;
Texture game_over_green_texture;
Texture game_over_red_texture;

/* sinalize if keys were pushed */
int esquerdaPressionada = FALSE;
int direitaPressionada = FALSE;
int cimaPressionada = FALSE;
int baixoPressionada = FALSE;
int atirar = FALSE;

/* meteors numbers to send each wave */
int meteorsEnviar;
/* if its on pause or not */
int pausado = FALSE;
/* meteors will be created in this time lapse */
int tempometeor;
/* meteors numbers to hit the ship */
int vida;
unsigned int pontuation; /* player’s pontuation */
/* selected continue or game over, after lost the lives */
int game_over_selected;

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
	desenhaPlanoGameOver(game_over_texture);
	/* continue button */
	desenhaBotaoContinuar(game_over_selected,
						  game_continue_green_texture,
						  game_continue_red_texture);
	/* leave button */
	desenhaBotaoSair(game_over_selected,
					 game_over_green_texture,
					 game_over_red_texture);
}

/* updates the state */
void atualizarEstados(void)
{
	int i = 0;
	for (i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
		if (shots[i].visible) {
			shots[i].last_position.z = shots[i].position.z;
			shots[i].position.z += shots[i].acceleration;
			atualizaCaixaColisaoTiro(&shots[i]);
			/*
			* Shot will move at Z axis, goint to the
			* negative value of it. When z value is less then the maximum,
			* the shot will dissapear.
			*/
			if (shots[i].position.z < MAX_DISTANCE_FOR_SHOOT_TREATMENT) {
				shots[i].visible = FALSE;
			}
			else {
				/* nothing to do */
			}
			int m;
			for (m = 0; m < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; m++) {
				if (meteors[m].visible) {
					if (verificaColisao(shots[i].collision, meteors[m].collision)) {
						PlaySound(EXPLOSION_MODEL,somExplosao);
						meteors[m].visible = FALSE;
						shots[i].visible = FALSE;

						int explos = posicaoVaziaExplosoes(explosions);
						pontuation += SCORE_VALUE;

						if (explos >= 0) {
							explosions[explos].position.x = meteors[m].position.x;
							explosions[explos].position.y = meteors[m].position.y;
							explosions[explos].position.z = meteors[m].position.z;
							explosions[explos].explosion_size = 1;
							explosions[explos].visible = TRUE;
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
	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (meteors[i].visible) {
			meteors[i].last_position.z = meteors[i].position.z;
			meteors[i].position.z += meteors[i].acceleration;
			atualizaCaixaColisao(&meteors[i]);
			if (verificaColisao(meteors[i].collision, aircraft.collision)) {
				PlaySound(EXPLOSION_MODEL,somExplosao);
				meteors[i].visible = FALSE;
				vida--;
				int explos = posicaoVaziaExplosoes(explosions);
				if (explos >= 0) {
					explosions[explos].position.x = meteors[i].position.x;
					explosions[explos].position.y = meteors[i].position.y;
					explosions[explos].position.z = meteors[i].position.z;
					explosions[explos].explosion_size = 1;
					explosions[explos].visible = TRUE;
				}
				else {
					/* nothing to do */
				}
				if (vida < 0) {
					aircraft.visible = FALSE;
				}
				else {
					/* nothing to do */
				}
			}
			else {
				/* nothing to do */
			}
			if (meteors[i].position.z > MAX_DISTANCE_OF_METEOR) {
				meteors[i].visible = FALSE;
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
			if(game_over_selected == CONTINUE) {
				game_over_selected = GAME_OVER;
			}
			else {
				game_over_selected = CONTINUE;
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
			if(game_over_selected == GAME_OVER) {
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
			glRasterPos2f(aircraft.position.x-posx,aircraft.position.y-posy);

			while (*string) {
				 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++);
			}
		glPopMatrix();
}

/* treats keys */
void trataTeclas()
{
	aircraft.last_position.x = aircraft.position.x;
	aircraft.last_position.y = aircraft.position.y;
	if (aircraft.position.x > MINIMUN_X) {
		aircraft.position.x -= (aircraft.acceleration * esquerdaPressionada);
		glLoadIdentity();
		gluLookAt(aircraft.position.x,
				  aircraft.position.y+CAMERA_INCLINATION,
				  DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  aircraft.position.x,
				  aircraft.position.y,
				  aircraft.position.z,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (aircraft.position.x < MAXIMUM_X) {
		aircraft.position.x += (aircraft.acceleration * direitaPressionada);
		glLoadIdentity();
		gluLookAt(aircraft.position.x,
				  aircraft.position.y+CAMERA_INCLINATION,
				  DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  aircraft.position.x,
				  aircraft.position.y,
				  0,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (aircraft.position.y < MAXIMUM_Y) {

		aircraft.position.y += (aircraft.acceleration * cimaPressionada);
		glLoadIdentity();
		gluLookAt(aircraft.position.x,
				  aircraft.position.y + CAMERA_INCLINATION,DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  aircraft.position.x,
				  aircraft.position.y,
				  0,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (aircraft.position.y > MINIMUM_Y) {

		aircraft.position.y-= (aircraft.acceleration * baixoPressionada);
		glLoadIdentity();
		gluLookAt(aircraft.position.x,
				  aircraft.position.y+CAMERA_INCLINATION,
				  DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  aircraft.position.x,
				  aircraft.position.y,
				  aircraft.position.z ,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	atualizaCaixaColisao(&aircraft);

	if (atirar) {
		PlaySound(SHOT_MODEL, somTiro);
		atirar = FALSE;
		int shot = posicaoVaziaTiros(shots);

		if (shot >= 0) {
			shots[shot].position.x = aircraft.position.x - 1;
			shots[shot].position.y = aircraft.position.y + 0.9;
			shots[shot].position.z = aircraft.position.z;
			shots[shot].visible = TRUE;

			shots[shot].last_position.x = aircraft.position.x - 1;
			shots[shot].last_position.y = aircraft.position.y + 0.9;
			shots[shot].last_position.z = aircraft.position.z;

			int c = 0;

			for (c = 0; c < 8; c++) {
				shots[shot].collision.points[c].x = dafault_collision_shot.points[c].x;
				shots[shot].collision.points[c].y = dafault_collision_shot.points[c].y;
				shots[shot].collision.points[c].z = dafault_collision_shot.points[c].z;
			}
			setaCaixaColisao(&shots[shot].collision, shots[shot].position);
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
			aircraft.rotation = 30;
			aircraft.rotZ = 1;
			break;
		case GLUT_KEY_RIGHT:
			direitaPressionada = TRUE;
			aircraft.rotation = -30;
			aircraft.rotZ = 1;
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
			aircraft.rotation = 0;
			aircraft.rotZ = 0;
			break;
		case GLUT_KEY_RIGHT:
			direitaPressionada = FALSE;
			aircraft.rotation = 0;
			aircraft.rotZ = 0;
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
	if (aircraft.visible) {
		if (!pausado) {
			int i = 0;
			for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
				meteors[i].acceleration += METEOR_SPEED_VARIATION;
			}
			if (meteorsEnviar < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR) {
				meteorsEnviar++;
			}
			else {
				/* nothing to do */
			}
			tempometeor -= 10;
			glutTimerFunc(tempometeor, timer, 0);
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
			if ((!pausado) && (aircraft.visible)) {
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
				glutTimerFunc(tempometeor, timer, NEW_METEOR);
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
	sprintf(texto2, "Pontuacao: %d", pontuation);
	DesenhaTexto(texto2, 6, 6);
}

/* draws */
void desenha()
{
	/* cleans the color and depth buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!pausado) {
		if (aircraft.visible) {
			desenhaFundo(background_texture);
			glPushMatrix();
				glTranslatef(aircraft.position.x, aircraft.position.y, aircraft.position.z);
				glRotatef(aircraft.rotation, 0, 0, aircraft.rotZ);
				desenhaModelo(AIRCRAFT_MODEL, texturaAviao, aircraft.model);
			glPopMatrix();
			desenhaTiros(shots, texturaTiro, shot);
			desenhameteors(meteors, texturaMetoro, meteor);
			desenhaExplosoes(explosions, explosion_texture, explosion);
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
	gluLookAt(aircraft.position.x, aircraft.position.y + 20, 20,
			  0, 0, 0,
			  0, 1, 0);
	glViewport(0, 0, larg,alt);
	glEnable(GL_DEPTH_TEST);
}

/* explosion timer */
void timerExplosao(int t)
{
	if (aircraft.visible) {
		if (!pausado) {
			int i;
			for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
				if (explosions[i].visible) {
					explosions[i].explosion_size -= EXPLOSION_INCREASE_RATE;
					if (explosions[i].explosion_size <= 0.3) {
						explosions[i].visible = FALSE;
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
	if (aircraft.visible) {
		if (!pausado) {
			printf("novo meteor");
			if (t == NEW_METEOR) {
				enviameteor(meteors, meteorsEnviar, dafault_meteor_collision);
				glutTimerFunc(tempometeor, timer, NEW_METEOR);
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
	meteorsEnviar = 10;
	tempometeor = 2000;
	pontuation = 0;
	vida = 3;
	game_over_selected = CONTINUE;

	inicializaObjetos(&aircraft,
					  &meteor,
					  &explosion,
					  &shot,
					  &dafault_meteor_collision,
					  meteors,
					  explosions,
					  &dafault_collision_shot,
					  shots);
	iniciaGlut(argc, argv);

	glutDisplayFunc(desenha);
	glutKeyboardFunc(controla);
	glutSpecialFunc(teclaEspecial);
	glutSpecialUpFunc(teclaEspecialSolta);
	glutReshapeFunc(redimensiona);

	iniciaCamera();

	Texture *textures[] = {&texturaAviao,
						   &texturaMetoro,
						   &texturaTiro,
						   &background_texture,
						   &explosion_texture,
						   &game_over_texture,
						   &game_continue_green_texture,
						   &game_continue_red_texture,
						   &game_over_green_texture,
						   &game_over_red_texture};
	configuratextures(textures);
	glutTimerFunc(1000, timer, NEW_METEOR);
	glutTimerFunc(500, timerExplosao, 0);
	glutTimerFunc(20000, aumentaDificuldade, 0);

	iniciaAudio(audio_rate, audio_format, audio_channels, audio_buffers);
	musica = Mix_LoadMUS("data/musica/musica.ogg");
	somTiro = Mix_LoadWAV("data/audio/shot.wav");
	somExplosao = Mix_LoadWAV("data/audio/explosion.wav");

	tocaMusica(musica);
}

/* reconfigurs objects */
void reconfigura()
{
	inicializaObjetos(&aircraft,
					  &meteor,
					  &explosion,
					  &shot,
					  &dafault_meteor_collision,
					  meteors,
					  explosions,
					  &dafault_collision_shot,
					  shots);
	criaCaixaColisao(aircraft.model, &aircraft.collision);
	glClearColor(0, 0, 0, 0);
	glutKeyboardFunc(controla);
	glutSpecialFunc(teclaEspecial);
	glutSpecialUpFunc(teclaEspecialSolta);
	glutTimerFunc(1000, timer, NEW_METEOR);
	glutTimerFunc(500, timerExplosao, 0);
	glutTimerFunc(20000, aumentaDificuldade, 0);
	int i;
	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++ ) {
		explosions[i].visible = FALSE;
		meteors[i].visible = FALSE;
	}
}
