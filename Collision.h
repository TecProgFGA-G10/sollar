/*
 * Solar 3D Técnicas de Programação.
 * Esse programa verifica a colisão de objetos com a nave.
 */

#ifndef COLLISION.H
#define COLLISION.H
#include <GL/glut.h>
#include "Solar_Utilities.h"
#define CUBE_EDGES 8

/* draws the collision box */
void desenhaCaixaColisao(collision_box *box)
/* *box - pointer to collision box */
{
	int i = 0;
	glBegin(GL_LINES);
	for (i = 0; i < CUBE_EDGES; i++) {
		glColor3ub(i * 10, i * 10, i * 10);
		glVertex3f(box->points[i].x, box->points[i].y, box->points[i].z);
	}
	glEnd();
}


 /*
 * Creates one collision box by the bigger
 * and smaller points in wich axix.
 */
void criaCaixaColisao(GLMmodel *model, collision_box *box)
/* *box - pointer to collision box */
{
	if (!model) {
		exit(1);
	}
	else{
		/* nothing to do */
	}

	int i = 0;
	int max_vertices_numbers = model->vertices_numbers;

	float greater_x = model->vertices[3];
	float lesser_x = model->vertices[3];
	float greater_y = model->vertices[4];
	float lesser_y = model->vertices[4];
	float greater_z = model->vertices[5];
	float lesser_z = model->vertices[5];

	float tmp_x;
	float tmp_y;
	float tmp_z;

	for (i = 2; i < max_vertices_numbers; i++) {
		tmp_x = model->vertices[i * 3];
		tmp_y = model->vertices[i * 3 + 1];
		tmp_z = model->vertices[i * 3 + 2];

		if (tmp_x > greater_x) {
			greater_x = tmp_x;
		}
		else {
			/* nothing to do */
		}

		if (tmp_x < lesser_x) {
			lesser_x = tmp_x;
		}
		else {
			/* nothing to do */
		}

		if (tmp_y > greater_y) {
			greater_y = tmp_y;
		}
		else {
			/* nothing to do */
		}

		if (tmp_y < lesser_y) {
			lesser_y = tmp_y;
		}
		else {
			/* nothing to do */
		}

		tmp_z = model->vertices[i * 3 + 2];

		if (tmp_x > greater_x) {
			greater_x = tmp_x;
		}
		else {
			/* nothing to do */
		}

		if (tmp_x < lesser_x) {
			lesser_x = tmp_x;
		}
		else {
			/* nothing to do */
		}

		if (tmp_y > greater_y) {
			greater_y = tmp_y;
		}
		else {
			/* nothing to do */
		}

		if (tmp_y < lesser_y) {
			lesser_y = tmp_y;
		}
		else {
			/* nothing to do */
		}

		if (tmp_z > greater_z) {
			greater_z = tmp_z;
		}
		else {
			/* nothing to do */
		}

		if (tmp_z < lesser_z) {
			lesser_z = tmp_z;
		}
		else {
			/* nothing to do */
		}
	}

	/* front upper left side */
	box->points[0].x = lesser_x;
	box->points[0].y = greater_y;
	box->points[0].z = lesser_z;

	/* front upper right side */
	box->points[1].x = greater_x;
	box->points[1].y = greater_y;
	box->points[1].z = lesser_z;

	/* front lower left side */
	box->points[2].x = lesser_x;
	box->points[2].y = lesser_y;
	box->points[2].z = lesser_z;

	/* front lower right side */
	box->points[3].x = greater_x;
	box->points[3].y = lesser_y;
	box->points[3].z = lesser_z;

	/* back upper left side */
	box->points[4].x = lesser_x;
	box->points[1].y = greater_y;
	box->points[1].z = lesser_z;

	/* back lower left side */
	box->points[2].x = lesser_x;
	box->points[2].y = lesser_y;
	box->points[2].z = lesser_z;

	/* back lower right side */
	box->points[3].x = greater_x;
	box->points[3].y = lesser_y;
	box->points[3].z = lesser_z;

	/* back upper left side */
	box->points[4].x = lesser_x;
	box->points[4].y = greater_y;
	box->points[4].z = greater_z;

	/* back upper right side */
	box->points[5].x = greater_x;
	box->points[5].y = greater_y;
	box->points[5].z = greater_z;

	/* back lower left side */
	box->points[6].x = lesser_x;
	box->points[6].y = lesser_y;
	box->points[6].z = greater_z;

	/* back lower right side */
	box->points[7].x = greater_x;
	box->points[7].y = lesser_y;
	box->points[7].z = greater_z;
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
	for (i = 0; i < CUBE_EDGES; i++) {
		c->points[i].x += position.x;
		c->points[i].y += position.y;
		c->points[i].z += position.z;
	}

}

/* actualizes the shoots and meteors states,positions and visibility */
void atualizaCaixaColisao(game_item *item)
{
	int i = 0;
	for (i = 0; i < CUBE_EDGES; i++) {
		if (item->position.z > item->last_position.z) {
			item->collision.points[i].z += item->acceleration;
		}
		else {
			/* nothing to do */
		}
		if (item->position.z < item->last_position.z) {
			item->collision.points[i].z +=-item->acceleration;
		}
		else {
			/* nothing to do */
		}
		if (item->position.x > item->last_position.x) {
			item->collision.points[i].x += item->acceleration;
		}
		else {
			/* nothing to do */
		}
		if (item->position.x < item->last_position.x) {
			item->collision.points[i].x += -item->acceleration;
		}
		else {
			/* nothing to do */
		}
		if (item->position.y > item->last_position.y) {
			item->collision.points[i].y += item->acceleration;
		}
		else {
			/* nothing to do */
		}
		if (item->position.y < item->last_position.y) {
			item->collision.points[i].y += -item->acceleration;
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
