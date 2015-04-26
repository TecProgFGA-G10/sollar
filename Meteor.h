/*
 * Solar 3D Técnicas de Programação
 * Esse programa carrega o meteoro, sua textura e
 * envia para a área do jogo, configurando a área ocupada e
 * desenhando o meteoro na área configurando também sua
 * aceleração
 */

#ifndef meteor_H
#define meteor_H
#include <GL/glut.h>
#include "Collision.h"

/* loads the meteor model */
void carregaModelometeor(GLMmodel **meteor)
{
	if (*meteor == NULL) {
		*meteor = glmReadOBJ("data/meteoro/meteoro.obj");
		if (*meteor == NULL){
			printf("\n\nErro carregando meteoro.obj");
			exit(0);
		}
		else {
		  /* nothing to do */
		}
		glmUnitize(*meteor); /* redimensions to unity matrix */
		glmFacetNormals(*meteor);
		glmVertexNormals(*meteor, 90.0);
	}
	else {
		printf("meteor nao carregado! \n");
	}
}

/* creates an empty meteor position */
int posicaoVaziameteors(game_item *meteors)
{
	int i = 0;
	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (!meteors[i].visible) {
			return i;
		}
		else {
			/* nothing to do */
		}
	}
	return -1;
}

/* send meteor to the screen */
void enviameteor(game_item *meteors,
				  int meteorsEnviar,
				  collision_box dafault_meteor_collision)
{
	int i = 0;

	for (i = 0; i < meteorsEnviar; i++) {
		int pos = posicaoVaziameteors(meteors);

		if (pos >= 0) {
			meteors[pos].visible = TRUE;
			meteors[pos].position.z = (INICIAL_POSITION_OF_METEOR_IN_THE_AXIS_Z-10) -
									   rand()%(INICIAL_POSITION_OF_METEOR_IN_THE_AXIS_Z -
									   -20);
			meteors[pos].position.x = MINIMUN_X + rand() % (MAXIMUM_X-MINIMUN_X);
			meteors[pos].position.y = MINIMUM_Y + rand() % (MAXIMUM_Y-MINIMUM_Y);
			/*
			 * Keep the last position to calculate the collision box
			 * to the meteor in a different position of <0, 0, 0>
			 */
			meteors[pos].last_position.z = meteors[pos].position.z;
			meteors[pos].last_position.x = meteors[pos].position.x;
			meteors[pos].last_position.y = meteors[pos].position.y;

			int c = 0;
			/* fix me. I am out of the bounds*/
			for (c = 0; c < 8; c++){
				meteors[pos].collision.points[c].x = dafault_meteor_collision.points[c].x;
				meteors[pos].collision.points[c].y = dafault_meteor_collision.points[c].y;
				meteors[pos].collision.points[c].z = dafault_meteor_collision.points[c].z;
			}
			setaCaixaColisao(&meteors[pos].collision, meteors[pos].position);
		}
		else {
			/* nothing to do */
		}
	}
}

/* loads the meteor texture */
void carregaTexturameteor(Texture *texturaMetoro, char *filePath)
{
	if (LoadTGA(texturaMetoro, filePath)) {
		glGenTextures(1, &texturaMetoro->texID);
		glBindTexture(GL_TEXTURE_2D, texturaMetoro->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 texturaMetoro->bpp / 8,
					 texturaMetoro->width,
					 texturaMetoro->height,
					 0,
					 texturaMetoro->type,
					 GL_UNSIGNED_BYTE,
					 texturaMetoro->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do meteor");
	}
}

/* configur the meteor collision box */
void configuraCaixaColisaometeor(collision_box *dafault_meteor_collision)
{
	int c;

	for (c = 0; c < 8; c++){
		dafault_meteor_collision->points[c].x*=METEOR_SCALE;
		dafault_meteor_collision->points[c].y*=METEOR_SCALE;
		dafault_meteor_collision->points[c].z*=METEOR_SCALE;
	}
}

/* configur the meteors' aceleration */
void configuraAceleracaometeors(game_item *meteors)
{
	int i = 0;

	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		meteors[i].acceleration = 0.02;
	}
}

/* draws meteors */
void desenhameteors(game_item *meteors,
					 Texture texturaMetoro,
					 GLMmodel *meteor)
{
	int i = 0;

	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (meteors[i].visible) {
			glPushMatrix();
			glTranslatef(meteors[i].position.x,
						 meteors[i].position.y,
						 meteors[i].position.z);
			desenhaModelo(METEOR_MODEL, texturaMetoro, meteor);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}

#endif
