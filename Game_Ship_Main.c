/*
 * Solar 3D Técnicas de Programação
 * Esse programa é a raiz principal do jogo,
 * que realiza todas as chamadas das funções dos outros arquivos
 * para que o jogo funcione, assim como também draw objetos
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
#include "logger.h"

void desenhaGameOver(void);
void atualizarEstados(void);
void trataTeclas(void);
void special_key(int , int , int );
void special_key_unpressed(int, int , int );
void increase_difficulty_level(int );
void controls(unsigned char , int , int );
void desenhaHUD();
void draw();
void resizes(int , int);
void explosion_timer(int );
void timer(int );
void configura();
void reconfigura();
void game_over_special_key(int, int, int);
void game_over_normal_key(unsigned char , int, int);
extern void mixaudio(void *unused, Uint8 *stream, int len);

SDL_AudioSpec fmt;
game_item ship;

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
Texture ship_texture;
Texture meteor_texture;
Texture shot_texture;
Texture background_texture;
Texture explosion_texture;
Texture game_over_texture;
Texture game_continue_green_texture;
Texture game_continue_red_texture;
Texture game_over_green_texture;
Texture game_over_red_texture;

/* sinalize if keys were pushed */
int left_button_pressed = FALSE;
int right_button_pressed = FALSE;
int up_button_pressed = FALSE;
int down_button_pressed = FALSE;
int shooting = FALSE;

/* meteors numbers to send each wave */
int meteors_to_send;
/* if its on pause or not */
int game_paused = FALSE;
/* meteors will be created in this time lapse */
int time_until_next_meteors_round;
/* meteors numbers to hit the ship */
int remaining_lives;
unsigned int score; /* player’s score */
/* selected continue or game over, after lost the lives */
int game_over_selected;

/* background music */
Mix_Music *music;
/* shot's SFX sound */
Mix_Chunk *shot_sound;
/* explosion's SFX sound */
Mix_Chunk *explosion_sound;

/* Frequency of audio playback */
int audio_rate = 22050;
/* Format of the audio we're playing */
Uint16 audio_format = AUDIO_S16;

/* 2 channels = stereo */
int audio_channels = 2;
int audio_buffers = 4096;

/* Main function */
int main(int argc, char **argv)
{
	configura(argc,argv);
	glutMainLoop();
	return 0;
}

/* produces the game over screen */
void desenhaGameOver()
{
	glutSpecialFunc(NULL);
	glutKeyboardFunc(NULL);

	glutSpecialFunc(game_over_special_key);
	glutKeyboardFunc(game_over_normal_key);

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
	for (int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
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
			for (int m = 0; m < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; m++) {
				if (meteors[m].visible) {
					if (verificaColisao(shots[i].collision, meteors[m].collision)) {
						PlaySound(EXPLOSION_MODEL,explosion_sound);
						meteors[m].visible = FALSE;
						shots[i].visible = FALSE;

						int explos = posicaoVaziaExplosoes(explosions);
						score += SCORE_VALUE;

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
	for (int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (meteors[i].visible) {
			meteors[i].last_position.z = meteors[i].position.z;
			meteors[i].position.z += meteors[i].acceleration;
			atualizaCaixaColisao(&meteors[i]);
			if (verificaColisao(meteors[i].collision, ship.collision)) {
				PlaySound(EXPLOSION_MODEL, explosion_sound);
				meteors[i].visible = FALSE;
				remaining_lives--;
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
				if (remaining_lives <= 0) {
					ship.visible = FALSE;
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
void game_over_special_key(int pressed_key, int x, int y)
{
	switch (pressed_key) {
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
void game_over_normal_key(unsigned char pressed_key, int x, int y)
{
	switch(pressed_key) {
		case ' ':

		case 13: /* ENTER */
			if(game_over_selected == GAME_OVER) {
				exit(0);
				print_verbose_log("Game ended by exit button");
			}
			else {
				reconfigura();
				print_verbose_log("Restart the game");
			}
			glutPostRedisplay();
			break;
		case 27: /* ESC */
			exit(0);
			print_verbose_log("Game ended by esc key");
			break;
	}

}

/* draws text */
void DesenhaTexto(char *string, int position_x, int position_y)
{
		glPushMatrix();
			glRasterPos2f(ship.position.x-position_x, ship.position.y-position_y);

			while (*string) {
				 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string++);
			}
		glPopMatrix();
}

/* shoot fire and updates collision */
void fire_shot()
{
	PlaySound(SHOT_MODEL, shot_sound);
	shooting = FALSE;
	int shot = create_empty_shot_position(shots);

	if (shot >= 0) {
		shots[shot].position.x = ship.position.x - 1;
		shots[shot].position.y = ship.position.y + 0.9;
		shots[shot].position.z = ship.position.z;
		shots[shot].visible = TRUE;

		shots[shot].last_position.x = ship.position.x - 1;
		shots[shot].last_position.y = ship.position.y + 0.9;
		shots[shot].last_position.z = ship.position.z;

		for (int c = 0; c < 8; c++) {
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

/* treats keys */
void trataTeclas()
{
	ship.last_position.x = ship.position.x;
	ship.last_position.y = ship.position.y;
	if (ship.position.x > MINIMUN_X) {
		ship.position.x -= (ship.acceleration * left_button_pressed);
		glLoadIdentity();
		gluLookAt(ship.position.x,
				  ship.position.y + CAMERA_INCLINATION,
				  DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  ship.position.x,
				  ship.position.y,
				  ship.position.z,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (ship.position.x < MAXIMUM_X) {
		ship.position.x += (ship.acceleration * right_button_pressed);
		glLoadIdentity();
		gluLookAt(ship.position.x,
				  ship.position.y + CAMERA_INCLINATION,
				  DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  ship.position.x,
				  ship.position.y,
				  0,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (ship.position.y < MAXIMUM_Y) {

		ship.position.y += (ship.acceleration * up_button_pressed);
		glLoadIdentity();
		gluLookAt(ship.position.x,
				  ship.position.y + CAMERA_INCLINATION,DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  ship.position.x,
				  ship.position.y,
				  0,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	if (ship.position.y > MINIMUM_Y) {

		ship.position.y-= (ship.acceleration * down_button_pressed);
		glLoadIdentity();
		gluLookAt(ship.position.x,
				  ship.position.y + CAMERA_INCLINATION,
				  DISTANCE_CAMERA_IN_THE_AXIS_Z,
				  ship.position.x,
				  ship.position.y,
				  ship.position.z,
				  0,
				  1,
				  0);
	}
	else {
		/* nothing to do */
	}
	atualizaCaixaColisao(&ship);

	while (shooting) {
		fire_shot();
		print_verbose_log("Shot's fired");
	}

	/* asks the OpenGL to redraw the screen, the ship now have new coordinates */
	glutPostRedisplay();
}

/*
 * When pressing a key keep it state to below.
 * This way the ship moves while the key is pressed
 * a pressed_key estiver pressionada
 */
void special_key(int pressed_key, int x, int y)
{
	switch (pressed_key) {
		case GLUT_KEY_LEFT:
			left_button_pressed = TRUE;
			ship.rotation = 30;
			ship.rotation_in_z = 1;
			print_verbose_log("Ship is going to left");
			break;
		case GLUT_KEY_RIGHT:
			right_button_pressed = TRUE;
			ship.rotation = -30;
			ship.rotation_in_z = 1;
			print_verbose_log("Ship is going to right");
			break;
		case GLUT_KEY_UP:
			up_button_pressed = TRUE;
			print_verbose_log("Ship is going up");
			break;
		case GLUT_KEY_DOWN:
			down_button_pressed = TRUE;
			print_verbose_log("Ship is going down");
			break;
	}
}

/* When a key is released, keeps it state when released */
void special_key_unpressed(int pressed_key, int x, int y)
{
	switch (pressed_key) {
		case GLUT_KEY_LEFT:
			left_button_pressed = FALSE;
			ship.rotation = 0;
			ship.rotation_in_z = 0;
			break;
		case GLUT_KEY_RIGHT:
			right_button_pressed = FALSE;
			ship.rotation = 0;
			ship.rotation_in_z = 0;
			break;
		case GLUT_KEY_UP:
			up_button_pressed = FALSE;
			break;
		case GLUT_KEY_DOWN:
			down_button_pressed = FALSE;
			break;
	}
}

/* raises the difficult */
void increase_difficulty_level(int t)
{
	if (ship.visible) {
		if (!game_paused) {
			for (int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
				meteors[i].acceleration += METEOR_SPEED_VARIATION;
			}
			if (meteors_to_send < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR) {
				meteors_to_send++;
			}
			else {
				/* nothing to do */
			}
			time_until_next_meteors_round -= 10;
			glutTimerFunc(time_until_next_meteors_round, timer, 0);
			glutTimerFunc(10000, increase_difficulty_level, 0);
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
void controls(unsigned char pressed_key, int x, int y)
{
	switch (pressed_key){
		case 27:
			limpa(music);
			exit(0) ;
			break;
		case ' ':
			if ((!game_paused) && (ship.visible)) {
				shooting = TRUE;
			}
			else {
				/* nothing to do */
			}
			break;
		case 'p':
			if (game_paused) {
				game_paused = FALSE;
				glutSpecialFunc(special_key);
				glutSpecialUpFunc(special_key_unpressed);
				glutReshapeFunc(resizes);
				glutPostRedisplay();
				glutTimerFunc(time_until_next_meteors_round, timer, NEW_METEOR);
				glutTimerFunc(200, explosion_timer, 0);
				glutTimerFunc(10000, increase_difficulty_level, 0);
				print_verbose_log("Unpaused the game");
			}
			else {
				glutSpecialFunc(NULL);
				glutSpecialUpFunc(NULL);
				glutReshapeFunc(NULL);
				game_paused = TRUE;
				glutPostRedisplay();
				print_verbose_log("Paused the game");
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
	sprintf(texto, "Vida: %d", remaining_lives);
	DesenhaTexto(texto, 5, 5);
	char texto2[20];
	sprintf(texto2, "Pontuacao: %d", score);
	DesenhaTexto(texto2, 6, 6);
}

/* draws */
void draw()
{
	/* cleans the color and depth buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!game_paused) {
		if (ship.visible) {
			desenhaFundo(background_texture);
			glPushMatrix();
			glTranslatef(ship.position.x, ship.position.y, ship.position.z);
			glRotatef(ship.rotation, 0, 0, ship.rotation_in_z);
			desenhaModelo(SHIP_MODEL, ship_texture, ship.model);
			glPopMatrix();
			desenhaTiros(shots, shot_texture, shot);
			desenhameteors(meteors, meteor_texture, meteor);
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
		DesenhaTexto("PAUSE", 0, 0);
	}
	glutSwapBuffers();
}

/* redimensions */
void resizes(int larg, int alt)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, larg/alt, 0.5, 110);

	/* resets the MODELVIEW alterations */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(ship.position.x, ship.position.y + 20, 20, 0, 0, 0, 0, 1, 0);
	glViewport(0, 0, larg, alt);
	glEnable(GL_DEPTH_TEST);
}

/* explosion timer */
void explosion_timer(int t)
{
	if (ship.visible && !game_paused) {
		for (int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
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
		glutTimerFunc(200, explosion_timer, t);
	}
	else {
		/* nothing to do */
	}
}

/* timer to new meteor */
void timer(int t)
{
	if (ship.visible && !game_paused) {
		print_verbose_log("New meteor");
		if (t == NEW_METEOR) {
			enviameteor(meteors, meteors_to_send, dafault_meteor_collision);
			glutTimerFunc(time_until_next_meteors_round, timer, NEW_METEOR);
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
	meteors_to_send = 10;
	time_until_next_meteors_round = 2000;
	score = 0;
	remaining_lives = 3;
	game_over_selected = CONTINUE;

	inicializaObjetos(&ship,
					  &meteor,
					  &explosion,
					  &shot,
					  &dafault_meteor_collision,
					  meteors,
					  explosions,
					  &dafault_collision_shot,
					  shots);
	iniciaGlut(argc, argv);

	glutDisplayFunc(draw);
	glutKeyboardFunc(controls);
	glutSpecialFunc(special_key);
	glutSpecialUpFunc(special_key_unpressed);
	glutReshapeFunc(resizes);

	iniciaCamera();

	Texture *textures[] = {&ship_texture,
						   &meteor_texture,
						   &shot_texture,
						   &background_texture,
						   &explosion_texture,
						   &game_over_texture,
						   &game_continue_green_texture,
						   &game_continue_red_texture,
						   &game_over_green_texture,
						   &game_over_red_texture};
	configuratextures(textures);
	glutTimerFunc(1000, timer, NEW_METEOR);
	glutTimerFunc(500, explosion_timer, 0);
	glutTimerFunc(20000, increase_difficulty_level, 0);

	iniciaAudio(audio_rate, audio_format, audio_channels, audio_buffers);
	music = Mix_LoadMUS("data/musica/musica.ogg");
	shot_sound = Mix_LoadWAV("data/audio/tiro.wav");
	explosion_sound = Mix_LoadWAV("data/audio/explosao.wav");

	tocaMusica(music);
}

/* reconfigurs objects */
void reconfigura()
{
	remaining_lives = 3;
	inicializaObjetos(&ship,
					  &meteor,
					  &explosion,
					  &shot,
					  &dafault_meteor_collision,
					  meteors,
					  explosions,
					  &dafault_collision_shot,
					  shots);
	criaCaixaColisao(ship.model, &ship.collision);
	glClearColor(0, 0, 0, 0);
	glutKeyboardFunc(controls);
	glutSpecialFunc(special_key);
	glutSpecialUpFunc(special_key_unpressed);
	glutTimerFunc(1000, timer, NEW_METEOR);
	glutTimerFunc(500, explosion_timer, 0);
	glutTimerFunc(20000, increase_difficulty_level, 0);

	for (int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++ ) {
		explosions[i].visible = FALSE;
		meteors[i].visible = FALSE;
	}
}
