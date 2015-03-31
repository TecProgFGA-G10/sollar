#ifndef TIROS_H
#define TIROS_H
#include "SolarUtil.h"

int posicaoVaziaTiros(itemDeJogo *tiros)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		/* encontra o primeiro tiro vazio e o retorna */
		if (!tiros[i].visivel) {
			return i;
		}
	}
	return -1;
}

void carregaModeloTiro(GLMmodel **tiro) {
	if (*tiro == NULL) {
		*tiro = glmReadOBJ("data/tiro/tiro.obj");

		if (*tiro == NULL) {
			printf("\n\nErro carregando tiro.obj");
			exit(0);
		}
		/* calcula as normais */
		glmUnitize(*tiro);
		glmFacetNormals(*tiro);
		glmVertexNormals(*tiro, 90.0);
	}
	else {
		printf("Modelo tiro nao carrgado!\n");
	}

}

void atualizaCaixaColisaoTiro(itemDeJogo *item)
{
	int i = 0;
	/* pontos totais caixa de colisão */
	for (i = 0; i < 8; i++) {
		if (item->posicao.z > item->posicaoAnterior.z) {
			item->colisao.pontos[i].z += -item->aceleracao;
		}

		if (item->posicao.z < item->posicaoAnterior.z) {
			item->colisao.pontos[i].z += item->aceleracao;
		}
	}
}

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

void desenhaTiros(itemDeJogo *tiros, Texture texturaTiro, GLMmodel *tiro)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		if (tiros[i].visivel) {
			glPushMatrix();
				glTranslatef(tiros[i].posicao.x,
				             tiros[i].posicao.y,
				             tiros[i].posicao.z);
				desenhaModelo(MODELO_TIRO, texturaTiro, tiro);
			glPopMatrix();
		}
	}
}

void configuraCaixaColisaoTiro(caixaColisao *colisaoTiroDefault)
{
	int c;
	for (c = 0; c < 8; c++) {
		colisaoTiroDefault->pontos[c].x *= ESCALA_TIRO;
		colisaoTiroDefault->pontos[c].y *= ESCALA_TIRO;
		colisaoTiroDefault->pontos[c].z *= ESCALA_TIRO;
	}
}

void configuraAceleracaoTiros(itemDeJogo *tiros)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_TIROS; i++) {
		tiros[i].aceleracao = -0.1;
	}
}

#endif
