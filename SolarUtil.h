#ifndef SOLAR_UTIL_H
#define SOLAR_UTIL_H
#define NUM_MAX_TIROS 10 //n�mero m�ximo de tiros que podem ser disparados
#define NUM_MAX_METEOROS 20 //n�mero m�ximo de meteoros que podem vir
#define FALSE 0 
#define TRUE 1

#define MAXIMO_DESENHO_TIRO -30//distancia maxima para tratar um tiro, no eixo Z
#define MAXIMO_DESENHO_METEORO 20//distancia m�xima que o meteoro se movimenta no eixo Z, vindo em dire��o � nave.
#define Z_INICIAL_METEORO -28//posi��o inicial dos meteoros.

#define CAMERA_Z 8//distancia da camera no eixo Z
#define VAR_CAMERA 4//inclina��o da camera

//para definir o 'campo' do jogo, representando cada canto
#define MINIMO_X -14
#define MAXIMO_X 14
#define MINIMO_Y -14
#define MAXIMO_Y 14

#define AUMENTO_VELOCIDADE_METEORO 0.05//a varia��o de velocidade do meteoro.
#define ESCALA_AVIAO 2
#define ESCALA_METEORO 1
#define ESCALA_TIRO 1

#define NOVO_METEORO 88//para representar um evento para enviar novo meteoro
#define DIMINUI_EXPLOSAO 99//para o timer, diminuir o tamanho da explosao.
#define VALOR_PONTO 10//valor da pontuacao para ser somado.
#define MODELO_NAVE 1//para desenhar o modelo da nave.
#define MODELO_METEORO 2//para desenhar o modelo do meteoro.
#define MODELO_TIRO 3//para desenhar o modelo do tiro.
#define MODELO_EXPLOSAO 4//para desenhar o modelo da explosao.
#define EXPLOSAO_DECRESCIMENTO 0.1//ser� diminuida de 0.1 a cada intervalo de tempo.

#define GAME_OVER 666//para representar que selecionou o "botao" para game over.
#define CONTINUAR 777//para representar que selecionou o "botao" para game CONTINUE.


#define MODO_GRAFICO  GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH //modo gr�fico de buffer duplo, cores RGBA e teste de profundidade.
#define LARGURA 800
#define ALTURA 600
#define NOME_JANELA "Sollar"

typedef struct{
    float x;
    float y;
    float z;
}ponto;

typedef struct
{
 ponto pontos[8];
}caixaColisao;


typedef struct{
    GLMmodel *modelo;
    ponto posicao;
    caixaColisao colisao;
    ponto posicaoAnterior;
    int visivel;
    float aceleracao;
    float tamanho;
    float rotacao;
    float rotX;
    float rotY;
    float rotZ;
}itemDeJogo;

typedef struct
{
        GLubyte * imageData;
        GLuint  bpp;
        GLuint  width;
        GLuint  height;
        GLuint  texID;
        GLuint  type;
} Texture;

typedef struct
{
        GLubyte Header[12];
} TGAHeader;

typedef struct
{
        GLubyte         header[6];                                                              
        GLuint          bytesPerPixel;                                                  
        GLuint          imageSize;                                                              
        GLuint          temp;                                                                   
        GLuint          type;
        GLuint          Height;                                                                 
        GLuint          Width;                                                                  
        GLuint          Bpp;   
} TGA;

TGAHeader tgaheader;
TGA tga;

GLubyte uTGAcompare[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

int LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA){
        if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
        {
        	if(fTGA !=NULL)
                {
                        fclose(fTGA);                }
                return FALSE;
        }

        texture->width  = tga.header[1] * 256 + tga.header[0];
        texture->height = tga.header[3] * 256 + tga.header[2];
        texture->bpp    = tga.header[4];
        tga.Width       = texture->width;
        tga.Height      = texture->height;
        tga.Bpp                 = texture->bpp;

        if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))
        {
                if(fTGA != NULL)
                {
                        fclose(fTGA);
                }
                return FALSE;
        }

        if(texture->bpp == 24)
                texture->type   = GL_RGB;
       else
                texture->type   = GL_RGBA;

        tga.bytesPerPixel       = (tga.Bpp / 8);
        tga.imageSize           = (tga.bytesPerPixel * tga.Width * tga.Height);
        texture->imageData      = (GLubyte *)malloc(tga.imageSize);

        if(texture->imageData == NULL)
        {
                fclose(fTGA);
                return FALSE;
        }

        GLuint pixelcount       = tga.Height * tga.Width;
        GLuint currentpixel     = 0;
        GLuint currentbyte      = 0;
        GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);
        do
        {
                GLubyte chunkheader = 0;

                if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)
                {
                        if(fTGA != NULL)
                        {
                                fclose(fTGA);
                        }
                        if(texture->imageData != NULL)
                        {
                                free(texture->imageData);
                        }
                        return FALSE;
                }

                if(chunkheader < 128){
                        chunkheader++;
                        short counter;
                        for(counter = 0; counter < chunkheader; counter++)
                        {
                                if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
                                {
                                        if(fTGA != NULL)
                                        {
                                                fclose(fTGA);
                                        }

                                        if(colorbuffer != NULL)
                                        {
                                                free(colorbuffer);
                                        }

                                        if(texture->imageData != NULL)
                                        {
                                                free(texture->imageData);
                                        }
                                        return FALSE;
                                }
                                 texture->imageData[currentbyte          ] = colorbuffer[2];
                                texture->imageData[currentbyte + 1      ] = colorbuffer[1];
                                texture->imageData[currentbyte + 2      ] = colorbuffer[0];

                                if(tga.bytesPerPixel == 4)
                                {
                                        texture->imageData[currentbyte + 3] = colorbuffer[3];
                                }

                                currentbyte += tga.bytesPerPixel;
                                currentpixel++;

                                if(currentpixel > pixelcount)
                                {

                                        if(fTGA != NULL)
                                        {
                                                fclose(fTGA);
                                        }

                                        if(colorbuffer != NULL)
                                        {
                                                free(colorbuffer);
                                        }

                                        if(texture->imageData != NULL)
                                        {
                                                free(texture->imageData);
                                        }

                                        return FALSE;
                                }
                        }
                }
                else
                {
                        chunkheader -= 127;
                        if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
                        {

                                if(fTGA != NULL)
                                {
                                        fclose(fTGA);
                                }

                                if(colorbuffer != NULL)
                                {
                                        free(colorbuffer);
                                }
                                if(texture->imageData != NULL)
                                {
                                        free(texture->imageData);
                                }

                                return FALSE;
                        }
            short counter;
                        for( counter = 0; counter < chunkheader; counter++)
                        {
                                texture->imageData[currentbyte          ] = colorbuffer[2];
                                texture->imageData[currentbyte + 1      ] = colorbuffer[1];
                                texture->imageData[currentbyte + 2      ] = colorbuffer[0];

                                if(tga.bytesPerPixel == 4)                                {
                                        texture->imageData[currentbyte + 3] = colorbuffer[3];
                                }

                                currentbyte += tga.bytesPerPixel;
                                currentpixel++;

                                if(currentpixel > pixelcount)
                                {

                                        if(fTGA != NULL)
                                        {
                                                fclose(fTGA);
                                        }

                                        if(colorbuffer != NULL)
                                        {
                                                free(colorbuffer);
                                        }

                                        if(texture->imageData != NULL)
                                        {
                                                free(texture->imageData);
                                        }

                                        return FALSE;
                                }
                        }
                }
        }

        while(currentpixel < pixelcount);
        fclose(fTGA);
        return TRUE;
}

int LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)
{                                                                                                                                                       // 
        if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
        {
                if(fTGA != NULL)
                {
                        fclose(fTGA);
                }
                return FALSE;
        }

        texture->width  = tga.header[1] * 256 + tga.header[0];
        texture->height = tga.header[3] * 256 + tga.header[2];
        texture->bpp    = tga.header[4];
        tga.Width               = texture->width;
        tga.Height              = texture->height;
        tga.Bpp                 = texture->bpp;

        if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))
        {
                if(fTGA != NULL)
                {
                        fclose(fTGA);
                }
                return FALSE;
        }

        if(texture->bpp == 24)
                texture->type   = GL_RGB;
        else
                texture->type   = GL_RGBA;

        tga.bytesPerPixel       = (tga.Bpp / 8);
        tga.imageSize           = (tga.bytesPerPixel * tga.Width * tga.Height);
        texture->imageData      = (GLubyte *)malloc(tga.imageSize);

        if(texture->imageData == NULL)
        {
                fclose(fTGA);
                return FALSE;
        }

        if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)  // Attempt to read image data
        {
                if(texture->imageData != NULL)
                {
                        free(texture->imageData);
                }
                fclose(fTGA);
                return FALSE;
        }

        GLuint cswap;
        for( cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
        {
                texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
                texture->imageData[cswap] ^= texture->imageData[cswap+2];
        }

        fclose(fTGA);
        return TRUE;
}


int LoadTGA(Texture * texture, char * filename)
{
        FILE * fTGA;
        fTGA = fopen(filename, "rb");

        if(fTGA == NULL)
        {
                return FALSE;
        }

        if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
        {
                if(fTGA != NULL)
                {
                        fclose(fTGA);
                }
                return FALSE;
        }

        if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0){
                LoadUncompressedTGA(texture, filename, fTGA);
        }
        else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
        {
                LoadCompressedTGA(texture, filename, fTGA);
        }
        else
        {
                fclose(fTGA);
                return FALSE;
        }
        return TRUE;
}


#endif
