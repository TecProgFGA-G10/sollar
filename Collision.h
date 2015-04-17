/* 
* Solar 3D Técnicas de Programação.
* Esse programa verifica a colisão de objetos com a nave.
*/

#ifndef COLISAO_H
#define COLISAO_H
#include <GL/glut.h>
#include "Solar_Utilities.h"

/* draws the collision box */
void desenhaCaixaColisao(collision_box *c)
{
	int i = 0;
	glBegin(GL_LINES);
	for (i = 0; i < 8; i++) {
		glColor3ub(i * 10, i * 10, i * 10);
		glVertex3f(c->pontos[i].x, c->pontos[i].y, c->pontos[i].z);
	}
	glEnd();
}


 /*
 * Creates one collision box by the bigger
 * and smaller points in wich axix.
 */
void criaCaixaColisao(GLMmodel *modelo, collision_box *caixa)
{
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
		else {
			/* nothing to do */
		}
		
		if (tmpX < menorX) {
			menorX = tmpX;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpY > maiorY) {
			maiorY = tmpY;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpY < menorY) {
			menorY = tmpY;
		}
		else {
			/* nothing to do */
		}
	   	
	   	tmpZ = modelo->vertices[i * 3 + 2];
		
		if (tmpX > maiorX) {
			maiorX = tmpX;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpX < menorX) {
			menorX = tmpX;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpY > maiorY) {
			maiorY = tmpY;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpY < menorY) {
			menorY = tmpY;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpZ > maiorZ) {
			maiorZ = tmpZ;
		}
		else {
			/* nothing to do */
		}
		
		if (tmpZ < menorZ) {
			menorZ = tmpZ;
		}
		else {
			/* nothing to do */
		}
	}

	/* front upper left side */
	caixa->pontos[0].x = menorX;
	caixa->pontos[0].y = maiorY;
	caixa->pontos[0].z = menorZ;

	/* front upper right side */
	caixa->pontos[1].x = maiorX;
	caixa->pontos[1].y = maiorY;
	caixa->pontos[1].z = menorZ;

	/* front lower left side */
	caixa->pontos[2].x = menorX;
	caixa->pontos[2].y = menorY;
	caixa->pontos[2].z = menorZ;

	/* front lower right side */
	caixa->pontos[3].x = maiorX;
	caixa->pontos[3].y = menorY;
	caixa->pontos[3].z = menorZ;

	/* back upper left side */
	caixa->pontos[4].x = menorX;
	caixa->pontos[1].y = maiorY;
	caixa->pontos[1].z = menorZ;

	/* back lower left side */
	caixa->pontos[2].x = menorX;
	caixa->pontos[2].y = menorY;
	caixa->pontos[2].z = menorZ;

	/* back lower right side */
	caixa->pontos[3].x = maiorX;
	caixa->pontos[3].y = menorY;
	caixa->pontos[3].z = menorZ;

	/* back upper left side */
	caixa->pontos[4].x = menorX;
	caixa->pontos[4].y = maiorY;
	caixa->pontos[4].z = maiorZ;

	/* back upper right side */
	caixa->pontos[5].x = maiorX;
	caixa->pontos[5].y = maiorY;
	caixa->pontos[5].z = maiorZ;

	/* back lower left side */
	caixa->pontos[6].x = menorX;
	caixa->pontos[6].y = menorY;
	caixa->pontos[6].z = maiorZ;

	/* back lower right side */
	caixa->pontos[7].x = maiorX;
	caixa->pontos[7].y = menorY;
	caixa->pontos[7].z = maiorZ;
}


 /*
 * The collision box is mounted starting at <0,0,0>. 
 * Meteors have other positions, then the default patter must be
 * calculated again.
 */
void setaCaixaColisao(collision_box *c, point position)
{
	int i;

	/* transfer the box */
	for (i = 0; i <8; i++) {
		c->pontos[i].x+= position.x;
		c->pontos[i].y+= position.y;
		c->pontos[i].z+= position.z;
	}

}

/* actualizes the shoots and meteors states,positions and visibility */
void atualizaCaixaColisao(itemDeJogo *item)
{
	int i = 0;
	for (i = 0; i < 8; i++) {
		if (item->position.z > item->posicaoAnterior.z) {
			item->colisao.pontos[i].z+= item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.z < item->posicaoAnterior.z) {
			item->colisao.pontos[i].z+=-item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.x > item->posicaoAnterior.x) {
			item->colisao.pontos[i].x+= item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.x < item->posicaoAnterior.x) {
			item->colisao.pontos[i].x+= -item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.y > item->posicaoAnterior.y) {
			item->colisao.pontos[i].y+= item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.y < item->posicaoAnterior.y) {
			item->colisao.pontos[i].y+= -item->aceleracao;
		}
		else {
			/* nothing to do */
		}
	}
}

/* verifies the ship's collision with the asteroids */
int verificaColisao(collision_box a, collision_box b)
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
			else {
			/* nothing to do */
			}
		}
		else {
			/* nothing to do */
		}
	}
	else {
			/* nothing to do */
		}
	return FALSE;
}

#endif
