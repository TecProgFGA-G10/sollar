#ifndef EXPLOSAO_H
#define EXPLOSAO_H

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


#endif
