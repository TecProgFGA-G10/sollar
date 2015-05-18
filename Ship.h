/*
 * Solar 3D Técnicas de Programação
 * Esse programa carrega o model da nave e sua textura no jogo
 */

#ifndef SHIP_H
#define SHIP_H
#include <GL/glut.h>
#include "Solar_Utilities.h"
#include "logger.h"

/* loads the ship model */
void load_ship_model(game_item *ship)
{
	ship->position.x = INITIALIZE_ZERO;
	ship->position.y = INITIALIZE_ZERO;
	ship->position.z = INITIALIZE_ZERO;
	ship->rotation_in_x = INITIALIZE_ZERO;
	ship->rotation_in_y = INITIALIZE_ZERO;
	ship->rotation_in_z = INITIALIZE_ZERO;
	ship->last_position.x = INITIALIZE_ZERO;
	ship->last_position.y = INITIALIZE_ZERO;
	ship->last_position.z = INITIALIZE_ZERO;
	ship->acceleration = SHIP_ACCELERATION;
	ship->visible = TRUE;

	double model_angle = 90.0;

	if (!ship->model) {
		ship->model = glmReadOBJ("data/aviao/jato.obj");
		if (!ship->model) {
			exit(0);
		}
		else {
			/* nothing to do */
		}

		glmUnitize(ship->model);
		criaCaixaColisao(ship->model, &ship->collision);
		glmFacetNormals(ship->model);
		glmVertexNormals(ship->model, model_angle);

		for (unsigned int scale = INITIALIZE_ZERO; scale < 8; scale++) {
			ship->collision.points[scale].x *= SHIP_SCALE;
			ship->collision.points[scale].y *= SHIP_SCALE;
			ship->collision.points[scale].z *= SHIP_SCALE;
		}
	}
	else {
		/* nothing to do */
	}
}

/* loads shit texture */
void load_ship_texture(Texture *ship_texture, char* filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = INITIALIZE_ZERO;
	unsigned int border = INITIALIZE_ZERO;
	if (LoadTGA(ship_texture,filePath)) {
		glGenTextures(amount_of_textures, &ship_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, ship_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 ship_texture->bpp / 8, /* divides the bpp (byte per pixel) for each byte */
					 ship_texture->width,
					 ship_texture->height,
					 border,
					 ship_texture->type,
					 GL_UNSIGNED_BYTE,
					 ship_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
		print_verbose_log("Ship texture loaded");
	}
	else {
		print_error_log("Error loading ship texture");
	}

}

#endif
