/* 
* Solar 3D Técnicas de Programação.
* Esse programa verifica a colisão de objetos com a nave.
*/

#ifndef COLISAO_H
#define COLISAO_H
#include <GL/glut.h>
#include "Solar_Utilities.h"

/* draws the collision box */
void desenhaCaixaColisao(collision_box *box)
/* *box - pointer to collision box */
{
	int i = 0;
	glBegin(GL_LINES);
	for (i = 0; i < 8; i++) {
		glColor3ub(i * 10, i * 10, i * 10);
		glVertex3f(box->points[i].x, box->points[i].y, box->points[i].z);
	}
	glEnd();
}


 /*
 * Creates one collision box by the bigger
 * and smaller points in wich axix.
 */
void criaCaixaColisao(GLMmodel *modelo, collision_box *box)
/* *box - pointer to collision box */
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
	box->points[0].x = menorX;
	box->points[0].y = maiorY;
	box->points[0].z = menorZ;

	/* front upper right side */
	box->points[1].x = maiorX;
	box->points[1].y = maiorY;
	box->points[1].z = menorZ;

	/* front lower left side */
	box->points[2].x = menorX;
	box->points[2].y = menorY;
	box->points[2].z = menorZ;

	/* front lower right side */
	box->points[3].x = maiorX;
	box->points[3].y = menorY;
	box->points[3].z = menorZ;

	/* back upper left side */
	box->points[4].x = menorX;
	box->points[1].y = maiorY;
	box->points[1].z = menorZ;

	/* back lower left side */
	box->points[2].x = menorX;
	box->points[2].y = menorY;
	box->points[2].z = menorZ;

	/* back lower right side */
	box->points[3].x = maiorX;
	box->points[3].y = menorY;
	box->points[3].z = menorZ;

	/* back upper left side */
	box->points[4].x = menorX;
	box->points[4].y = maiorY;
	box->points[4].z = maiorZ;

	/* back upper right side */
	box->points[5].x = maiorX;
	box->points[5].y = maiorY;
	box->points[5].z = maiorZ;

	/* back lower left side */
	box->points[6].x = menorX;
	box->points[6].y = menorY;
	box->points[6].z = maiorZ;

	/* back lower right side */
	box->points[7].x = maiorX;
	box->points[7].y = menorY;
	box->points[7].z = maiorZ;
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
		c->points[i].x+= position.x;
		c->points[i].y+= position.y;
		c->points[i].z+= position.z;
	}

}

/* actualizes the shoots and meteors states,positions and visibility */
void atualizaCaixaColisao(game_item *item)
{
	int i = 0;
	for (i = 0; i < 8; i++) {
		if (item->position.z > item->posicaoAnterior.z) {
			item->colisao.points[i].z+= item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.z < item->posicaoAnterior.z) {
			item->colisao.points[i].z+=-item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.x > item->posicaoAnterior.x) {
			item->colisao.points[i].x+= item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.x < item->posicaoAnterior.x) {
			item->colisao.points[i].x+= -item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.y > item->posicaoAnterior.y) {
			item->colisao.points[i].y+= item->aceleracao;
		}
		else {
			/* nothing to do */
		}
		if (item->position.y < item->posicaoAnterior.y) {
			item->colisao.points[i].y+= -item->aceleracao;
		}
		else {
			/* nothing to do */
		}
	}
}

/* verifies the ship's collision with the asteroids */
int verificaColisao(collision_box a, collision_box b)
{
	if (((a.points[0].z >= b.points[0].z) &&
	     (a.points[0].z <= b.points[4].z)) ||
	    ((a.points[4].z >= b.points[4].z) &&
	     (a.points[4].z <= b.points[0].z)))
	{
		if (((a.points[0].x >= b.points[0].x) &&
		     (a.points[0].x <= b.points[1].x)) ||
		    ((a.points[1].x >= b.points[0].x) &&
		     (a.points[1].x <= b.points[1].x)))
		{
			if( (a.points[0].y >= b.points[2].y) &&
			    (a.points[2].y <= b.points[0].y) ||
			    (a.points[2].y >= b.points[2].y) &&
			    (a.points[2].y <= b.points[0].y))
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
