/*
 * Solar 3D Técnicas de Programação
 * Esse programa inicia o gráfico do jogo
 * colocando a visão da câmera, a iluminação,
 * a textura e o desenho de fundo
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H
#include <stdio.h>
#include <GL/glut.h>
#include "Solar_Utilities.h"
#include "logger.h"

/* initialize the library Glut */
void starts_glut(int argc, char **argv)
{
	glutInit(&argc, argv);
	/* 2 buffer mode and RGBA (accepts alpha) */
	glutInitDisplayMode(GRAPHIC_MODEL);
	glutInitWindowSize(WIDTH, HEIGHT); /* initial size of the window */
	glutInitWindowPosition(INITIALIZE_ZERO, INITIALIZE_ZERO); /* initial position of the window */
	glutCreateWindow(WINDOW_NAME); /* window's name created */
	glClearColor(INITIALIZE_ZERO, INITIALIZE_ZERO, INITIALIZE_ZERO, INITIALIZE_ZERO); /* clean collor, background collor */
	print_verbose_log("Lib Glut initialized");
}

/* initializes camera projection and perspective of the game */
void starts_camera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 75, 0.5, 120);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 10, 20,
			  0,  0,  0,
			  0,  1,  0);
	glViewport(INITIALIZE_ZERO, INITIALIZE_ZERO, WIDTH,HEIGHT);
	glEnable(GL_DEPTH_TEST); /* loads the depth test */
	glDepthFunc(GL_LESS);
	print_verbose_log("Camera initialized");
}

/* configure the ilumination */
void configures_lighting()
{
	float environment_color_light[] = {1, 1, 1, 1}; /* white light */
	float light_position[] = {0, 0, 0, 0.2}; /* light's position */
	glLightfv(GL_LIGHT0, GL_AMBIENT, environment_color_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	float material_environment[] = {0.2, 0.2, 0.2, 1};
	float diffuse_material[] = {1, 1, 1, 1};
	float specular_material[] = {1, 1, 1, 1};
	float brightness = 70;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_environment);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, brightness);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	print_verbose_log("Ilumination configureted");
}

/* draws models of th ship */
void draw_model(int tipo, Texture textura, GLMmodel *model)
{
	glBindTexture(GL_TEXTURE_2D, textura.texture_id);

	if (tipo == SHIP_MODEL) {
		glScalef(SHIP_SCALE, SHIP_SCALE, SHIP_SCALE);
		print_verbose_log("Scale factors specified");
	}
	else {
		print_error_log("Error, not possible draw model");
	}
	glmDraw(model, GLM_SMOOTH | GLM_TEXTURE);
	print_verbose_log("Model is drawned");
}

/* draws the background of the game */
void draws_fund(Texture background_texture)
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, background_texture.texture_id);
	glBegin(GL_QUADS);

	glColor3ub(255, 255, 255);
	glTexCoord2f(0.0, 0.0);
	glVertex3i(MINIMUN_X * 5, MAXIMUM_Y * 5, -50);
	glTexCoord2f(1.0, 0.0);
	glVertex3i(MINIMUN_X * 5, MINIMUM_Y * 5, -50);
	glTexCoord2f(1.0, 1.0);
	glVertex3i(MAXIMUM_X * 5, MINIMUM_Y * 5, -50);
	glTexCoord2f(0.0, 1.0);
	glVertex3i(MAXIMUM_X * 5, MAXIMUM_Y * 5, -50);

	glEnd();
	glPopMatrix();
	print_verbose_log("Background is drawned");
}

#endif
