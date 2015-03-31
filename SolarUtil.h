#ifndef SOLAR_UTIL_H
#define SOLAR_UTIL_H
#define NUM_MAX_TIROS 10 /* max numbers of shoots */
#define NUM_MAX_METEOROS 20 /* max number of meteors that can appear */
#define FALSE 0
#define TRUE 1
#define MAXIMO_DESENHO_TIRO -30 /* max distance to treat a shoot, on the z axis */
#define MAXIMO_DESENHO_METEORO 20 /* max distance the meteor moves on z axis, coming to the ship */
#define Z_INICIAL_METEORO -28 /* initial position of meteors */
#define CAMERA_Z 8 /* camera distance on z axis */
#define VAR_CAMERA 4 /* camera inclination */

/* to define the field of the game, representing each corner */
#define MINIMO_X -14
#define MAXIMO_X 14
#define MINIMO_Y -14
#define MAXIMO_Y 14
#define AUMENTO_VELOCIDADE_METEORO 0.05 /* meteor's speed variation */
#define ESCALA_AVIAO 2
#define ESCALA_METEORO 1
#define ESCALA_TIRO 1
#define NOVO_METEORO 88 /* send the event to new meteor */
#define DIMINUI_EXPLOSAO 99 /* stop the timer, make the explosion smaller */
#define VALOR_PONTO 10 /* pontuation to be added */
#define MODELO_NAVE 1 /* draw the ship model */
#define MODELO_METEORO 2 /* draw the meteor model */
#define MODELO_TIRO 3 /* draw the shoot model */
#define MODELO_EXPLOSAO 4 /* draw the explosion model */
#define EXPLOSAO_DECRESCIMENTO 0.1 /* will loose 0.1 each time count */

#define GAME_OVER 666 /* game over button selected */
#define CONTINUAR 777 /* continue button selected */

/* rgba colors and depth test */
#define MODO_GRAFICO  GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH
#define LARGURA 800
#define ALTURA 600
#define NOME_JANELA "Sollar"

typedef struct {
	float x;
	float y;
	float z;
} ponto;

typedef struct {
	ponto pontos[8];
} caixaColisao;


typedef struct {
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
} itemDeJogo;

typedef struct {
	GLubyte * imageData;
	GLuint  bpp;
	GLuint  width;
	GLuint  height;
	GLuint  texID;
	GLuint  type;
} Texture;

typedef struct {
	GLubyte Header[12];
} TGAHeader;

typedef struct {
	GLubyte header[6];
	GLuint bytesPerPixel;
	GLuint imageSize;
	GLuint temp;
	GLuint type;
	GLuint Height;
	GLuint Width;
	GLuint Bpp;
} TGA;

TGAHeader tgaheader;
TGA tga;

GLubyte uTGAcompare[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

int LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)
{
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		if (fTGA !=NULL) {
			fclose(fTGA);
		}
		else {
			/* nothing to do */
		}
			return FALSE;
	}
	else {
		/* nothing to do */
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp    = tga.header[4];
	tga.Width       = texture->width;
	tga.Height      = texture->height;
	tga.Bpp         = texture->bpp;

	if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32))) {
		if (fTGA != NULL) {
			fclose(fTGA);
		}
		return FALSE;
	}
	else {
		/* nothing to do */
	}

	if (texture->bpp == 24) {
		texture->type = GL_RGB;
	}
	else {
		texture->type   = GL_RGBA;
		tga.bytesPerPixel       = (tga.Bpp / 8);
		tga.imageSize           = (tga.bytesPerPixel * tga.Width * tga.Height);
		texture->imageData      = (GLubyte *)malloc(tga.imageSize);
	}

	if (texture->imageData == NULL) {
		fclose(fTGA);
		return FALSE;
	}
	GLuint pixelcount       = tga.Height * tga.Width;
	GLuint currentpixel     = 0;
	GLuint currentbyte      = 0;
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);
	do {
		GLubyte chunkheader = 0;
		if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) {
			if (fTGA != NULL) {
				fclose(fTGA);
			}
			else {
				/* nothing to do */
			}
			if (texture->imageData != NULL) {
				free(texture->imageData);
			}
			else {
				/* nothing to do */
			}
				return FALSE;
			}
			if (chunkheader < 128) {
				chunkheader++;
				short counter;
				for (counter = 0; counter < chunkheader; counter++) {
					if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) {
						if (fTGA != NULL) {
							fclose(fTGA);
						}
						else {
							/* nothing to do */
						}
						if (colorbuffer != NULL) {
							free(colorbuffer);
						}
						else {
							/* nothing to do */
						}
						if (texture->imageData != NULL) {
							free(texture->imageData);
						}
						else {
							/* nothing to do */
						}
						return FALSE;
					}
					else {
						/* nothing to do */
					}
					texture->imageData[currentbyte          ] = colorbuffer[2];
					texture->imageData[currentbyte + 1      ] = colorbuffer[1];
					texture->imageData[currentbyte + 2      ] = colorbuffer[0];

								if (tga.bytesPerPixel == 4) {
										texture->imageData[currentbyte + 3] = colorbuffer[3];
								}
								else {
									/* nothing to do */
								}
								currentbyte += tga.bytesPerPixel;
								currentpixel++;

								if (currentpixel > pixelcount) {

										if (fTGA != NULL) {
												fclose(fTGA);
										}
										else {
											/* nothing to do */
										}
										if (colorbuffer != NULL) {
												free(colorbuffer);
										}
										else {
											/* nothing to do */
										}
										if (texture->imageData != NULL) {
												free(texture->imageData);
										}
										else {
											/* nothing to do */
										}
										return FALSE;
								}
								else {
									/* nothing to do */
								}
						}
				}
				else {
						chunkheader -= 127;
						if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) {

								if (fTGA != NULL) {
										fclose(fTGA);
								}
								else {
									/* nothing to do */
								}
								if (colorbuffer != NULL) {
										free(colorbuffer);
								}
								else {
									/* nothing to do */
								}
								if (texture->imageData != NULL) {
										free(texture->imageData);
								}
								else {
									/* nothing to do */
								}
								return FALSE;
						}
			short counter;
						for ( counter = 0; counter < chunkheader; counter++) {
							texture->imageData[currentbyte          ] = colorbuffer[2];
							texture->imageData[currentbyte + 1      ] = colorbuffer[1];
							texture->imageData[currentbyte + 2      ] = colorbuffer[0];
							if (tga.bytesPerPixel == 4) {
								texture->imageData[currentbyte + 3] = colorbuffer[3];
							}
							else {
								/* nothing to do */
							}
							currentbyte += tga.bytesPerPixel;
							currentpixel++;
							if (currentpixel > pixelcount) {
								if (fTGA != NULL) {
									fclose(fTGA);
								}
								else {
									/* nothing to do */
								}
								if (colorbuffer != NULL) {
									free(colorbuffer);
								}
								else {
									/* nothing to do */
								}
								if (texture->imageData != NULL) {
									free(texture->imageData);
								}
								else {
									/* nothing to do */
								}
								return FALSE;
							}
							else {
								/* nothing to do */
							}
						}
				}
		}

		while (currentpixel < pixelcount);
		fclose(fTGA);
		return TRUE;
}

int LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)
{                                                                                                                                                       //
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		if (fTGA != NULL) {
			fclose(fTGA);
		}
		else {
			/* nothing to do */
		}
	return FALSE;
	}

	texture->width  = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp    = tga.header[4];
	tga.Width               = texture->width;
	tga.Height              = texture->height;
	tga.Bpp                 = texture->bpp;

	if ((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32))) {
		if (fTGA != NULL) {
			fclose(fTGA);
		}
		else {
			/* nothing to do */
		}
		return FALSE;
	}
	else {
		/* nothing to do */
	}

	if (texture->bpp == 24) {
		texture->type   = GL_RGB;
	}
	else
		texture->type   = GL_RGBA;
		tga.bytesPerPixel       = (tga.Bpp / 8);
		tga.imageSize           = (tga.bytesPerPixel * tga.Width * tga.Height);
		texture->imageData      = (GLubyte *)malloc(tga.imageSize);

	if (texture->imageData == NULL) {
		fclose(fTGA);
		return FALSE;
	}
	else {
		/* nothing to do */
	}
	 // Attempt to read image data
	if (fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize) {
		if (texture->imageData != NULL) {
			free(texture->imageData);
		}
		else {
			/* nothing to do */
		}
		fclose(fTGA);
		return FALSE;
	}
	else {
		/* nothing to do */
	}

	GLuint cswap;
	for (cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel) {
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
	if (fTGA == NULL) {
		return FALSE;
	}
	else {
		/* nothing to do */
	}
	if (fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0) {
		if (fTGA != NULL) {
			fclose(fTGA);
		}
		else {
			/* nothing to do */
		}
		return FALSE;
	}
	else {
		/* nothing to do */
	}
	if (memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0) {
		LoadUncompressedTGA(texture, filename, fTGA);
	}
	else if (memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0) {
		LoadCompressedTGA(texture, filename, fTGA);
	}
	else {
		fclose(fTGA);
		return FALSE;
	}
	return TRUE;
}


#endif
