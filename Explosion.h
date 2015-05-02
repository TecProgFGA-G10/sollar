/*
 * Solar 3D Técnicas de Programação
 * Esse programa realiza inspeções relacionado
 * às explosões no jogo, criando local, configurando
 * o explosion_size das explosões, carrega o modelo e textura e
 * draw a explosão no jogo
 */

#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "Display_Manager.h"
#include "Solar_Utilities.h"

/* Creates an empty space for the explosions */
int posicaoVaziaExplosoes(game_item *explosions) /*pointer to the item explosion*/
{
	int result_iteration = -1;

	for (unsigned int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if(!explosions[i].visible) {
			result_iteration = i;
		}
		else {
		  /* nothing to do */
		}
	}
	return result_iteration;
}

/* Configure the explosions size */
void configuraTamanhoExplosoes(game_item *explosions){
	unsigned int size_value = 1;

	for (unsigned int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		explosions[i].explosion_size = size_value;
	}
}

/* loads the explosion's model */
void carregaModeloExplosao(GLMmodel **explosion){
	double explosion_angle = 90.0;
	const unsigned int NEUTRAL_VALUE = 0;
	if (*explosion == NULL) {
			*explosion = glmReadOBJ("data/explosao/explosao.obj");

			if (*explosion == NULL) {
				printf("\n\nErro carregando explosion.obj");
				exit(NEUTRAL_VALUE);
			}
			else {
				/* nothing to do */
			}
			/* redimension to unity matrix */
			glmUnitize(*explosion);
			glmFacetNormals(*explosion);
			glmVertexNormals(*explosion, explosion_angle);
	}
	else {
		printf("model explosion nao carregou!");
	}
}

/* loads explosion's texture */
void load_explosion_texture(Texture *explosion_texture, char* filePath){
	if (LoadTGA(explosion_texture, filePath)) {
		glGenTextures(1, &explosion_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, explosion_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_RGBA,
					 explosion_texture->width,
					 explosion_texture->height,
					 0,
					 explosion_texture->type,
					 GL_UNSIGNED_BYTE,
					 explosion_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura da explosion");
	}

}

/* draws the explosions */
void desenhaExplosoes(game_item *explosions,
					  Texture explosion_texture,
					  GLMmodel *explosion)
{
	unsigned int first_color_scale = 255;
	unsigned int second_color_scale = 255;
	unsigned int third_color_scale = 255;

	for (unsigned int i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (explosions[i].visible) {
			glColor3ub(first_color_scale, second_color_scale, third_color_scale);
			glPushMatrix();
			glTranslatef(explosions[i].position.x,
						 explosions[i].position.y,
						 explosions[i].position.z);
			glScalef(explosions[i].explosion_size,
					 explosions[i].explosion_size,
					 explosions[i].explosion_size);
			desenhaModelo(EXPLOSION_MODEL, explosion_texture, explosion);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}
#endif
