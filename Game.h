/*
 * Solar 3D Técnicas de Programação
 * Esse programa carrega os modelos, texturas
 * e realiza o desenho da caixa ou tela do jogo,
 * também configurando as textures
 */

#ifndef GAME_H
#define GAME_H
#include <GL/glut.h>
#include "Solar_Utilities.h"
#include "Ship.h"
#include "Explosion.h"
#include "Meteor.h"
#include "Shots.h"
#include "logger.h"

/* loads the background texture */
void load_background_texture(Texture *background_texture, char *filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = 0;
	unsigned int border = 0;
	if (LoadTGA(background_texture, filePath)) {
		glGenTextures(amount_of_textures, &background_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, background_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 background_texture->bpp / 8, /* divides the bpp (byte per pixel) for each byte */
					 background_texture->width,
					 background_texture->height,
					 border,
					 background_texture->type,
					 GL_UNSIGNED_BYTE,
					 background_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		print_error_log("Error loading background texture");
	}
}

/* loads the game over texture */
void carrega_game_over_texture(Texture *game_over_texture, char* filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = 0;
	unsigned int border = 0;
	if (LoadTGA(game_over_texture, filePath)) {
		glGenTextures(amount_of_textures, &game_over_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, game_over_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 GL_RGBA,
					 game_over_texture->width,
					 game_over_texture->height,
					 border,
					 game_over_texture->type,
					 GL_UNSIGNED_BYTE,
					 game_over_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
		print_debug_log("gameover texture loaded");
	}
	else {
		print_error_log("Error loading gameover texture");
	}
}

/* loads the game texture */
void load_game_texture(Texture *game_continue_texture, char *filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = 0;
	unsigned int border = 0;
	if (LoadTGA(game_continue_texture, filePath)) {
		glGenTextures(amount_of_textures, &game_continue_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, game_continue_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 GL_RGBA,
					 game_continue_texture->width,
					 game_continue_texture->height,
					 border,
					 game_continue_texture->type,
					 GL_UNSIGNED_BYTE,
					 game_continue_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		print_error_log("Error loading continue texture");
	}
}

/* loads the game over texture */
void carrega_end_game_texture(Texture *end_game_texture, char *filePath)
{
	unsigned int amount_of_textures = 1;
	unsigned int level_of_detail = 0;
	unsigned int border = 0;
	if (LoadTGA(end_game_texture, filePath)) {
		glGenTextures(amount_of_textures, &end_game_texture->texture_id);
		glBindTexture(GL_TEXTURE_2D, end_game_texture->texture_id);
		glTexImage2D(GL_TEXTURE_2D,
					 level_of_detail,
					 GL_RGBA,
					 end_game_texture->width,
					 end_game_texture->height,
					 border,
					 end_game_texture->type,
					 GL_UNSIGNED_BYTE,
					 end_game_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		print_error_log("Error loading game over texture");
	}
}

/* configures textures */
void configuratextures(Texture **textures)
{
	load_ship_texture(textures[0], "data/aviao/textura_jato.tga");
	load_meteor_texture(textures[1], "data/meteoro/meteoro_Sphere.tga");
	load_shot_texture(textures[2], "data/tiro/tiro_Cube.tga");
	load_background_texture(textures[3], "data/fundo/fundo.tga");
	load_explosion_texture(textures[4], "data/explosao/explosao.tga");
	carrega_game_over_texture(textures[5], "data/gameover/gameover.tga");
	load_game_texture(textures[6], "data/gameover/continuarVerde.tga");
	load_game_texture(textures[7], "data/gameover/continuarVermelho.tga");
	carrega_game_over_texture(textures[8], "data/gameover/fimJogoVerde.tga");
	carrega_game_over_texture(textures[9], "data/gameover/fimJogoVermelho.tga");
}

/* draws game over plan */
void desenhaPlanoGameOver(Texture game_over_texture)
{
	glPushMatrix();
	glTranslatef(0, 0, 0); /* coordinates x, y, z */
	glBindTexture(GL_TEXTURE_2D, game_over_texture.texture_id);
	glScalef(0.6, 0.6, 0.6); /* coordinates x, y, z */

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); /* texture coordinates */
	glVertex3i(MINIMUN_X, MAXIMUM_Y, 0); /* coordinates x, y, z */

	glTexCoord2f(0.0, 0.0); /* texture coordinates */
	glVertex3i(MINIMUN_X, MINIMUM_Y, 0); /* coordinates x, y, z */

	glTexCoord2f(1.0, 0.0); /* texture coordinates */
	glVertex3i(MAXIMUM_X, MINIMUM_Y, 0); /* coordinates x, y, z */

	glTexCoord2f(1.0, 1.0); /* texture coordinates */
	glVertex3i(MAXIMUM_X, MAXIMUM_Y, 0); /* coordinates x, y, z */

	glEnd();
	glPopMatrix();
	glTranslatef(0, 0, 1); /* coordinates x, y, z */
}

/* draws continue button */
void desenhaBotaoContinuar(int game_over_selected,
						   Texture game_continue_green_texture,
						   Texture game_continue_red_texture)
{
	glPushMatrix();
	glTranslatef(-3, -2, 0); /* coordinates x, y, z */

	if (game_over_selected == CONTINUE) {
		glBindTexture(GL_TEXTURE_2D, game_continue_green_texture.texture_id);
		print_verbose_log("Continue button selected");
	}
	else {
		glBindTexture(GL_TEXTURE_2D, game_continue_red_texture.texture_id);
		print_verbose_log("Continue button deselected");
	}

	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0); /* texture coordinates */
	glVertex3f(1, 1, 1); /* coordinates x, y, z */

	glTexCoord2f(1.0, 0.0); /* texture coordinates */
	glVertex3f(1, -1, 1); /* coordinates x, y, z */

	glTexCoord2f(0.0, 0.0); /* texture coordinates */
	glVertex3f(-1, -1, 1); /* coordinates x, y, z */

	glTexCoord2f(0.0, 1.0); /* texture coordinates */
	glVertex3f(-1, 1, 1); /* coordinates x, y, z */

	glEnd();
	glPopMatrix();
}

/* draws the leave button */
void desenhaBotaoSair(int game_over_selected,
					  Texture game_over_green_texture,
					  Texture game_over_red_texture)
{
	glPushMatrix();
	glTranslatef(3,-2,0); /* coordinates x, y, z */

	if (game_over_selected == GAME_OVER) {
		glBindTexture(GL_TEXTURE_2D, game_over_green_texture.texture_id);
		print_verbose_log("Game over selected");
	}
	else {
		glBindTexture(GL_TEXTURE_2D, game_over_red_texture.texture_id);
		print_verbose_log("Game over deselected");
	}

	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0); /* texture coordinates */
	glVertex3f(1, 1, 1); /* coordinates x, y, z */

	glTexCoord2f(1.0, 0.0); /* texture coordinates */
	glVertex3f(1, -1, 1); /* coordinates x, y, z */

	glTexCoord2f(0.0, 0.0); /* texture coordinates */
	glVertex3f(-1, -1, 1); /* coordinates x, y, z */

	glTexCoord2f(0.0, 1.0); /* texture coordinates */
	glVertex3f(-1, 1, 1); /* coordinates x, y, z */

	glEnd();
	glPopMatrix();
}

/* initializes objects */
void inicializaObjetos(game_item *ship,
					   GLMmodel **meteor,
					   GLMmodel **explosion,
					   GLMmodel **shot,
					   collision_box *dafault_meteor_collision,
					   game_item *meteors,
					   game_item *explosions,
					   collision_box *dafault_collision_shot,
					   game_item *shots)
{
	carregaModeloship(ship);
	carregaModelometeor(meteor);
	carregaModeloExplosao(explosion);
	load_shot_model(shot);
	criaCaixaColisao(meteor[0], dafault_meteor_collision);
	configuraCaixaColisaometeor(dafault_meteor_collision);
	configuraAceleracaometeors(meteors);
	configuraTamanhoExplosoes(explosions);
	criaCaixaColisao(shot[0], dafault_collision_shot);
	configuraCaixaColisaoTiro(dafault_collision_shot);
	configuraAceleracaoTiros(shots);
}

#endif
