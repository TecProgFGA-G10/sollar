#ifndef GAME_H
#define GAME_H
#include <GL/glut.h>
#include "SolarUtil.h"
#include "Nave.h"
#include "Explosao.h"
#include "Meteoro.h"
#include "Tiros.h"

void carregaTexturaFundo(Texture *texturaFundo, char *filePath){
    if(LoadTGA(texturaFundo,filePath))
    {
        glGenTextures(1,&texturaFundo->texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaFundo->texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaFundo->bpp / 8, texturaFundo->width,
                     texturaFundo->height, 0, texturaFundo->type, GL_UNSIGNED_BYTE, texturaFundo->imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fundo");
    }

}

void carregaTexturaGameOver(Texture *texturaGameOver, char* filePath){
    if(LoadTGA(texturaGameOver,filePath))
    {
        glGenTextures(1,&texturaGameOver->texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaGameOver->texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturaGameOver->width,
                     texturaGameOver->height, 0, texturaGameOver->type, GL_UNSIGNED_BYTE, texturaGameOver->imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do gameover");
    }
}


void carregaTexturaJogo(Texture *continuaJogo, char* filePath){
    if(LoadTGA(continuaJogo,filePath))
    {
        glGenTextures(1,&continuaJogo->texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, continuaJogo->texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, continuaJogo->width,
                     continuaJogo->height, 0, continuaJogo->type, GL_UNSIGNED_BYTE, continuaJogo->imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura continuar jogo");
    }
}

void carregaTexturaFimJogo(Texture *fimDeJogo, char* filePath){
    if(LoadTGA(fimDeJogo,filePath))
    {
        glGenTextures(1,&fimDeJogo->texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, fimDeJogo->texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fimDeJogo->width,
                     fimDeJogo->height, 0, fimDeJogo->type, GL_UNSIGNED_BYTE, fimDeJogo->imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fimDeJogo");
    }
}

void configuraTexturas(Texture *texturaNave,Texture *texturaMeteoro,Texture *texturaTiro,Texture *texturaFundo,Texture *texturaExplosao,Texture *texturaGameOver,Texture *continuaJogoVerde,Texture *continuaJogoVermelho, Texture *fimJogoVerde, Texture *fimJogoVermelho)
{
    carregaTexturaNave(texturaNave,"data/aviao/textura_jato.tga");

    carregaTexturaMeteoro(texturaMeteoro,"data/meteoro/meteoro_Sphere.tga");

    carregaTexturaTiro(texturaTiro,"data/tiro/tiro_Cube.tga");

    carregaTexturaFundo(texturaFundo,"data/fundo/fundo.tga");

    carregaTexturaExplosao(texturaExplosao,"data/explosao/explosao.tga");

    carregaTexturaGameOver(texturaGameOver,"data/gameover/gameover.tga");

    carregaTexturaJogo(continuaJogoVerde,"data/gameover/continuarVerde.tga");

    carregaTexturaJogo(continuaJogoVermelho,"data/gameover/continuarVermelho.tga");

    carregaTexturaFimJogo(fimJogoVerde,"data/gameover/fimJogoVerde.tga");

    carregaTexturaFimJogo(fimJogoVermelho,"data/gameover/fimJogoVermelho.tga");
}


void desenhaPlanoGameOver(Texture texturaGameOver){
    glPushMatrix();
        glTranslatef(0,0,0);
        glBindTexture(GL_TEXTURE_2D, texturaGameOver.texID);
        glScalef(0.6,0.6,0.6);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0,1.0);
            glVertex3i(MINIMO_X,MAXIMO_Y,0);
            glTexCoord2f(0.0,0.0);
            glVertex3i(MINIMO_X,MINIMO_Y,0);
            glTexCoord2f(1.0,0.0);
            glVertex3i(MAXIMO_X,MINIMO_Y,0);
            glTexCoord2f(1.0,1.0);
            glVertex3i(MAXIMO_X,MAXIMO_Y, 0);
        glEnd();
    glPopMatrix();
    glTranslatef(0,0,1);
}

void desenhaBotaoContinuar(int gameOverSelecionado, Texture continuaJogoVerde, Texture continuaJogoVermelho){
    glPushMatrix();
        glTranslatef(-3,-2,0);
        if(gameOverSelecionado == CONTINUAR)
        {
            glBindTexture(GL_TEXTURE_2D, continuaJogoVerde.texID);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, continuaJogoVermelho.texID);
        }

        glBegin(GL_QUADS);
            glTexCoord2f(1.0,1.0);
            glVertex3f(1,1,1);

            glTexCoord2f(1.0,0.0);
            glVertex3f(1,-1,1);

            glTexCoord2f(0.0,0.0);
            glVertex3f(-1,-1,1);

            glTexCoord2f(0.0,1.0);
            glVertex3f(-1,1,1);

        glEnd();
	glPopMatrix();
}

void desenhaBotaoSair(int gameOverSelecionado, Texture fimJogoVerde, Texture fimJogoVermelho){
    glPushMatrix();
        glTranslatef(3,-2,0);
        if(gameOverSelecionado == GAME_OVER)
        {
            glBindTexture(GL_TEXTURE_2D, fimJogoVerde.texID);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, fimJogoVermelho.texID);
        }
        glBegin(GL_QUADS);
            glTexCoord2f(1.0,1.0);
            glVertex3f(1,1,1);

            glTexCoord2f(1.0,0.0);
            glVertex3f(1,-1,1);

            glTexCoord2f(0.0,0.0);
            glVertex3f(-1,-1,1);

            glTexCoord2f(0.0,1.0);
            glVertex3f(-1,1,1);

        glEnd();
    glPopMatrix();

}

#endif
