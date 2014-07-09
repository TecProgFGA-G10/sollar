#ifndef GERENCIADOR_GRAFICO_H
#define GERENCIADOR_GRAFICO_H

#include <stdio.h>
#include <GL/glut.h>

#include "SolarUtil.h"

void iniciaGlut(){
	glutInitDisplayMode(MODO_GRAFICO); //diz que o modo � com 2 buffer e RGBA (aceita alpha)
    glutInitWindowSize(LARGURA,ALTURA);//tamanho inicial da janela
    glutInitWindowPosition(0,0);//poisi��o inicial da janela
    glutCreateWindow(NOME_JANELA);//nome da janela criada
    glClearColor(0,0,0,0);//cor de limpeza, ou seja, do fundo

}

void iniciaCamera(){
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,75,0.5,120);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0,10,20,
              0,0,0,
              0,1,0);
    glViewport(0,0,LARGURA,ALTURA);
    glEnable(GL_DEPTH_TEST);//habilita o teste de profundidade
    glDepthFunc(GL_LESS);

}

void configuraIluminacao()
{
    float corAmbienteLuz[] = {1,1,1,1};//uma luz branca.
    float posicaoLuz[] = {0,0,0 ,0.2};//posicao da luz.

    glLightfv(GL_LIGHT0, GL_AMBIENT, corAmbienteLuz);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
    float materialAmbiente[] = {0.2,0.2,0.2,1};
    float materialDifusa[] = {1,1,1,1};
    float materialSpecular[] = {1,1,1,1};
    float brilho = 70;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDifusa);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);

    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,brilho);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void desenhaModelo(int tipo, Texture textura,GLMmodel *modelo)
{

    glBindTexture(GL_TEXTURE_2D, textura.texID);

    if(tipo == MODELO_NAVE)
    {
        glScalef(ESCALA_AVIAO,ESCALA_AVIAO,ESCALA_AVIAO);
    }
    glmDraw(modelo, GLM_SMOOTH | GLM_TEXTURE );

}

void desenhaFundo(Texture texturaFundo)
{
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texturaFundo.texID);
        glBegin(GL_QUADS);
            glColor3ub(255,255,255);
            glTexCoord2f(0.0,0.0);
            glVertex3i(MINIMO_X*5,MAXIMO_Y*5,-50);
            glTexCoord2f(1.0,0.0);
            glVertex3i(MINIMO_X*5,MINIMO_Y*5,-50);
            glTexCoord2f(1.0,1.0);
            glVertex3i(MAXIMO_X*5,MINIMO_Y*5,-50);
            glTexCoord2f(0.0,1.0);
            glVertex3i(MAXIMO_X*5,MAXIMO_Y*5, -50);
        glEnd();
    glPopMatrix();
}



#endif
