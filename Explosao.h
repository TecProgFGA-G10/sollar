#ifndef EXPLOSAO_H
#define EXPLOSAO_H
#include "GerenciadorGrafico.h"

#include "SolarUtil.h"

int posicaoVaziaExplosoes(itemDeJogo *explosoes)
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        if(!explosoes[i].visivel)
            return i;
    }
    return -1;//caso n�o encontra retorna -1;
}

Texture carregaTexturaExplosao(Texture texturaExplosao, char* filePath){
    if(LoadTGA(&texturaExplosao,filePath))
    {
        glGenTextures(1,&texturaExplosao.texID);
        glBindTexture(GL_TEXTURE_2D, texturaExplosao.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturaExplosao.width,
                     texturaExplosao.height, 0, texturaExplosao.type, GL_UNSIGNED_BYTE, texturaExplosao.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura da explosao");
    }
	return texturaExplosao;
}

void desenhaExplosoes(itemDeJogo *explosoes,Texture texturaExplosao, GLMmodel *explosao)
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        if(explosoes[i].visivel)
        {
            glColor3ub(255,255,255);
            glPushMatrix();

                glTranslatef(explosoes[i].posicao.x,explosoes[i].posicao.y,explosoes[i].posicao.z);
                glScalef(explosoes[i].tamanho,explosoes[i].tamanho,explosoes[i].tamanho);
                desenhaModelo(MODELO_EXPLOSAO,texturaExplosao,explosao);
            glPopMatrix();
        }
    }
}


#endif
