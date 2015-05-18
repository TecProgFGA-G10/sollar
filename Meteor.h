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
#include "logger.h"
#include "Solar_Utilities.h"

/* loads the meteor model */
void carregaModelometeor(GLMmodel **meteor)
{
	double explosion_angle = 90.0;
	if (*meteor == NULL) {
		*meteor = glmReadOBJ("data/meteoro/meteoro.obj");
		if (*meteor == NULL){
			print_error_log("Error loading meteoro.obj");
			exit(0);
		}
		else {
		  /* nothing to do */
		}
		glmUnitize(*meteor); /* redimensions to unity matrix */
		glmFacetNormals(*meteor);
		glmVertexNormals(*meteor, explosion_angle);
	}
	else {
		print_error_log("Meteor not loaded");
	}
}

/* creates an empty meteor position */
int posicaoVaziameteors(game_item *meteors)
{
	unsigned int result_iteration = -1;
	for (unsigned int i = INITIALIZE_ZERO; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (!meteors[i].visible) {
			result_iteration = i;
		}
		else {
			/* nothing to do */
		}
	}
	return result_iteration;
}

/* send meteor to the screen */
void enviameteor(game_item *meteors,
				  int meteors_to_send,
				  collision_box dafault_meteor_collision)
{
	for (unsigned int i = INITIALIZE_ZERO; i < meteors_to_send; i++) {
		unsigned int pos = posicaoVaziameteors(meteors);

		if (pos >= 0) { /* position can be 0 or greater than 0 so that can view the meteor and calculate colision */
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

			/* fix me. I am out of the bounds*/
			for (unsigned int c = INITIALIZE_ZERO; c < 8; c++){
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
void load_meteor_texture(Texture *meteor_texture, char *filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = INITIALIZE_ZERO;
	unsigned int border = INITIALIZE_ZERO;
	if (LoadTGA(meteor_texture, filePath)) {
		glGenTextures(amount_of_textures, &meteor_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, meteor_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 meteor_texture->bpp / 8, /* divides the bpp (byte per pixel) for each byte */
					 meteor_texture->width,
					 meteor_texture->height,
					 border,
					 meteor_texture->type,
					 GL_UNSIGNED_BYTE,
					 meteor_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		print_error_log("Error loading the meteor texture");
	}
}

/* configur the meteor collision box */
void configuraCaixaColisaometeor(collision_box *dafault_meteor_collision)
{
	for (unsigned int c = INITIALIZE_ZERO; c < 8; c++){
		dafault_meteor_collision->points[c].x*=METEOR_SCALE;
		dafault_meteor_collision->points[c].y*=METEOR_SCALE;
		dafault_meteor_collision->points[c].z*=METEOR_SCALE;
	}
}

/* configur the meteors' aceleration */
void configuraAceleracaometeors(game_item *meteors)
{
	for (unsigned int i = INITIALIZE_ZERO; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		meteors[i].acceleration = 5.00;
	}
}

/* draws meteors */
void desenhameteors(game_item *meteors,
					 Texture meteor_texture,
					 GLMmodel *meteor)
{
	for (unsigned int i = INITIALIZE_ZERO; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (meteors[i].visible) {
			glPushMatrix();
			glTranslatef(meteors[i].position.x,
						 meteors[i].position.y,
						 meteors[i].position.z);
			draw_model(METEOR_MODEL, meteor_texture, meteor);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}

#endif
