#ifndef TIROS_H
#define TIROS_H

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


#endif
