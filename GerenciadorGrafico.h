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
    //desenha um quadrado no fundo texturizado.
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
