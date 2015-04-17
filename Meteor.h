/*
* Solar 3D Técnicas de Programação
* Esse programa carrega o meteoro, sua textura e
* envia para a área do jogo, configurando a área ocupada e
* desenhando o meteoro na área configurando também sua 
* aceleração
*/

#ifndef METEORO_H
#define METEORO_H
#include <GL/glut.h>
#include "Collision.h"

/* loads the meteor model */
void carregaModeloMeteoro(GLMmodel **meteoro)
{
	if (*meteoro == NULL) {
		*meteoro = glmReadOBJ("data/meteoro/meteoro.obj");
		if (*meteoro == NULL){
			printf("\n\nErro carregando meteoro.obj");
			exit(0);
		}
		else {
		  /* nothing to do */
		}
		glmUnitize(*meteoro); /* redimensions to unity matrix */
		glmFacetNormals(*meteoro);
		glmVertexNormals(*meteoro, 90.0);
	}
	else {
		printf("Meteoro nao carregado! \n");
	}
}

/* creates an empty meteor position */
int posicaoVaziaMeteoros(itemDeJogo *meteoros)
{
	int i = 0;
	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		if (!meteoros[i].visivel) {
			return i;
		}
		else {
			/* nothing to do */
		}
	}
	return -1;
}

/* send meteor to the screen */
void enviaMeteoro(itemDeJogo *meteoros,
				  int meteorosEnviar,
				  caixaColisao colisaoMeteoroDefault)
{
	int i = 0;
	
	for (i = 0; i < meteorosEnviar; i++) {
		int pos = posicaoVaziaMeteoros(meteoros);
		
		if (pos >= 0) {
			meteoros[pos].visivel = TRUE;
			meteoros[pos].posicao.z = (Z_INICIAL_METEORO-10) -
									   rand()%(Z_INICIAL_METEORO -
									   -20);
			meteoros[pos].posicao.x = MINIMO_X + rand() % (MAXIMO_X-MINIMO_X);
			meteoros[pos].posicao.y = MINIMO_Y + rand() % (MAXIMO_Y-MINIMO_Y);
			/*
			 * Keep the last position to calculate the collision box
			 * to the meteor in a different position of <0, 0, 0>
			 */
			meteoros[pos].posicaoAnterior.z = meteoros[pos].posicao.z;
			meteoros[pos].posicaoAnterior.x = meteoros[pos].posicao.x;
			meteoros[pos].posicaoAnterior.y = meteoros[pos].posicao.y;

			int c = 0;
			/* fix me. I am out of the bounds*/
			for (c = 0; c < 8; c++){
				meteoros[pos].colisao.pontos[c].x = colisaoMeteoroDefault.pontos[c].x;
				meteoros[pos].colisao.pontos[c].y = colisaoMeteoroDefault.pontos[c].y;
				meteoros[pos].colisao.pontos[c].z = colisaoMeteoroDefault.pontos[c].z;
			}
			setaCaixaColisao(&meteoros[pos].colisao, meteoros[pos].posicao);
		}
		else {
			/* nothing to do */
		}
	}
}

/* loads the meteor texture */
void carregaTexturaMeteoro(Texture *texturaMetoro, char *filePath)
{
	if (LoadTGA(texturaMetoro, filePath)) {
		glGenTextures(1, &texturaMetoro->texID);
		glBindTexture(GL_TEXTURE_2D, texturaMetoro->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 texturaMetoro->bpp / 8,
					 texturaMetoro->width,
					 texturaMetoro->height,
					 0,
					 texturaMetoro->type,
					 GL_UNSIGNED_BYTE,
					 texturaMetoro->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do meteoro");
	}
}

/* configur the meteor collision box */
void configuraCaixaColisaoMeteoro(caixaColisao *colisaoMeteoroDefault)
{
	int c;

	for (c = 0; c < 8; c++){
		colisaoMeteoroDefault->pontos[c].x*=ESCALA_METEORO;
		colisaoMeteoroDefault->pontos[c].y*=ESCALA_METEORO;
		colisaoMeteoroDefault->pontos[c].z*=ESCALA_METEORO;
	}
}

/* configur the meteors' aceleration */
void configuraAceleracaoMeteoros(itemDeJogo *meteoros)
{
	int i = 0;

	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		meteoros[i].aceleracao = 0.02;
	}
}

/* draws meteors */
void desenhaMeteoros(itemDeJogo *meteoros,
					 Texture texturaMetoro,
					 GLMmodel *meteoro)
{
	int i = 0;

	for (i = 0; i < NUM_MAX_METEOROS; i++) {
		if (meteoros[i].visivel) {
			glPushMatrix();
			glTranslatef(meteoros[i].posicao.x,
						 meteoros[i].posicao.y,
						 meteoros[i].posicao.z);
			desenhaModelo(MODELO_METEORO, texturaMetoro, meteoro);
			glPopMatrix();
		}
		else {
			/* nothing to do */
		}
	}
}

#endif
