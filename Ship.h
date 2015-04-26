/*
* Solar 3D Técnicas de Programação
* Esse programa carrega o model da nave e sua textura no jogo
*/

#ifndef SHIP_H
#define SHIP_H
#include <GL/glut.h>
#include "Solar_Utilities.h"

/* loads the ship model */
void carregaModeloship(game_item *ship)
{
	ship->position.x = 0;
	ship->position.y = 0;
	ship->position.z = 0;
	ship->rotX = 0;
	ship->rotY = 0;
	ship->rotation_in_z = 0;
	ship->last_position.x = 0;
	ship->last_position.y = 0;
	ship->last_position.z = 0;
	ship->acceleration = 0.03;
	ship->visible = TRUE;

	int scala = 0;

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
		glmVertexNormals(ship->model, 90.0);

		for (scala = 0; scala < 8; scala++) {
			ship->collision.points[scala].x *= SHIP_SCALE;
			ship->collision.points[scala].y *= SHIP_SCALE;
			ship->collision.points[scala].z *= SHIP_SCALE;
		}
	}
	else {
		/* nothing to do */
	}
}

/* loads shit texture */
void load_ship_texture(Texture *ship_texture, char* filePath)
{
	if (LoadTGA(ship_texture,filePath)) {
		glGenTextures(1, &ship_texture->texID);
		glBindTexture(GL_TEXTURE_2D, ship_texture->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 ship_texture->bpp / 8,
					 ship_texture->width,
					 ship_texture->height,
					 0,
					 ship_texture->type,
					 GL_UNSIGNED_BYTE,
					 ship_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do jato.");
	}

}

#endif
