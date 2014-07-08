#ifndef METEORO_H
#define METEORO_H
#include <GL/glut.h>
#include "Colisao.h"


int posicaoVaziaMeteoros(itemDeJogo *meteoros)
{
    int i =0;
    for(i = 0; i < NUM_MAX_METEOROS; i++)
    {
        //encontra o primeiro meteoro vazio e o retorna.
        if(!meteoros[i].visivel)
            return i;
    }
    return -1;//caso n�o encontra retorna -1;
}

void enviaMeteoro(itemDeJogo *meteoros, int meteorosEnviar, caixaColisao colisaoMeteoroDefault){
    int i =0;
    for(i = 0; i < meteorosEnviar; i++)//envia meteorosEnviar meteoros
    {
       int pos = posicaoVaziaMeteoros(meteoros);
        if(pos >=0)//se tem uma pos disponivel..
        {
            meteoros[pos].visivel = TRUE;
            meteoros[pos].posicao.z = (Z_INICIAL_METEORO-10) - rand()%(Z_INICIAL_METEORO - -20);
            meteoros[pos].posicao.x = MINIMO_X + rand()%(MAXIMO_X-MINIMO_X);
            meteoros[pos].posicao.y = MINIMO_Y + rand()%(MAXIMO_Y-MINIMO_Y) ;
            //guarda a posi��o anterior para calcular a caixa de colis�o para o metoro
            //em uma posi��o diferente da <0,0,0>
            meteoros[pos].posicaoAnterior.z = meteoros[pos].posicao.z;
            meteoros[pos].posicaoAnterior.x = meteoros[pos].posicao.x;
            meteoros[pos].posicaoAnterior.y = meteoros[pos].posicao.y;

            int c =0;
            for(c =0; c< 8; c++)
            //configura a caixa de colis�o, para um meteoro na posi��o <0,0,0>
            {
                meteoros[pos].colisao.pontos[c].x = colisaoMeteoroDefault.pontos[c].x;
                meteoros[pos].colisao.pontos[c].y = colisaoMeteoroDefault.pontos[c].y;
                meteoros[pos].colisao.pontos[c].z = colisaoMeteoroDefault.pontos[c].z;
            }
            //calcula a posi��o da caixa para posi��o do meteoro.
            setaCaixaColisao(&meteoros[pos].colisao, meteoros[pos].posicao);
        }
	}
}

Texture carregaTexturaMeteoro(Texture texturaMetoro,char * filePath){
    if(LoadTGA(&texturaMetoro,filePath))
    {
        glGenTextures(1,&texturaMetoro.texID);//cria uma textura..
        glBindTexture(GL_TEXTURE_2D, texturaMetoro.texID);
        glTexImage2D(GL_TEXTURE_2D, 0, texturaMetoro.bpp / 8, texturaMetoro.width,
                     texturaMetoro.height, 0, texturaMetoro.type, GL_UNSIGNED_BYTE, texturaMetoro.imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        printf("\nErro carregando a textura do meteoro");
    }
return texturaMetoro;
}

#endif
