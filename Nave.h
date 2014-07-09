#ifndef NAVE_H
#define NAVE_H

#include "SolarUtil.h"

Texture carregaTexturaNave(Texture texturaNave, char* filePath){
    if(LoadTGA(&texturaNave,filePath))
    {
        glGenTextures(1,&texturaNave.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaNave.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaNave.bpp / 8, texturaNave.width,
                     texturaNave.height, 0, texturaNave.type, GL_UNSIGNED_BYTE, texturaNave.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do jato.");
    }

	return texturaNave;
}

#endif
