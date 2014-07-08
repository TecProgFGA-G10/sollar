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


#endif
