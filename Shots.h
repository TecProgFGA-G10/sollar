/*
* Solar 3D Técnicas de Programação
* Esse programa realiza a inspenção de tiros da nave, carregando modelo, 
* definindo a posição, atualiza o local do tiro, desenha tiro, carrega a textura,
* configura local da colisão do tiro e aceleração
*/

#ifndef TIROS_H
#define TIROS_H
#include "Solar_Utilities.h"

/* creates an empty position to the shots */
int posicaoVaziaTiros(game_item *tiros)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		/* finds the first empty shot and returns it */
		if (!tiros[i].visivel) {
			return i;
		}
		else {
			/* nothing to do */
		}
	}
	return -1;
}

/* loads shot model */
void carregaModeloTiro(GLMmodel **tiro) {
	if (*tiro == NULL) {
		*tiro = glmReadOBJ("data/tiro/tiro.obj");

		if (*tiro == NULL) {
			printf("\n\nErro carregando tiro.obj");
			exit(0);
		}
		else {
			/* nothing to do */
		}
		/* calculate the normal ones */
		glmUnitize(*tiro);
		glmFacetNormals(*tiro);
		glmVertexNormals(*tiro, 90.0);
	}
	else {
		printf("Modelo tiro nao carregado!\n");
	}

}

/* update shot's box collision */
void atualizaCaixaColisaoTiro(game_item *item)
{
	int i = 0;
	/* total points collision box */
	for (i = 0; i < 8; i++) {
		if (item->position.z > item->posicaoAnterior.z) {
			item->colisao.pontos[i].z += -item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.z < item->posicaoAnterior.z) {
			item->colisao.pontos[i].z += item->aceleracao;
		}
		else {
			/* nothing to do */
		}
	}
}

/* loads shot's texture */
void carregaTexturaTiro(Texture *texturaTiro, char *filePath)
{
   if (LoadTGA(texturaTiro,filePath)) {
		glGenTextures(1, &texturaTiro->texID);
		glBindTexture(GL_TEXTURE_2D, texturaTiro->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 texturaTiro->bpp / 8,
					 texturaTiro->width,
					 texturaTiro->height,
					 0,
					 texturaTiro->type,
					 GL_UNSIGNED_BYTE,
					 texturaTiro->imageData);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do tiro");
	}
}

/* draws the shots */
void desenhaTiros(game_item *tiros, Texture texturaTiro, GLMmodel *tiro)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		if (tiros[i].visivel) {
			glPushMatrix();
				glTranslatef(tiros[i].position.x,
							 tiros[i].position.y,
							 tiros[i].position.z);
				desenhaModelo(MODELO_TIRO, texturaTiro, tiro);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}

/* configures shot's collision box */
void configuraCaixaColisaoTiro(collision_box *colisaoTiroDefault)
{
	int c;
	for (c = 0; c < 8; c++) {
		colisaoTiroDefault->pontos[c].x *= ESCALA_TIRO;
		colisaoTiroDefault->pontos[c].y *= ESCALA_TIRO;
		colisaoTiroDefault->pontos[c].z *= ESCALA_TIRO;
	}
}

/* configures shot's acceleration */
void configuraAceleracaoTiros(game_item *tiros)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		tiros[i].aceleracao = -0.1;
	}
}

#endif
