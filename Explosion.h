/*
 * Solar 3D Técnicas de Programação
 * Esse programa realiza inspeções relacionado
 * às explosões no jogo, criando local, configurando
 * o explosion_size das explosões, carrega o model e textura e
 * desenha a explosão no jogo
 */

#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "Display_Manager.h"
#include "Solar_Utilities.h"

/* Creates an empty space for the explosions */
int posicaoVaziaExplosoes(game_item *explosions) /*pointer to the item explosion*/
{
	int i = 0;

	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if(!explosions[i].visible) {
			return i;
		}
		else {
		  /* nothing to do */
		}
	}
	return -1;
}

/* Configure the explosions size */
void configuraTamanhoExplosoes(game_item *explosions){
	int i = 0;

	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		explosions[i].explosion_size = 1;
	}
}

/* loads the explosion's model */
void carregaModeloExplosao(GLMmodel **explosion){
	if (*explosion == NULL) {
			*explosion = glmReadOBJ("data/explosao/explosao.obj");

			if (*explosion == NULL) {
				printf("\n\nErro carregando explosion.obj");
				exit(0);
			}
			else {
				/* nothing to do */
			}
			/* redimension to unity matrix */
			glmUnitize(*explosion);
			glmFacetNormals(*explosion);
			glmVertexNormals(*explosion, 90.0);
	}
	else {
		printf("model explosion nao carregou!");
	}
}

/* loads explosion's texture */
void carregaexplosion_texture(Texture *explosion_texture, char* filePath){
	if (LoadTGA(explosion_texture, filePath)) {
		glGenTextures(1, &explosion_texture->texID);
		glBindTexture(GL_TEXTURE_2D, explosion_texture->texID);
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
	int i = 0;

	for (i = 0; i < MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR; i++) {
		if (explosions[i].visible) {
			glColor3ub(255,255,255);
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
