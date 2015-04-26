/*
 * Solar 3D Técnicas de Programação
 * Esse programa inicia o gráfico do jogo
 * colocando a visão da câmera, a iluminação,
 * a textura e o desenho de fundo
 */

#ifndef DISPLAY_MANAGER.H
#define DISPLAY_MANAGER.H

#include <stdio.h>
#include <GL/glut.h>
#include "Solar_Utilities.h"

/* initialize the library Glut */
void iniciaGlut(int argc, char **argv)
{
	glutInit(&argc, argv);
	/* 2 buffer mode and RGBA (accepts alpha) */
	glutInitDisplayMode(GRAPHIC_MODEL);
	glutInitWindowSize(WIDTH, HEIGHT); /* initial size of the window */
	glutInitWindowPosition(0, 0); /* initial position of the window */
	glutCreateWindow(WINDOW_NAME); /* window's name created */
	glClearColor(0, 0, 0, 0); /* clean collor, background collor */
}

/* initializes camera */
void iniciaCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 75, 0.5, 120);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 10, 20,
			  0,  0,  0,
			  0,  1,  0);
	glViewport(0, 0, WIDTH,HEIGHT);
	glEnable(GL_DEPTH_TEST); /* loads the depth test */
	glDepthFunc(GL_LESS);
}

/* configure the ilumination */
void configuraIluminacao()
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
}

/* draws models */
void desenhaModelo(int tipo, Texture textura, GLMmodel *model)
{
	glBindTexture(GL_TEXTURE_2D, textura.texID);

	if (tipo == AIRCRAFT_MODEL) {
		glScalef(AIRCRAFT_SCALE, AIRCRAFT_SCALE, AIRCRAFT_SCALE);
	}
	else {
		/* nothing to do */
	}
	glmDraw(model, GLM_SMOOTH | GLM_TEXTURE);
}

/* draws the background */
void desenhaFundo(Texture background_texture)
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, background_texture.texID);
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
}

#endif
