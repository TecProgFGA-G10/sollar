#ifndef COLISAO_H
#define COLISAO_H
#include <GL/glut.h>
#include "SolarUtil.h"

void desenhaCaixaColisao(caixaColisao *c)
{
	int i = 0;
	glBegin(GL_LINES);
	for (i = 0; i < 8; i++) {
		glColor3ub(i * 10, i * 10, i * 10);
		glVertex3f(c->pontos[i].x, c->pontos[i].y, c->pontos[i].z);
	}
	glEnd();
}

void criaCaixaColisao(GLMmodel *modelo, caixaColisao *caixa)
{
    /*
	 * Cria uma caixa de colisao atraves dos maiores
	 * e menores pontos em cada eixo.
	 */
	if (!modelo) {
		exit(1);
	}

	int i = 0;
	int max = modelo->numvertices;

	float maiorX = modelo->vertices[3];
	float menorX = modelo->vertices[3];
	float maiorY = modelo->vertices[4];
	float menorY = modelo->vertices[4];
	float maiorZ = modelo->vertices[5];
	float menorZ = modelo->vertices[5];

	float tmpX;
	float tmpY;
	float tmpZ;
	for (i = 2; i < max; i++) {
		tmpX = modelo->vertices[i*3];
		tmpY = modelo->vertices[i*3+1];
		tmpZ = modelo->vertices[i*3+2];
		if (tmpX > maiorX) {
			maiorX = tmpX;
		}
		if (tmpX < menorX) {
			menorX = tmpX;
		}
		if (tmpY > maiorY) {
			maiorY = tmpY;
		}
		if (tmpY < menorY) {
			menorY = tmpY;
		}
	   tmpZ = modelo->vertices[i * 3 + 2];
		if (tmpX > maiorX) {
			maiorX = tmpX;
		}
		if (tmpX < menorX) {
			menorX = tmpX;
		}
		if (tmpY > maiorY) {
			maiorY = tmpY;
		}
		if (tmpY < menorY) {
			menorY = tmpY;
		}
		if (tmpZ > maiorZ) {
			maiorZ = tmpZ;
		}
		if (tmpZ < menorZ) {
			menorZ = tmpZ;
		}
	}

	/* canto esquerdo superior da frente */
	caixa->pontos[0].x = menorX;
	caixa->pontos[0].y = maiorY;
	caixa->pontos[0].z = menorZ;

	/* canto direito superior da frente */
	caixa->pontos[1].x = maiorX;
	caixa->pontos[1].y = maiorY;
	caixa->pontos[1].z = menorZ;

	/* canto esquerdo inferior da frente */
	caixa->pontos[2].x = menorX;
	caixa->pontos[2].y = menorY;
	caixa->pontos[2].z = menorZ;

	/* canto direito inferior da frente */
	caixa->pontos[3].x = maiorX;
	caixa->pontos[3].y = menorY;
	caixa->pontos[3].z = menorZ;

	/* agora os de traz */

	/* canto esquerdo superior da frente */
	caixa->pontos[4].x = menorX;
	caixa->pontos[1].y = maiorY;
	caixa->pontos[1].z = menorZ;

	/* canto esquerdo inferior da frente */
	caixa->pontos[2].x = menorX;
	caixa->pontos[2].y = menorY;
	caixa->pontos[2].z = menorZ;

	/* canto direito inferior da frente */
	caixa->pontos[3].x = maiorX;
	caixa->pontos[3].y = menorY;
	caixa->pontos[3].z = menorZ;

	/* agora os de traz */

	/* canto esquerdo superior da frente */
	caixa->pontos[4].x = menorX;
	caixa->pontos[4].y = maiorY;
	caixa->pontos[4].z = maiorZ;

	/* canto direito superior da frente */
	caixa->pontos[5].x = maiorX;
	caixa->pontos[5].y = maiorY;
	caixa->pontos[5].z = maiorZ;

	/* canto esquerdo inferior da frente */
	caixa->pontos[6].x = menorX;
	caixa->pontos[6].y = menorY;
	caixa->pontos[6].z = maiorZ;

	/* canto direito inferior da frente */
	caixa->pontos[7].x = maiorX;
	caixa->pontos[7].y = menorY;
	caixa->pontos[7].z = maiorZ;
}

void setaCaixaColisao(caixaColisao *c, ponto posicao)
{
	/*
	 * A caixa de colisao e montada a partir do ponto <0,0,0>.. os meteoros tem uma posicao diferente
	 * dessa entao, para eles, deve-se calcular o valor padrao novamente.
	 */
	int i;
	for (i = 0; i <8; i++) {
		/* 'translada' a caixa */
		c->pontos[i].x+= posicao.x;
		c->pontos[i].y+= posicao.y;
		c->pontos[i].z+= posicao.z;
	}

}

/* atualiza o estado dos tiros e meteoros, suas posicoes e visibilidade */
void atualizaCaixaColisao(itemDeJogo *item)
{
	int i =0;
	for (i = 0; i < 8; i++) {
		if (item->posicao.z > item->posicaoAnterior.z) {
			item->colisao.pontos[i].z+= item->aceleracao;
		}
		if (item->posicao.z < item->posicaoAnterior.z) {
			item->colisao.pontos[i].z+=-item->aceleracao;
		}
		if (item->posicao.x > item->posicaoAnterior.x) {
			item->colisao.pontos[i].x+= item->aceleracao;
		}
		if (item->posicao.x < item->posicaoAnterior.x) {
			item->colisao.pontos[i].x+= -item->aceleracao;
		}
		if (item->posicao.y > item->posicaoAnterior.y) {
			item->colisao.pontos[i].y+= item->aceleracao;
		}
		if (item->posicao.y < item->posicaoAnterior.y) {
			item->colisao.pontos[i].y+= -item->aceleracao;
		}
	}
}

/* verifica colisao da nave com os asteroids */
int verificaColisao(caixaColisao a, caixaColisao b)
{
	if (((a.pontos[0].z >= b.pontos[0].z) &&
	     (a.pontos[0].z <= b.pontos[4].z)) ||
	    ((a.pontos[4].z >= b.pontos[4].z) &&
	     (a.pontos[4].z <= b.pontos[0].z)))
	{
		if (((a.pontos[0].x >= b.pontos[0].x) &&
		     (a.pontos[0].x <= b.pontos[1].x)) ||
		    ((a.pontos[1].x >= b.pontos[0].x) &&
		     (a.pontos[1].x <= b.pontos[1].x)))
		{
			if( (a.pontos[0].y >= b.pontos[2].y) &&
			    (a.pontos[2].y <= b.pontos[0].y) ||
			    (a.pontos[2].y >= b.pontos[2].y) &&
			    (a.pontos[2].y <= b.pontos[0].y))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

#endif
