/*
 * Solar 3D Técnicas de Programação.
 * Esse programa verifica a colisão de objetos com a nave.
 */

#ifndef COLLISION_H
#include <GL/glut.h>
#include "Solar_Utilities.h"
#include <assert.h>
#include "logger.h"

#define CUBE_EDGES 8
#define COLLISION_H

/* draws the collision box */
void desenhaCaixaColisao(collision_box *box)
/* *box - pointer to collision box */
{
  /* Testing parameters */
  assert(box);

	glBegin(GL_LINES);
	for (int i = INITIALIZE_ZERO; i < CUBE_EDGES; i++) {
		glColor3ub(i * 10, i * 10, i * 10);
		glVertex3f(box->points[i].x, box->points[i].y, box->points[i].z);
	}
	glEnd();
}

/* return the greater value of two values */
float greater_value(float value_a, float value_b)
{
  /* Testing parameters */
  assert(value_a);
  assert(value_b);

  float result_greater_value = INITIALIZE_ZERO;
  if (value_a > value_b) {
    result_greater_value = value_a;
  }
  else {
    result_greater_value = value_b;
  }
  return result_greater_value;
}

/* return the lesse value of two values */
float lesser_value(float value_a, float value_b)
{
  /* Testing parameters */
  assert(value_a);
  assert(value_b);

  float result_lesser_value = INITIALIZE_ZERO;
  if (value_a < value_b) {
    result_lesser_value = value_a;
  }
  else {
    result_lesser_value = value_b;
  }
  return result_lesser_value;
}

/* updates the points of the box in collision */
void update_box_points(collision_box *box,
                       float lesser_x, float lesser_y, float lesser_z,
                       float greater_x, float greater_y, float greater_z)
{
  /* Testing parameters */
  assert(box);
  assert(lesser_x);
  assert(lesser_z);
  assert(lesser_y);
  assert(greater_y);
  assert(greater_x);
  assert(greater_z);

  /* front upper left side */
  box->points[0].x = lesser_x;
  box->points[0].y = greater_y;
  box->points[0].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[0].x = %f", box->points[0].x);
  print_debug_log("Box->points[0].y = %f", box->points[0].y);
  print_debug_log("Box->points[0].z = %f", box->points[0].z);

  /* front upper right side */
  box->points[1].x = greater_x;
  box->points[1].y = greater_y;
  box->points[1].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[1].x = %d", box->points[1].x);
  print_debug_log("Box->points[1].y = %d", box->points[1].y);
  print_debug_log("Box->points[1].z = %d", box->points[1].z);

  /* front lower left side */
  box->points[2].x = lesser_x;
  box->points[2].y = lesser_y;
  box->points[2].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[2].x = %d", box->points[2].x);
  print_debug_log("Box->points[2].y = %d", box->points[2].y);
  print_debug_log("Box->points[2].z = %d", box->points[2].z);

  /* front lower right side */
  box->points[3].x = greater_x;
  box->points[3].y = lesser_y;
  box->points[3].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[3].x = %d", box->points[3].x);
  print_debug_log("Box->points[3].y = %d", box->points[3].y);
  print_debug_log("Box->points[3].z = %d", box->points[3].z);

  /* back upper left side */
  box->points[4].x = lesser_x;
  box->points[1].y = greater_y;
  box->points[1].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[4].x = %d", box->points[4].x);
  print_debug_log("Box->points[1].y = %d", box->points[1].y);
  print_debug_log("Box->points[1].z = %d", box->points[1].z);

  /* back lower left side */
  box->points[2].x = lesser_x;
  box->points[2].y = lesser_y;
  box->points[2].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[2].x = %d", box->points[2].x);
  print_debug_log("Box->points[2].y = %d", box->points[2].y);
  print_debug_log("Box->points[2].z = %d", box->points[2].z);

  /* back lower right side */
  box->points[3].x = greater_x;
  box->points[3].y = lesser_y;
  box->points[3].z = lesser_z;

  /* Log */
  print_debug_log("Box->points[3].x = %d", box->points[3].x);
  print_debug_log("Box->points[3].y = %d", box->points[3].y);
  print_debug_log("Box->points[3].z = %d", box->points[3].z);

  /* back upper left side */
  box->points[4].x = lesser_x;
  box->points[4].y = greater_y;
  box->points[4].z = greater_z;

  /* Log */
  print_debug_log("Box->points[4].x = %d", box->points[4].x);
  print_debug_log("Box->points[4].y = %d", box->points[4].y);
  print_debug_log("Box->points[4].z = %d", box->points[4].z);

  /* back upper right side */
  box->points[5].x = greater_x;
  box->points[5].y = greater_y;
  box->points[5].z = greater_z;

  /* Log */
  print_debug_log("Box->points[5].x = %d", box->points[5].x);
  print_debug_log("Box->points[5].y = %d", box->points[5].y);
  print_debug_log("Box->points[5].z = %d", box->points[5].z);

  /* back lower left side */
  box->points[6].x = lesser_x;
  box->points[6].y = lesser_y;
  box->points[6].z = greater_z;

  /* Log */
  print_debug_log("Box->points[6].x = %d", box->points[6].x);
  print_debug_log("Box->points[6].y = %d", box->points[6].y);
  print_debug_log("Box->points[6].z = %d", box->points[6].z);

  /* back lower right side */
  box->points[7].x = greater_x;
  box->points[7].y = lesser_y;
  box->points[7].z = greater_z;

  /* Log */
  print_debug_log("Box->points[7].x = %d", box->points[7].x);
  print_debug_log("Box->points[7].y = %d", box->points[7].y);
  print_debug_log("Box->points[7].z = %d", box->points[7].z);
}

 /*
  * Creates one collision box by the bigger
  * and smaller points in wich axix.
  */
void criaCaixaColisao(GLMmodel *model, collision_box *box)
/* box - pointer to collision box */
{
  /* Testing parameters */
  assert(model);
  assert(box);

	if (!model) {
		exit(1);
	}
	else {
		int max_vertices_numbers = model->vertices_numbers;

		float greater_x = model->vertex[3];
		float lesser_x = model->vertex[3];
		float greater_y = model->vertex[4];
		float lesser_y = model->vertex[4];
		float greater_z = model->vertex[5];
		float lesser_z = model->vertex[5];

		float tmp_x = 0;
		float tmp_y = 0;
		float tmp_z = 0;

		for (int i = 2; i < max_vertices_numbers; i++) {
			tmp_x = model->vertex[i * 3];
			greater_x = greater_value(greater_x, tmp_x);
			lesser_x = lesser_value(lesser_x, tmp_x);

			tmp_y = model->vertex[i * 3 + 1];
			greater_y = greater_value(greater_y, tmp_y);
			lesser_y = lesser_value(lesser_y, tmp_y);

			tmp_z = model->vertex[i * 3 + 2];
			greater_z = greater_value(greater_z, tmp_z);
			lesser_z = lesser_value(lesser_z, tmp_z);
		}

    print_verbose_log("Value of variable greater_x: ", greater_x);
    print_verbose_log("Value of variable lesser_x: ", lesser_x);
    print_verbose_log("Value of variable greater_y: ", greater_y);
    print_verbose_log("Value of variable lesser_y: ", lesser_y);
    print_verbose_log("Value of variable greater_z: ", greater_z);
    print_verbose_log("Value of variable lesser_z: ", lesser_x);

		update_box_points(box,
		                  lesser_x, lesser_y, lesser_z,
		                  greater_x, greater_y, greater_z);
	}
}


 /*
  * The collision box is mounted starting at <0,0,0>.
  * Meteors have other positions, then the default patter must be
  * calculated again.
  */
void set_collision_box(collision_box *c, point position)
{
  /* Testing parameters */
  assert(c);
  assert(position);

	/* transfer the box */
	for (int i = INITIALIZE_ZERO; i < CUBE_EDGES; i++) {
		c->points[i].x += position.x;
		c->points[i].y += position.y;
		c->points[i].z += position.z;

    print_debug_log("Final value of c->point[%d].x: %d", c->points[i].x);
    print_debug_log("Final value of c->point[%d].y: %d", c->points[i].y);
    print_debug_log("Final value of c->point[%d].z: %d", c->points[i].z);
	}
}

/* evaluates collision for every points x, y and z */
float evaluates_collision(float position,
							float last_position,
							float collision_points,
							float acceleration)
{
  /* Testing parameters */
  assert(position);
  assert(last_position);
  assert(collision_points);
  assert(acceleration);

	if (position > last_position) {
		collision_points += acceleration;
	}
	else if (position < last_position) {
		collision_points += -acceleration;
	}
	else {
		 /* nothing to do */
	}

	return collision_points;
}

/* actualizes the shoots and meteors states,positions and visibility */
void update_collision_box(game_item *item)
{
	for (int i = INITIALIZE_ZERO; i < CUBE_EDGES; i++) {
		item->collision.points[i].z = evaluates_collision(
            item->position.z,
			item->last_position.z,
			item->collision.points[i].z,
			item->acceleration
		);

		item->collision.points[i].x = evaluates_collision(
		    item->position.x,
			item->last_position.x,
			item->collision.points[i].x,
			item->acceleration
		);

		item->collision.points[i].y = evaluates_collision(
			item->position.y,
			item->last_position.y,
			item->collision.points[i].y,
			item->acceleration
		);
	}
}

/* verifies the ship's collision with the asteroids */
int verify_collision(collision_box a, collision_box b)
{
	int boolean_value = FALSE;
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
			if(((a.points[0].y >= b.points[2].y) &&
			    (a.points[2].y <= b.points[0].y)) ||
			   ((a.points[2].y >= b.points[2].y) &&
			    (a.points[2].y <= b.points[0].y)))
			{
				boolean_value = TRUE;
				print_verbose_log("Collision detected");
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
	return boolean_value;
}

#endif
