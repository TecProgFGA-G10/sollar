/*
* Solar 3D Técnicas de Programação
* Esse programa carrega o model da nave e sua textura no jogo
*/

#ifndef aircraft_H
#define aircraft_H
#include <GL/glut.h>
#include "Solar_Utilities.h"

/* loads the ship model */
void carregaModeloaircraft(game_item *aircraft)
{
	aircraft->position.x = 0;
	aircraft->position.y = 0;
	aircraft->position.z = 0;
	aircraft->rotX = 0;
	aircraft->rotY = 0;
	aircraft->rotZ = 0;
	aircraft->last_position.x = 0;
	aircraft->last_position.y = 0;
	aircraft->last_position.z = 0;
	aircraft->acceleration = 0.03;
	aircraft->visible = TRUE;

	int scala = 0;

	if (!aircraft->model) {
		aircraft->model = glmReadOBJ("data/aviao/jato.obj");
		if (!aircraft->model) {
			exit(0);
		}
		else {
			/* nothing to do */
		}

		glmUnitize(aircraft->model);
		criaCaixaColisao(aircraft->model, &aircraft->collision);
		glmFacetNormals(aircraft->model);
		glmVertexNormals(aircraft->model, 90.0);

		for (scala = 0; scala < 8; scala++) {
			aircraft->collision.points[scala].x *= AIRCRAFT_SCALE;
			aircraft->collision.points[scala].y *= AIRCRAFT_SCALE;
			aircraft->collision.points[scala].z *= AIRCRAFT_SCALE;
		}
	}
	else {
		/* nothing to do */
	}
}

/* loads shit texture */
void carregaTexturaaircraft(Texture *texturaaircraft, char* filePath)
{
	if (LoadTGA(texturaaircraft,filePath)) {
		glGenTextures(1, &texturaaircraft->texID);
		glBindTexture(GL_TEXTURE_2D, texturaaircraft->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 texturaaircraft->bpp / 8,
					 texturaaircraft->width,
					 texturaaircraft->height,
					 0,
					 texturaaircraft->type,
					 GL_UNSIGNED_BYTE,
					 texturaaircraft->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do jato.");
	}

}

#endif
