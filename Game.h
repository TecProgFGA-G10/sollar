#ifndef GAME_H
#define GAME_H
#include "SolarUtil.h"

Texture carregaTexturaFundo(Texture texturaFundo, char *filePath){
    if(LoadTGA(&texturaFundo,filePath))
    {
        glGenTextures(1,&texturaFundo.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaFundo.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaFundo.bpp / 8, texturaFundo.width,
                     texturaFundo.height, 0, texturaFundo.type, GL_UNSIGNED_BYTE, texturaFundo.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fundo");
    }
	return texturaFundo;
}

Texture carregaTexturaGameOver(Texture texturaGameOver, char* filePath){
    if(LoadTGA(&texturaGameOver,filePath))
    {
        glGenTextures(1,&texturaGameOver.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaGameOver.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturaGameOver.width,
                     texturaGameOver.height, 0, texturaGameOver.type, GL_UNSIGNED_BYTE, texturaGameOver.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do gameover");
    }
	return texturaGameOver;
}


Texture carregaTexturaJogo(Texture continuaJogo, char* filePath){
    if(LoadTGA(&continuaJogo,filePath))
    {
        glGenTextures(1,&continuaJogo.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, continuaJogo.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, continuaJogo.width,
                     continuaJogo.height, 0, continuaJogo.type, GL_UNSIGNED_BYTE, continuaJogo.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura continuar jogo");
    }
	return continuaJogo;
}

Texture carregaTexturaFimJogo(Texture fimDeJogo, char* filePath){
    if(LoadTGA(&fimDeJogo,filePath))
    {
        glGenTextures(1,&fimDeJogo.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, fimDeJogo.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fimDeJogo.width,
                     fimDeJogo.height, 0, fimDeJogo.type, GL_UNSIGNED_BYTE, fimDeJogo.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do fimDeJogo");
    }
	return fimDeJogo;
}
#endif
