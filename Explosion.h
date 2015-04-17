/*
* Solar 3D Técnicas de Programação
* Esse programa realiza inspeções relacionado
* às explosões no jogo, criando local, configurando
* o tamanho das explosões, carrega o modelo e textura e 
* desenha a explosão no jogo
*/

#ifndef EXPLOSAO_H
#define EXPLOSAO_H

#include "Display_Manager.h"
#include "Solar_Utilities.h"

/* Creates an empty space for the explosions */
int posicaoVaziaExplosoes(itemDeJogo *explosoes)
{
	int i = 0;

	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		if(!explosoes[i].visivel) {
			return i;
		}
		else {
		  /* nothing to do */
		}
	}
	return -1; 
}

/* Configure the explosions size */
void configuraTamanhoExplosoes(itemDeJogo *explosoes){
	int i = 0;

	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		explosoes[i].tamanho = 1;
	}
}

/* loads the explosion's model */
void carregaModeloExplosao(GLMmodel **explosao){
	if (*explosao == NULL) {
			*explosao = glmReadOBJ("data/explosao/explosao.obj");

			if (*explosao == NULL) {
				printf("\n\nErro carregando explosao.obj");
				exit(0);
			}
			else {
				/* nothing to do */
			}
			/* redimension to unity matrix */
			glmUnitize(*explosao); 
			glmFacetNormals(*explosao);
			glmVertexNormals(*explosao, 90.0);
	} 
	else {
		printf("Modelo explosao nao carregou!");
	}
}

/* loads explosion's texture */
void carregaTexturaExplosao(Texture *texturaExplosao, char* filePath){
	if (LoadTGA(texturaExplosao, filePath)) {
		glGenTextures(1, &texturaExplosao->texID);
		glBindTexture(GL_TEXTURE_2D, texturaExplosao->texID);
		glTexImage2D(GL_TEXTURE_2D, 0,
					 GL_RGBA,
					 texturaExplosao->width,
					 texturaExplosao->height,
					 0,
					 texturaExplosao->type,
					 GL_UNSIGNED_BYTE,
					 texturaExplosao->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura da explosao");
	}

}

/* draws the explosions */
void desenhaExplosoes(itemDeJogo *explosoes,
					  Texture texturaExplosao,
					  GLMmodel *explosao)
{
	int i = 0;

	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		if (explosoes[i].visivel) {
			glColor3ub(255,255,255);
			glPushMatrix();
			glTranslatef(explosoes[i].posicao.x,
						 explosoes[i].posicao.y,
						 explosoes[i].posicao.z);
			glScalef(explosoes[i].tamanho,
					 explosoes[i].tamanho,
					 explosoes[i].tamanho);
			desenhaModelo(MODELO_EXPLOSAO, texturaExplosao, explosao);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}
#endif
