/*
 * Solar 3D Técnicas de Programação
 * Esse programa realiza a inspenção de shots da nave, carregando model,
 * definindo a posição, atualiza o local do shot, draw shot, carrega a textura,
 * configura local da colisão do tiro e aceleração
 */

#ifndef SHOTS_H
#define SHOTS_H
#include "Solar_Utilities.h"

/* creates an empty position to the shots */
int posicaoVaziaTiros(game_item *shots)
{
	int result_iterator = -1;
	for (unsigned int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
		/* finds the first empty shot and returns it */
		if (!shots[i].visible) {
			result_iterator = i;
		}
		else {
			/* nothing to do */
		}
	}
	return result_iterator;
}

/* loads shot model */
void carregaModeloTiro(GLMmodel **shot)
{
	double shot_angle = 90.0;
	if (*shot == NULL) {
		*shot = glmReadOBJ("data/tiro/tiro.obj");

		if (*shot == NULL) {
			printf("\n\nErro carregando tiro.obj");
			exit(0);
		}
		else {
			/* nothing to do */
		}
		/* calculate the normal ones */
		glmUnitize(*shot);
		glmFacetNormals(*shot);
		glmVertexNormals(*shot, shot_angle);
	}
	else {
		printf("model shot nao carregado!\n");
	}

}

/* update shot's box collision */
void atualizaCaixaColisaoTiro(game_item *item)
{
	/* total points collision box */
	for (unsigned int i = 0; i < 8; i++) {
		if (item->position.z > item->last_position.z) {
			item->collision.points[i].z += -item->acceleration;
		}
		else {
			/* nothing to do */
		}
		if (item->position.z < item->last_position.z) {
			item->collision.points[i].z += item->acceleration;
		}
		else {
			/* nothing to do */
		}
	}
}

/* loads shot's texture */
void load_shot_texture(Texture *shot_texture, char *filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = 0;
	unsigned int border = 0;
   	if (LoadTGA(shot_texture,filePath)) {
		glGenTextures(amount_of_textures, &shot_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, shot_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 shot_texture->bpp / 8, /* divides the bpp (byte per pixel) for each byte */
					 shot_texture->width,
					 shot_texture->height,
					 border,
					 shot_texture->type,
					 GL_UNSIGNED_BYTE,
					 shot_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do shot");
	}
}

/* draws the shots */
void desenhaTiros(game_item *shots, Texture shot_texture, GLMmodel *shot)
{
	for (unsigned int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
		if (shots[i].visible) {
			glPushMatrix();
				glTranslatef(shots[i].position.x,
							 shots[i].position.y,
							 shots[i].position.z);
				desenhaModelo(SHOT_MODEL, shot_texture, shot);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}

/* configures shot's collision box */
void configuraCaixaColisaoTiro(collision_box *dafault_collision_shot)
{
	for (unsigned int c = 0; c < 8; c++) {
		dafault_collision_shot->points[c].x *= SHOT_SCALE;
		dafault_collision_shot->points[c].y *= SHOT_SCALE;
		dafault_collision_shot->points[c].z *= SHOT_SCALE;
	}
}

/* configures shot's acceleration */
void configuraAceleracaoTiros(game_item *shots)
{
	for (unsigned int i = 0; i < MAX_NUMBER_OF_SHOTS; i++) {
		shots[i].acceleration = -0.9;
	}
}

#endif
