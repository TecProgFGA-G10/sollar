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

/* loads the background texture */
void carregaTexturaFundo(Texture *background_texture, char *filePath)
{
	if (LoadTGA(background_texture, filePath)) {
		glGenTextures(1, &background_texture->texID);
		glBindTexture(GL_TEXTURE_2D, background_texture->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 background_texture->bpp / 8,
					 background_texture->width,
					 background_texture->height,
					 0,
					 background_texture->type,
					 GL_UNSIGNED_BYTE,
					 background_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do fundo");
	}
}

/* loads the game over texture */
void carregagame_over_texture(Texture *game_over_texture, char* filePath)
{
	if (LoadTGA(game_over_texture, filePath)) {
		glGenTextures(1, &game_over_texture->texID);
		glBindTexture(GL_TEXTURE_2D, game_over_texture->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 game_over_texture->width,
					 game_over_texture->height,
					 0,
					 game_over_texture->type,
					 GL_UNSIGNED_BYTE,
					 game_over_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do gameover");
	}
}

/* loads the game texture */
void carregaTexturaJogo(Texture *game_continue_texture, char *filePath)
{
	if (LoadTGA(game_continue_texture, filePath)) {
		glGenTextures(1, &game_continue_texture->texID);
		glBindTexture(GL_TEXTURE_2D, game_continue_texture->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 game_continue_texture->width,
					 game_continue_texture->height,
					 0,
					 game_continue_texture->type,
					 GL_UNSIGNED_BYTE,
					 game_continue_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura continuar jogo");
	}
}

/* loads the game over texture */
void carregaTexturaFimJogo(Texture *game_over_texture, char *filePath)
{
	if (LoadTGA(game_over_texture, filePath)) {
		glGenTextures(1, &game_over_texture->texID);
		glBindTexture(GL_TEXTURE_2D, game_over_texture->texID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 game_over_texture->width,
					 game_over_texture->height,
					 0,
					 game_over_texture->type,
					 GL_UNSIGNED_BYTE,
					 game_over_texture->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		printf("\nErro carregando a textura do game_over_texture");
	}
}

/* configures textures */
void configuratextures(Texture **textures)
{
	carregaTexturaaircraft(textures[0], "data/aviao/textura_jato.tga");
	carregaTexturameteor(textures[1], "data/meteoro/meteoro_Sphere.tga");
	carregaTexturaTiro(textures[2], "data/tiro/tiro_Cube.tga");
	carregaTexturaFundo(textures[3], "data/fundo/fundo.tga");
	carregaexplosion_texture(textures[4], "data/explosao/explosao.tga");
	carregagame_over_texture(textures[5], "data/gameover/gameover.tga");
	carregaTexturaJogo(textures[6], "data/gameover/continuarVerde.tga");
	carregaTexturaJogo(textures[7], "data/gameover/continuarVermelho.tga");
	carregaTexturaFimJogo(textures[8], "data/gameover/fimJogoVerde.tga");
	carregaTexturaFimJogo(textures[9], "data/gameover/fimJogoVermelho.tga");
}

/* draws game over plan */
void desenhaPlanoGameOver(Texture game_over_texture)
{
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glBindTexture(GL_TEXTURE_2D, game_over_texture.texID);
	glScalef(0.6, 0.6, 0.6);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3i(MINIMUN_X,MAXIMUM_Y, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3i(MINIMUN_X,MINIMUM_Y, 0);

	glTexCoord2f(1.0, 0.0);
	glVertex3i(MAXIMUM_X,MINIMUM_Y, 0);

	glTexCoord2f(1.0, 1.0);
	glVertex3i(MAXIMUM_X,MAXIMUM_Y, 0);

	glEnd();
	glPopMatrix();
	glTranslatef(0, 0, 1);
}

/* draws continue button */
void desenhaBotaoContinuar(int game_over_selected,
						   Texture game_continue_green_texture,
						   Texture game_continue_red_texture)
{
	glPushMatrix();
	glTranslatef(-3, -2, 0);

	if (game_over_selected == CONTINUE) {
		glBindTexture(GL_TEXTURE_2D, game_continue_green_texture.texID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, game_continue_red_texture.texID);
	}

	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1, 1, 1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(1, -1, 1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1, -1, 1);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1, 1, 1);

	glEnd();
	glPopMatrix();
}

/* draws the leave button */
void desenhaBotaoSair(int game_over_selected,
					  Texture game_over_green_texture,
					  Texture game_over_red_texture)
{
	glPushMatrix();
	glTranslatef(3,-2,0);

	if (game_over_selected == GAME_OVER) {
		glBindTexture(GL_TEXTURE_2D, game_over_green_texture.texID);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, game_over_red_texture.texID);
	}

	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1, 1, 1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(1, -1, 1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1, -1, 1);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1, 1, 1);

	glEnd();
	glPopMatrix();
}

/* initializes objects */
void inicializaObjetos(game_item *aircraft,
					   GLMmodel **meteor,
					   GLMmodel **explosion,
					   GLMmodel **shot,
					   collision_box *dafault_meteor_collision,
					   game_item *meteors,
					   game_item *explosions,
					   collision_box *dafault_collision_shot,
					   game_item *shots)
{
	carregaModeloaircraft(aircraft);
	carregaModelometeor(meteor);
	carregaModeloExplosao(explosion);
	carregaModeloTiro(shot);
	criaCaixaColisao(meteor[0], dafault_meteor_collision);
	configuraCaixaColisaometeor(dafault_meteor_collision);
	configuraAceleracaometeors(meteors);
	configuraTamanhoExplosoes(explosions);
	criaCaixaColisao(shot[0], dafault_collision_shot);
	configuraCaixaColisaoTiro(dafault_collision_shot);
	configuraAceleracaoTiros(shots);
}

#endif
