/*
 * Solar 3D Técnicas de Programação
 * Esse programa realiza a inspenção de shots da nave, carregando model,
 * definindo a posição, atualiza o local do shot, draw shot, carrega a textura,
 * configura local da colisão do tiro e aceleração
 */

#ifndef SHOTS_H
#define SHOTS_H
#include "Solar_Utilities.h"
#include "logger.h"

/* creates an empty position to the shots */
int create_empty_shot_position(game_item *shots)
{
	int result_iterator = -1;
	for (unsigned int i = INITIALIZE_ZERO; i < MAX_NUMBER_OF_SHOTS; i++) {
		/* finds the first empty shot and returns it */
		if (!shots[i].visible) {
			result_iterator = i;
			print_verbose_log("Empty shot located");
		}
		else {
			print_error_log("Error, not possible find empty shot");
		}
	}
	return result_iterator;
	print_verbose_log("Empty shot position is created");
}

/* loads shot model */
void load_shot_model(GLMmodel **shot)
{
	double shot_angle = 90.0;
	if (*shot == NULL) {
		*shot = glmReadOBJ("data/tiro/tiro.obj");

		if (*shot != NULL) {
			print_verbose_log("tiro.obj sucessfully loaded");
		}
		else {
			print_error_log("Error loading tiro.obj");
			exit(0);
		}
		/* calculate the normal ones */
		glmUnitize(*shot);
		glmFacetNormals(*shot);
		glmVertexNormals(*shot, shot_angle);
	}
	else {
		print_error_log("Shot model not loaded");
	}
	print_verbose_log("Shot model is loaded");
}

/* update shot's box collision */
void atualizaCaixaColisaoTiro(game_item *item)
{
	/* total points collision box */
	for (unsigned int i = INITIALIZE_ZERO; i < 8; i++) {
		if (item->position.z > item->last_position.z) {
			item->collision.points[i].z += -item->acceleration;
			print_debug_log("Value of item->collision.points[%d].z: %d%d", item->collision.points[i].z);
			print_verbose_log("points collision is acelerated");
		}
		else {
			/* nothing to do */
		}
		if (item->position.z < item->last_position.z) {
			item->collision.points[i].z += item->acceleration;
			print_debug_log("Value of item->collision.points[%d].z: %d%d", item->collision.points[i].z);
			print_verbose_log("points collision is acelerated");
		}
		else {
			/* nothing to do */
		}
	}
	print_verbose_log("Shot box collision is updated");
}

/* loads shot's texture */
void load_shot_texture(Texture *shot_texture, char *filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = INITIALIZE_ZERO;
	unsigned int border = INITIALIZE_ZERO;
   	if (LoadTGA(shot_texture,filePath)) {
		glGenTextures(amount_of_textures, &shot_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, shot_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 shot_texture->bpp / 8, /* divides the bpp (byte per pixel) for each byte */
					 shot_texture->width,
					 shot_texture->height,
					 border,
					 shot_texture->type,
					 GL_UNSIGNED_BYTE,
					 shot_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
		print_verbose_log("Texture TGA is loaded");
	}
	else {
		print_error_log("Error loading shot texture");
	}
	print_verbose_log("Shot texture is loaded");
}

/* draws the shots */
void draw_shot(game_item *shots, Texture shot_texture, GLMmodel *shot)
{
	for (unsigned int i = INITIALIZE_ZERO; i < MAX_NUMBER_OF_SHOTS; i++) {
		if (shots[i].visible) {
			glPushMatrix();
				glTranslatef(shots[i].position.x,
							 shots[i].position.y,
							 shots[i].position.z);
				draw_model(SHOT_MODEL, shot_texture, shot);
			glPopMatrix();
			print_verbose_log("Shots is visible");
		}
		else {
			print_error_log("Error, shots not visible");
		}
	}
	print_verbose_log("Shots is drawned");
}

/* configures shot's collision box */
void configuraCaixaColisaoTiro(collision_box *dafault_collision_shot)
{
	for (unsigned int c = INITIALIZE_ZERO; c < 8; c++) {
		dafault_collision_shot->points[c].x *= SHOT_SCALE;
		dafault_collision_shot->points[c].y *= SHOT_SCALE;
		dafault_collision_shot->points[c].z *= SHOT_SCALE;

		print_debug_log("Value of dafault_collision_shot->points[%d].x: %d", dafault_collision_shot->points[c].x);
		print_debug_log("Value of dafault_collision_shot->points[%d].y: %d", dafault_collision_shot->points[c].y);
		print_debug_log("Value of dafault_collision_shot->points[%d].z: %d", dafault_collision_shot->points[c].z);
	}
	print_verbose_log("Shots collision box is configureted");
}

/* configures shot's acceleration */
void set_shot_acceleration(game_item *shots)
{
	for (unsigned int i = INITIALIZE_ZERO; i < MAX_NUMBER_OF_SHOTS; i++) {
		shots[i].acceleration = -0.9;
		print_debug_log("Value of shots[%d].acceleration: %d", shots[i].acceleration);
	}
	print_verbose_log("Shot acceleration is configureted");
}

#endif
