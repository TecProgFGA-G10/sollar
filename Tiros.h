#ifndef TIROS_H
#define TIROS_H

#include "SolarUtil.h"

int posicaoVaziaTiros(itemDeJogo *tiros)
{
    int i =0;
    for(i = 0; i < NUM_MAX_TIROS; i++)
    {
        //encontra o primeiro tiro vazio e o retorna.
        if(!tiros[i].visivel)
            return i;
    }
    return -1;//caso n�o encontra retorna -1;
}

void atualizaCaixaColisaoTiro(itemDeJogo *item)
{
    //� diferente pois o tiro se move em um valor diferente no eixo z.
    int i =0;
    for(i = 0; i < 8; i++)//tem 8 pontos da caixa de colisao.
    {
        if(item->posicao.z > item->posicaoAnterior.z)
            item->colisao.pontos[i].z+= -item->aceleracao;//(item->posicao.x - item->posicaoAnterior.x);

        if(item->posicao.z < item->posicaoAnterior.z)
            item->colisao.pontos[i].z+= item->aceleracao;//(item->posicao.x - item->posicaoAnterior.x);
    }
}

Texture carregaTexturaTiro(Texture texturaTiro, char * filePath){
   if(LoadTGA(&texturaTiro,filePath))
    {
        glGenTextures(1,&texturaTiro.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaTiro.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaTiro.bpp / 8, texturaTiro.width,
                     texturaTiro.height, 0, texturaTiro.type, GL_UNSIGNED_BYTE, texturaTiro.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do tiro");
    }

	return texturaTiro;
}

#endif
