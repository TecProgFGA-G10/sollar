/*
 * Solar 3D Técnicas de Programação
 * This file defines the graphics interface of the game, load the
 * textures and manage the TGA files from the folder "data".
 */
 
#include "logger.h"
#ifndef SOLAR_UTIL_H
#define SOLAR_UTIL_H
#define MAX_NUMBER_OF_SHOTS 10 /* max numbers of shoots */
#define MAX_NUMBER_OF_METEORS_THAT_WILL_APPEAR 20 /* max number of meteors that can appear */
#define FALSE 0
#define TRUE 1
#define INITIALIZE_ZERO 0 /* constant used to initialize variables with 0 number */

/* max distance to treat a shoot, on the z axis */
#define MAX_DISTANCE_FOR_SHOOT_TREATMENT -30

/* max distance the meteor moves on z axis, coming to the ship */
#define MAX_DISTANCE_OF_METEOR 20
#define INICIAL_POSITION_OF_METEOR_IN_THE_AXIS_Z -28 /* initial position of meteors */
#define DISTANCE_CAMERA_IN_THE_AXIS_Z 8 /* camera distance on z axis */
#define CAMERA_INCLINATION 4 /* camera inclination */

/* to define the field of the game, representing each corner */
#define MINIMUN_X -14
#define MAXIMUM_X 14
#define MINIMUM_Y -14
#define MAXIMUM_Y 14
#define METEOR_SPEED_VARIATION 0.05 /* meteor's speed variation */
#define SHIP_SCALE 2
#define METEOR_SCALE 1
#define SHOT_SCALE 1
#define NEW_METEOR 88 /* send the event to new meteor */
#define EXPLOSION_DECREASE_RATE 99 /* stop the timer, make the explosion smaller */
#define SCORE_VALUE 10 /* score to be added */
#define SHIP_MODEL 1 /* draw the ship model */
#define METEOR_MODEL 2 /* draw the meteor model */
#define SHOT_MODEL 3 /* draw the shoot model */
#define EXPLOSION_MODEL 4 /* draw the explosion model */
#define EXPLOSION_INCREASE_RATE 0.1 /* will loose 0.1 each time count */
#define SHIP_ACCELERATION 0.4 /* acceleration that the ship change directions */

#define GAME_OVER 666 /* game over button selected */
#define CONTINUE 777 /* continue button selected */
#define TEXTURE_SIZE 256 /* quantity of texture */

/* rgba colors and depth test */
#define GRAPHIC_MODEL  GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH
#define WIDTH 800
#define HEIGHT 600
#define WINDOW_NAME "Sollar"
#include <string.h>

typedef struct {
	float x;
	float y;
	float z;
} point; /* point with information of x,y and z axis */


typedef struct {
	point points[8]; /* points of the collision box */
} collision_box; /* collision box with an array of 8 x,y,z points*/


typedef struct {
	GLMmodel *model; /* draws the model with OpenGL */
	point position; /* position of the x,y,z point */
	collision_box collision;
	point last_position;
	int visible;
	float acceleration;
	float explosion_size;
	float rotation;
	float rotation_in_x;
	float rotation_in_y;
	float rotation_in_z;
} game_item;

typedef struct {
	GLubyte * imageData;
	GLuint  bpp; /* byte pr pixel */
	GLuint  width;
	GLuint  height;
	GLuint  texture_id;
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

GLubyte uTGAcompare[12] = {
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)2,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0};
GLubyte cTGAcompare[12] = {
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)10,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0,
	(GLubyte)0};

/*
 * Closes a file and record in log file the operation result
 * Parameters:	*file_to_close - pointer to the file to be close
 * 				*filename - pointer to the name of file that will be close
 * Return:		void
 */
void close_file(FILE *file_to_close, char *filename) {
	if (fclose(file_to_close) != 0){
		print_error_log("File %s was not close!", filename);
	}
	else {
		print_verbose_log("File %s was close.", filename);
	}
}

int Verify_header(FILE *file, char *filename){
	if (fread(tga.header, sizeof(tga.header), 1, file) == 0) {
		close_file(file, filename);
		print_error_log("Error, file is empty, not readed");

		return FALSE;
	}
	else {
		print_verbose_log("File is readed");
	}
}

int Verify_correct_bits(FILE *file, char *filename, GLuint bytes_per_pixel) {
	if ((bytes_per_pixel != 24) && (bytes_per_pixel != 32))
	{
		close_file(file, filename);
		print_error_log("Error, invalid texture");

		return FALSE;
	}
	else {
		print_verbose_log("Texture is valid");
	}
}

GLuint rgb_for_type(GLuint bytes_per_pixel, GLuint type_texture) {
	if (bytes_per_pixel == 24) {
		print_verbose_log("RGB is seted for type");
		return GL_RGB;
	}
	else {
		print_verbose_log("RGBA is seted for type");
		return GL_RGBA;
	}
}

int Evaluate_image_data_to_close(FILE *file, char *filename, GLubyte **image_data) {
	if (image_data == NULL) {
		close_file(file, filename);
		print_error_log("Error, image Data is null");

		return FALSE;
	}
	else {
		print_verbose_log("Image data filled");
	}
}

void Evaluate_image_data_to_release(GLubyte *image_data) {
	if (image_data != NULL) {
		free(image_data);
		print_verbose_log("texture->imageData is released");
	}
	else {
		print_verbose_log("texture->imageData not released");
	}
}

int Verify_chunkheader(FILE *file, char *filename, GLubyte *image_data, GLubyte *chunkheader) {
	if (fread(chunkheader, sizeof(GLubyte), 1, file) == 0) {
		close_file(file, filename);
		Evaluate_image_data_to_release(image_data);

		return FALSE;
	}
	else {
		//print_error_log("fTGA arquive is closed");
		return TRUE; /*Included after the create this function*/
	}
}

void Evaluate_color_buffer(GLubyte *colorbuffer) {
	if (colorbuffer != NULL) {
		free(colorbuffer);
		//print_verbose_log("colorbuffer is released");
	}
	else {
		//print_verbose_log("colorbuffer not released");
	}
}

int Evaluate_size_object(GLubyte *colorbuffer, GLuint bytes_per_pixel, FILE *file, char *filename, GLubyte *image_data) {
	if (fread(colorbuffer, 1, bytes_per_pixel, file) != bytes_per_pixel) {
		close_file(file, filename);
		Evaluate_color_buffer(colorbuffer);
		Evaluate_image_data_to_release(image_data);
		//print_verbose_log("Variables clean");

		return FALSE;
	}
	else {
		//print_error_log("Error, not possible clean variables");
	}
}

GLubyte Check_pixels_for_image_data(GLuint bytes_per_pixel, GLubyte *colorbuffer) {
	GLubyte image_data = 0;;
	if (bytes_per_pixel == 4) {
		image_data = colorbuffer[3];
		//print_verbose_log("imageData received bytesPerPixel");
		return image_data;
	}
	else {
		//print_error_log("Error, not possible receives bytesPerPixel");
	}
}

int Evaluate_pixel(GLuint current_pixel, GLuint pixel_count, FILE *file, char *filename, GLubyte *image_data) {
	if (current_pixel > pixel_count) {
		close_file(file, filename);

		Evaluate_image_data_to_release(image_data);
		//print_verbose_log("fTGA is closed");

		return FALSE;
	}
	else {
		//print_error_log("Error, not possible close fTGA");
	}
}

/* fix me! I am a monster! */
int LoadCompressedTGA(Texture *texture, char *filename, FILE *fTGA)
{
	/*if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
		close_file(fTGA, filename);
		print_error_log("Error, file is empty, not readed");

		return FALSE;
	}
	else {
		print_verbose_log("File is readed");
	}*/

	Verify_header(fTGA, filename);

	texture->width = (GLuint)tga.header[1] * (GLuint)TEXTURE_SIZE + (GLuint)tga.header[0];
	texture->height = (GLuint)tga.header[3] * (GLuint)TEXTURE_SIZE + (GLuint)tga.header[2];
	texture->bpp = (GLuint)tga.header[4];
	tga.Width = texture->width;
	tga.Height = texture->height;
	tga.Bpp = texture->bpp;

	/*if ((texture->bpp != 24) && (texture->bpp != 32))
	{
		close_file(fTGA, filename);
		print_error_log("Error, invalid texture");

		return FALSE;
	}
	else {
		print_verbose_log("Texture is valid");
	}*/

	Verify_correct_bits(fTGA, filename, texture->bpp);

	/*if (texture->bpp == 24) {
		texture->type = GL_RGB;
		print_verbose_log("RGB is seted for type");
	}
	else {
		texture->type = GL_RGBA;
		print_verbose_log("RGBA is seted for type");
	}*/

	texture->type = rgb_for_type(texture->bpp, texture->type);

	tga.bytesPerPixel = (tga.Bpp / 8);
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
	free(texture->imageData);
	texture->imageData = (GLubyte *)malloc(tga.imageSize * sizeof(GLubyte));

	/*if (texture->imageData == NULL) {
		close_file(fTGA, filename);
		print_error_log("Error, image Data is null");

		return FALSE;
	}
	else {
		print_verbose_log("Image data filled");
	}*/

	Evaluate_image_data_to_close(fTGA, filename, &texture->imageData);

	GLuint pixelcount = tga.Height * tga.Width;
	GLuint currentpixel = 0;
	GLuint currentbyte = 0;
	GLubyte *colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);

	do {
		GLubyte chunkheader = 0;
		// if (fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0) {
		// 	close_file(fTGA, filename);

		// 	if (texture->imageData != NULL) {
		// 		free(texture->imageData);
		// 		print_verbose_log("texture->imageData is released");
		// 	}
		// 	else {
		// 		print_verbose_log("texture->imageData not released");
		// 	}
		// 	Evaluate_image_data_to_release(texture->imageData);

		// 	return FALSE;
		// }
		// else {
		// 	print_error_log("fTGA arquive is closed");
		// }

		int result_verify_chunkheader =  Verify_chunkheader(fTGA, filename, texture->imageData, &chunkheader);
		/*if (result_verify_chunkheader != TRUE){
			return FALSE;
		}*/

		if (chunkheader < 128) {
			chunkheader++;

			for (short counter = 0; counter < chunkheader; counter++) {
				/*if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) {
					close_file(fTGA, filename);

					if (colorbuffer != NULL) {
						free(colorbuffer);
						//print_verbose_log("colorbuffer is released");
					}
					else {
						//print_verbose_log("colorbuffer not released");
					}
					Evaluate_color_buffer(colorbuffer);

					/*if (texture->imageData != NULL) {
						free(texture->imageData);
						print_verbose_log("texture->imageData is released");
					}
					else {
						print_verbose_log("texture->imageData not released");
					}
					Evaluate_image_data_to_release(texture->imageData);
					//print_verbose_log("Variables clean");

					return FALSE;
				}
				else {
					//print_error_log("Error, not possible clean variables");
				}*/
				
				//avalia se o arquivo pode ser fechado, avalia se há cor e libera memória, avalia a imagem da textura e libera memória
				Evaluate_size_object(colorbuffer, tga.bytesPerPixel, fTGA, filename, texture->imageData);

				texture->imageData[currentbyte] = colorbuffer[2];
				texture->imageData[currentbyte + 1] = colorbuffer[1];
				texture->imageData[currentbyte + 2] = colorbuffer[0];

				/*if (tga.bytesPerPixel == 4) {
					texture->imageData[currentbyte + 3] = colorbuffer[3];
					//print_verbose_log("imageData received bytesPerPixel");
				}
				else {
					//print_error_log("Error, not possible receives bytesPerPixel");
				}*/
				texture->imageData[currentbyte + 3] = Check_pixels_for_image_data(tga.bytesPerPixel, colorbuffer);

				currentbyte += tga.bytesPerPixel;
				currentpixel++;

				/*if (currentpixel > pixelcount) {
					close_file(fTGA, filename);

					if (texture->imageData != NULL) {
						free(texture->imageData);
						//print_verbose_log("texture->imageData is released");
					}
					else {
						//print_verbose_log("texture->imageData not released");
					}
					Evaluate_image_data_to_release(texture->imageData);
					//print_verbose_log("fTGA is closed");

					return FALSE;
				}
				else {
					//print_error_log("Error, not possible close fTGA");
				}*/
				
				Evaluate_pixel(currentpixel, pixelcount, fTGA, filename, texture->imageData);
			}

		}
		else {
			chunkheader -= 127;
			/*if (fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) {
				close_file(fTGA, filename);

				/*if (colorbuffer != NULL) {
					free(colorbuffer);
					//print_verbose_log("colorbuffer is released");
				}
				else {
					//print_verbose_log("colorbuffer not released");
				}
				Evaluate_color_buffer(colorbuffer);

				/*if (texture->imageData != NULL) {
					free(texture->imageData);
					//print_verbose_log("texture->imageData is released");
				}
				else {
					//print_verbose_log("texture->imageData not released");
				}
				Evaluate_image_data_to_release(texture->imageData);
				//print_verbose_log("fTGA is closed");

				return FALSE;
			}
			else {
				//print_error_log("Error, not possible close fTGA");
			}*/
			Evaluate_size_object(colorbuffer, tga.bytesPerPixel, fTGA, filename, texture->imageData);

			for (short counter = 0; counter < chunkheader; counter++) {
				texture->imageData[currentbyte] = colorbuffer[2];
				texture->imageData[currentbyte + 1] = colorbuffer[1];
				texture->imageData[currentbyte + 2] = colorbuffer[0];

				if (tga.bytesPerPixel == 4) {
					texture->imageData[currentbyte + 3] = colorbuffer[3];
					//print_verbose_log("imageData received bytesPerPixel");
				}
				else {
					//print_error_log("Error, not possible receives bytesPerPixel");
				}

				currentbyte += tga.bytesPerPixel;
				currentpixel++;
				if (currentpixel > pixelcount) {
					close_file(fTGA, filename);

					/*if (colorbuffer != NULL) {
						free(colorbuffer);
						//print_verbose_log("colorbuffer is released");
					}
					else {
						//print_verbose_log("colorbuffer not released");
					}*/
					Evaluate_color_buffer(colorbuffer);

					/*if (texture->imageData != NULL) {
						free(texture->imageData);
						//print_verbose_log("texture->imageData is released");
					}
					else {
						//print_verbose_log("texture->imageData not released");
					}*/
					Evaluate_image_data_to_release(texture->imageData);
					//print_verbose_log("fTGA is closed");

					return FALSE;
				}
				else {
					//print_error_log("Error, not possible close fTGA");
				}
			}
		}
	} 
	while (currentpixel < pixelcount);

	close_file(fTGA, filename);

	return TRUE;
}

/* loads uncompressed TGA */
int LoadUncompressedTGA(Texture *texture, char *filename, FILE *fTGA)
{
	if (fread(tga.header, sizeof(tga.header), 1, fTGA) == 0) {
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

	texture->width = tga.header[1] * TEXTURE_SIZE + tga.header[0];
	texture->height = tga.header[3] * TEXTURE_SIZE + tga.header[2];
	texture->bpp = tga.header[4];
	tga.Width = texture->width;
	tga.Height = texture->height;
	tga.Bpp = texture->bpp;

	if ((texture->width <= 0) || (texture->height <= 0) ||
		((texture->bpp != 24) && (texture->bpp !=32))) {
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
		texture->type = GL_RGB;
	}
	else {
		texture->type = GL_RGBA;
	}

	tga.bytesPerPixel = (tga.Bpp / 8);
	tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);
	texture->imageData = (GLubyte *)malloc(tga.imageSize);

	if (texture->imageData == NULL) {
		fclose(fTGA);
		return FALSE;
	}
	else {
		/* nothing to do */
	}
	/* attempt to read image data */
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

	for (GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel) {
		texture->imageData[cswap] ^= texture->imageData[cswap+2];
		texture->imageData[cswap+2] ^=	texture->imageData[cswap];
		texture->imageData[cswap] ^= texture->imageData[cswap+2];
	}
	fclose(fTGA);
	return TRUE;
}

/* loads TGA */
int LoadTGA(Texture *texture, char *filename)
{
	FILE *fTGA;
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

