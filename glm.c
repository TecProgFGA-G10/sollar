/*
 * glm.c
 * Nate Robins, 1997, 2000
 * nate@pobox.com, http://www.pobox.com/~nate
 *
 * Wavefront OBJ model file format reader/writer/manipulator.
 *
 * Includes routines for generating smooth normals with
 * preservation of edges, welding redundant vertex & texture
 * coordinate generation (spheremap and planar projections) + more.
 */
#define _GNU_SOURCE 1

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "glm.h"

#define T(x) (model->triangles[(x)])

/* _GLMnode: general purpose node */
typedef struct _GLMnode {
	GLuint         index;
	GLboolean      averaged;
	struct _GLMnode* next;
} GLMnode;

/* glmMax: returns the maximum of two floats */
static GLfloat
glmMax(GLfloat a, GLfloat b)
{
	GLfloat result_maximum = 0;
	if (b > a) {
		//return b;
		result_maximum = b;
	}
	else {
		//return a;
		result_maximum = a;
	}
	return result_maximum;
}

/* glmAbs: returns the absolute value of a float */
static GLfloat
glmAbs(GLfloat f)
{
	GLfloat result_absolute_value = 0;
	if (f < 0) {
		//return -f;
		result_absolute_value = -f;
	}
	else {
		//return f;
		result_absolute_value = f;
	}
	return result_absolute_value;
}

/*
 * glmDot: compute the dot product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 */
static GLfloat
glmDot(GLfloat *u, GLfloat *v)
{
	assert(u); assert(v);

	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

/*
 * glmCross: compute the cross product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 * n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
 */
static GLvoid
glmCross(GLfloat* u, GLfloat* v, GLfloat* n)
{
	assert(u); assert(v); assert(n);

	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
}

/*
 * glmNormalize: normalize a vector
 *
 * v - array of 3 GLfloats (GLfloat v[3]) to be normalized
 */
static GLvoid
glmNormalize(GLfloat *v)
{
	GLfloat l;

	assert(v);

	l = (GLfloat)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= l;
	v[1] /= l;
	v[2] /= l;
}

/*
 * glmEqual: compares two vectors and returns GL_TRUE if they are
 * equal (within a certain threshold) or GL_FALSE if not. An epsilon
 * that works fairly well is 0.000001.
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 */
static GLboolean
glmEqual(GLfloat *u, GLfloat *v, GLfloat epsilon)
{
	GLboolean result_values_glmEqual;
	if (glmAbs(u[0] - v[0]) < epsilon &&
		glmAbs(u[1] - v[1]) < epsilon &&
		glmAbs(u[2] - v[2]) < epsilon)
	{
		//return GL_TRUE;
		result_values_glmEqual = GL_TRUE;
	}
	else {
		//return GL_FALSE;
		result_values_glmEqual = GL_FALSE;
	}
	return result_values_glmEqual;
}

/*
 * glmWeldVectors: eliminate (weld) vectors that are within an
 * epsilon of each other.
 *
 * vectors     - array of GLfloat[3]'s to be welded
 * number_of_vectors  - number of GLfloat[3]'s in vectors
 * epsilon     - maximum difference between vectors
 *
 */
GLfloat*
glmWeldVectors(GLfloat *vectors, GLuint *number_of_vectors, GLfloat epsilon)
{
	GLfloat *copies;
	GLuint copied;
	GLuint duplicated = 0;

	copies = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (*number_of_vectors + 1));
	memcpy(copies, vectors, (sizeof(GLfloat) * 3 * (*number_of_vectors + 1)));

	copied = 1;
	for (GLuint i = 1; i <= *number_of_vectors; i++) {
		GLuint j = 1;
		for (j = 1; j <= copied; j++) {
			if (glmEqual(&vectors[3 * i], &copies[3 * j], epsilon)) {
				duplicated = 1;
				break;
			}
			else {
				/* nothing to do */
			}
		}

		if (duplicated) {
			/*
			 * Set the first component of this vector to point at the correct index
			 * into the new copies array.
			 */
			vectors[3 * i + 0] = (GLfloat)j;
		}
		else{
			/* must not be any duplicates -- add to the copies array */
			copies[3 * copied + 0] = vectors[3 * i + 0];
			copies[3 * copied + 1] = vectors[3 * i + 1];
			copies[3 * copied + 2] = vectors[3 * i + 2];
			j = copied; /* pass this along for below */
			copied++;

			vectors[3 * i + 0] = (GLfloat)j;
		}
	}

	*number_of_vectors = copied-1;
	return copies;
}

/* glmFindGroup: Find a group in the model */
GLMgroup*
glmFindGroup(GLMmodel *model, char *name)
{
	GLMgroup *group;

	assert(model);

	group = model->groups;
	while (group) {
		if (!strcmp(name, group->name)) {
			break;
		}
		else {
			group = group->next;
		}
	}

	return group;
}

/* glmAddGroup: Add a group to the model */
GLMgroup*
glmAddGroup(GLMmodel *model, char *name)
{
	GLMgroup *group;

	group = glmFindGroup(model, name);
	if (!group) {
		group = (GLMgroup*)malloc(sizeof(GLMgroup));
		group->name = strdup(name);
		group->material = 0;
		group->number_of_triangles = 0;
		group->triangles = NULL;
		group->next = model->groups;
		model->groups = group;
		model->number_of_groups++;
	}
	else {
		/* nothing to do */
	}

	return group;
}

/* glmFindGroup: Find a material in the model */
GLuint
glmFindMaterial(GLMmodel *model, char *name)
{
	GLuint model_found = 0;
	/*
	 * XXX doing a linear search on a string key'd list is pretty lame,
	 * but it works and is fast enough for now.
	 */
	for (GLuint i = 0; i < model->number_of_materials; i++) {
		if (!strcmp(model->materials[i].name, name)) {
			model_found = i;
		}
		else {
			/* nothing to do */
		}
	}

	if (!model_found) {
	printf("glmFindMaterial():  can't find material \"%s\".\n", name);
	}
	else {
		/* nothing to do */
	}

	return model_found;
}

/*
 * glmDirName: return the directory given a path
 *
 * path - filesystem path
 *
 * NOTE: the return value should be free'd.
 */
static char*
glmDirName(char *path)
{
	char *dir;
	char *s;

	dir = strdup(path);

	s = strrchr(dir, '/');
	if (s) {
		s[1] = '\0';
	}
	else {
		dir[0] = '\0';
	}

	return dir;
}

/*
 * glmReadMTL: read a wavefront material library file
 *
 * model - properly initialized GLMmodel structure
 * name  - name of the material library
 */
static GLvoid
glmReadMTL(GLMmodel *model, char *name)
{
	FILE *file;
	char *dir;
	char *filename;
	char buffer[128];
	GLuint number_of_materials;

	dir = glmDirName(model->path_name);
	filename = (char *)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
	strcpy(filename, dir);
	strcat(filename, name);
	free(dir);

	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr,
		        "glmReadMTL() failed: can't open material file \"%s\".\n",
		        filename);
		exit(1);
	}
	else {
		/* nothing to do */
	}
	free(filename);

	/* count the number of materials in the file */
	number_of_materials = 1;
	while (fscanf(file, "%s", buffer) != EOF) {
		switch (buffer[0]) {
			case '#': /* comment */
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
			case 'n': /* newmtl */
				fgets(buffer, sizeof(buffer), file);
				number_of_materials++;
				sscanf(buffer, "%s %s", buffer, buffer);
				break;
			default:
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
		}
	}

	rewind(file);

	model->materials = (GLMmaterial*)malloc(sizeof(GLMmaterial) * number_of_materials);
	model->number_of_materials = number_of_materials;

	/* set the default material */
	for (GLuint i = 0; i < number_of_materials; i++) {
		model->materials[i].name = NULL;
		model->materials[i].shininess = 65.0;
		model->materials[i].diffuse[0] = 0.8;
		model->materials[i].diffuse[1] = 0.8;
		model->materials[i].diffuse[2] = 0.8;
		model->materials[i].diffuse[3] = 1.0;
		model->materials[i].ambient[0] = 0.2;
		model->materials[i].ambient[1] = 0.2;
		model->materials[i].ambient[2] = 0.2;
		model->materials[i].ambient[3] = 1.0;
		model->materials[i].specular[0] = 0.0;
		model->materials[i].specular[1] = 0.0;
		model->materials[i].specular[2] = 0.0;
		model->materials[i].specular[3] = 1.0;
	}
	model->materials[0].name = strdup("default");

	/* now, read in the data */
	number_of_materials = 0;
	while (fscanf(file, "%s", buffer) != EOF) {
		switch (buffer[0]) {
			case '#': /* comment */
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
			case 'n': /* newmtl */
				fgets(buffer, sizeof(buffer), file);
				sscanf(buffer, "%s %s", buffer, buffer);
				number_of_materials++;
				model->materials[number_of_materials].name = strdup(buffer);
				break;
			case 'N':
				fscanf(file, "%f", &model->materials[number_of_materials].shininess);
				/* wavefront shininess is from [0, 1000], so scale for OpenGL */
				model->materials[number_of_materials].shininess /= 1000.0;
				model->materials[number_of_materials].shininess *= 128.0;
				break;
			case 'K':
				switch (buffer[1]) {
					case 'd':
						fscanf(file, "%f %f %f",
							&model->materials[number_of_materials].diffuse[0],
							&model->materials[number_of_materials].diffuse[1],
							&model->materials[number_of_materials].diffuse[2]);
						break;
					case 's':
						fscanf(file, "%f %f %f",
							&model->materials[number_of_materials].specular[0],
							&model->materials[number_of_materials].specular[1],
							&model->materials[number_of_materials].specular[2]);
						break;
					case 'a':
						fscanf(file, "%f %f %f",
							&model->materials[number_of_materials].ambient[0],
							&model->materials[number_of_materials].ambient[1],
							&model->materials[number_of_materials].ambient[2]);
						break;
					default:
						/* eat up rest of line */
						fgets(buffer, sizeof(buffer), file);
						break;
				}
				break;
				default:
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
					break;
		}
	}
}

/*
 * glmWriteMTL: write a wavefront material library file
 *
 * model   - properly initialized GLMmodel structure
 * model_path  - path_name of the model being written
 * mtl_lib_name - name of the material library to be written
 */
static GLvoid
glmWriteMTL(GLMmodel *model, char *model_path, char *mtl_lib_name)
{
	FILE *file;
	char *dir;
	char *filename;
	GLMmaterial *material;

	dir = glmDirName(model_path);
	filename = (char *)malloc(sizeof(char) * (strlen(dir) + strlen(mtl_lib_name)));
	strcpy(filename, dir);
	strcat(filename, mtl_lib_name);
	free(dir);

	/* open the file */
	file = fopen(filename, "w");
	if (!file) {
		fprintf(stderr, "glmWriteMTL() failed: can't open file \"%s\".\n",
			filename);
		exit(1);
	}
	else {
		/* nothing to do */
	}
	free(filename);

	/* spit out a header */
	fprintf(file, "#  \n");
	fprintf(file, "#  Wavefront MTL generated by GLM library\n");
	fprintf(file, "#  \n");
	fprintf(file, "#  GLM library\n");
	fprintf(file, "#  Nate Robins\n");
	fprintf(file, "#  ndr@pobox.com\n");
	fprintf(file, "#  http://www.pobox.com/~ndr\n");
	fprintf(file, "#  \n\n");

	for (GLuint i = 0; i < model->number_of_materials; i++) {
		material = &model->materials[i];
		fprintf(file, "newmtl %s\n", material->name);
		fprintf(file, "Ka %f %f %f\n",
			material->ambient[0], material->ambient[1], material->ambient[2]);
		fprintf(file, "Kd %f %f %f\n",
			material->diffuse[0], material->diffuse[1], material->diffuse[2]);
		fprintf(file, "Ks %f %f %f\n",
			material->specular[0],material->specular[1],material->specular[2]);
		fprintf(file, "Ns %f\n", material->shininess / 128.0 * 1000.0);
		fprintf(file, "\n");
	}
}

/*
 * glmFirstPass: first pass at a Wavefront OBJ file that gets all the
 * statistics of the model (such as #vertex, #normals, etc)
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor
 */
static GLvoid
glmFirstPass(GLMmodel *model, FILE *file)
{
	GLuint vertices_numbers;  /* number of vertex in model  */
	GLuint number_of_normals_in_model;   /* number of normals in model   */
	GLuint texture_cordinates_number; /* number of texture_cordinates in model */
	GLuint number_of_triangles; /* number of triangles in model */
	GLMgroup *group;     /* current group                */
	unsigned v, n, t;
	char buffer[128];

	/* make a default group */
	group = glmAddGroup(model, "default");

	vertices_numbers = number_of_normals_in_model = texture_cordinates_number = number_of_triangles = 0;
	while (fscanf(file, "%s", buffer) != EOF) {
		switch (buffer[0]) {
			case '#': /* comment */
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
			case 'v': /* v, vn, vt */
				switch(buffer[1]) {
				case '\0': /* vertex */
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
					vertices_numbers++;
					break;
				case 'n': /* normal */
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
					number_of_normals_in_model++;
					break;
				case 't': /* texcoord */
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
					texture_cordinates_number++;
					break;
				default:
					printf("glmFirstPass(): Unknown token \"%s\".\n", buffer);
					exit(1);
					break;
				}
				break;
				case 'm':
					fgets(buffer, sizeof(buffer), file);
					sscanf(buffer, "%s %s", buffer, buffer);
					model->mtl_lib_name = strdup(buffer);
					glmReadMTL(model, buffer);
					break;
				case 'u':
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
					break;
				case 'g': /* group */
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
				#if SINGLE_STRING_GROUP_NAMES
				sscanf(buffer, "%s", buffer);
				#else
				buffer[strlen(buffer)-1] = '\0'; /* nuke '\n' */
				#endif
				group = glmAddGroup(model, buffer);
				break;
			case 'f': /* face */
				v = n = t = 0;
				fscanf(file, "%s", buffer);
				/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
				if (strstr(buffer, "//")) {
					/* v//n */
					sscanf(buffer, "%u//%u", &v, &n);
					fscanf(file, "%u//%u", &v, &n);
					fscanf(file, "%u//%u", &v, &n);
					number_of_triangles++;
					group->number_of_triangles++;
					while (fscanf(file, "%u//%u", &v, &n) > 0) {
						number_of_triangles++;
						group->number_of_triangles++;
					}
				}
				else if (sscanf(buffer, "%u/%u/%u", &v, &t, &n) == 3) {
					/* v/t/n */
					fscanf(file, "%u/%u/%u", &v, &t, &n);
					fscanf(file, "%u/%u/%u", &v, &t, &n);
					number_of_triangles++;
					group->number_of_triangles++;
					while (fscanf(file, "%u/%u/%u", &v, &t, &n) > 0) {
						number_of_triangles++;
						group->number_of_triangles++;
					}
				}
				else if (sscanf(buffer, "%u/%u", &v, &t) == 2) {
					/* v/t */
					fscanf(file, "%u/%u", &v, &t);
					fscanf(file, "%u/%u", &v, &t);
					number_of_triangles++;
					group->number_of_triangles++;
					while (fscanf(file, "%u/%u", &v, &t) > 0) {
						number_of_triangles++;
						group->number_of_triangles++;
					}
				}
				else {
					/* v */
					fscanf(file, "%u", &v);
					fscanf(file, "%u", &v);
					number_of_triangles++;
					group->number_of_triangles++;
					while (fscanf(file, "%u", &v) > 0) {
						number_of_triangles++;
						group->number_of_triangles++;
					}
				}
				break;

			default:
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
		}
	}

	/* set the stats in the model structure */
	model->vertices_numbers = vertices_numbers;
	model->number_of_normals_in_model = number_of_normals_in_model;
	model->texture_cordinates_number = texture_cordinates_number;
	model->number_of_triangles = number_of_triangles;

	/* allocate memory for the triangles in each group */
	group = model->groups;
	while (group) {
	  group->triangles = (GLuint *)malloc(sizeof(GLuint) * group->number_of_triangles);
	  group->number_of_triangles = 0;
	  group = group->next;
	}
}

/*
 * glmSecondPass: second pass at a Wavefront OBJ file that gets all
 * the data.
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor
 */
static GLvoid
glmSecondPass(GLMmodel *model, FILE *file)
{
	GLuint vertices_numbers;  /* number of vertex in model  */
	GLuint number_of_normals_in_model;   /* number of normals in model   */
	GLuint texture_cordinates_number; /* number of texture_cordinates in model */
	GLuint number_of_triangles; /* number of triangles in model */
	GLfloat *vertex;   /* array of vertex            */
	GLfloat *normals;    /* array of normals             */
	GLfloat *texture_cordinates;  /* array of texture coordinates */
	GLMgroup *group;     /* current group pointer        */
	GLuint material;     /* current material             */
	GLuint v, n, t;
	char buffer[128];

	/* set the pointer shortcuts */
	vertex = model->vertex;
	normals = model->normals;
	texture_cordinates = model->texture_cordinates;
	group = model->groups;

	/*
	 * On the second pass through the file,
	 * read all the data into the allocated arrays
	 */
	vertices_numbers = number_of_normals_in_model = texture_cordinates_number = 1;
	number_of_triangles = 0;
	material = 0;
	while (fscanf(file, "%s", buffer) != EOF) {
		switch(buffer[0]) {
			case '#': /* comment */
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
			case 'v': /* v, vn, vt */
				switch(buffer[1]) {
					case '\0': /* vertex */
						fscanf(file, "%f %f %f",
						       &vertex[3 * vertices_numbers + 0],
						       &vertex[3 * vertices_numbers + 1],
						       &vertex[3 * vertices_numbers + 2]);
						vertices_numbers++;
						break;
					case 'n': /* normal */
						fscanf(file, "%f %f %f",
						       &normals[3 * number_of_normals_in_model + 0],
						       &normals[3 * number_of_normals_in_model + 1],
						       &normals[3 * number_of_normals_in_model + 2]);
						number_of_normals_in_model++;
						break;
					case 't': /* texcoord */
						fscanf(file, "%f %f",
						       &texture_cordinates[2 * texture_cordinates_number + 0],
							   &texture_cordinates[2 * texture_cordinates_number + 1]);
						texture_cordinates_number++;
						break;
				}
				break;
				case 'u':
					fgets(buffer, sizeof(buffer), file);
					sscanf(buffer, "%s %s", buffer, buffer);
					group->material = material = glmFindMaterial(model, buffer);
					break;
				case 'g': /* group */
					/* eat up rest of line */
					fgets(buffer, sizeof(buffer), file);
				#if SINGLE_STRING_GROUP_NAMES
				sscanf(buffer, "%s", buffer);
				#else
				buffer[strlen(buffer)-1] = '\0'; /* nuke '\n' */
				#endif
				group = glmFindGroup(model, buffer);
				group->material = material;
				break;
			case 'f': /* face */
				v = n = t = 0;
				fscanf(file, "%s", buffer);
				/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
				if (strstr(buffer, "//")) {
					/* v//n */
					sscanf(buffer, "%u//%u", &v, &n);
					T(number_of_triangles).vertex_indexes[0] = v;
					T(number_of_triangles).normal_indexes[0] = n;
					fscanf(file, "%u//%u", &v, &n);
					T(number_of_triangles).vertex_indexes[1] = v;
					T(number_of_triangles).normal_indexes[1] = n;
					fscanf(file, "%u//%u", &v, &n);
					T(number_of_triangles).vertex_indexes[2] = v;
					T(number_of_triangles).normal_indexes[2] = n;
					group->triangles[group->number_of_triangles++] = number_of_triangles;
					number_of_triangles++;
					while (fscanf(file, "%u//%u", &v, &n) > 0) {
						T(number_of_triangles).vertex_indexes[0] = T(number_of_triangles-1).vertex_indexes[0];
						T(number_of_triangles).normal_indexes[0] = T(number_of_triangles-1).normal_indexes[0];
						T(number_of_triangles).vertex_indexes[1] = T(number_of_triangles-1).vertex_indexes[2];
						T(number_of_triangles).normal_indexes[1] = T(number_of_triangles-1).normal_indexes[2];
						T(number_of_triangles).vertex_indexes[2] = v;
						T(number_of_triangles).normal_indexes[2] = n;
						group->triangles[group->number_of_triangles++] = number_of_triangles;
						number_of_triangles++;
					}
				}
				else if (sscanf(buffer, "%u/%u/%u", &v, &t, &n) == 3) {
					/* v/t/n */
					T(number_of_triangles).vertex_indexes[0] = v;
					T(number_of_triangles).triangle_indexes[0] = t;
					T(number_of_triangles).normal_indexes[0] = n;
					fscanf(file, "%u/%u/%u", &v, &t, &n);
					T(number_of_triangles).vertex_indexes[1] = v;
					T(number_of_triangles).triangle_indexes[1] = t;
					T(number_of_triangles).normal_indexes[1] = n;
					fscanf(file, "%u/%u/%u", &v, &t, &n);
					T(number_of_triangles).vertex_indexes[2] = v;
					T(number_of_triangles).triangle_indexes[2] = t;
					T(number_of_triangles).normal_indexes[2] = n;
					group->triangles[group->number_of_triangles++] = number_of_triangles;
					number_of_triangles++;
					while (fscanf(file, "%u/%u/%u", &v, &t, &n) > 0) {
						T(number_of_triangles).vertex_indexes[0] = T(number_of_triangles-1).vertex_indexes[0];
						T(number_of_triangles).triangle_indexes[0] = T(number_of_triangles-1).triangle_indexes[0];
						T(number_of_triangles).normal_indexes[0] = T(number_of_triangles-1).normal_indexes[0];
						T(number_of_triangles).vertex_indexes[1] = T(number_of_triangles-1).vertex_indexes[2];
						T(number_of_triangles).triangle_indexes[1] = T(number_of_triangles-1).triangle_indexes[2];
						T(number_of_triangles).normal_indexes[1] = T(number_of_triangles-1).normal_indexes[2];
						T(number_of_triangles).vertex_indexes[2] = v;
						T(number_of_triangles).triangle_indexes[2] = t;
						T(number_of_triangles).normal_indexes[2] = n;
						group->triangles[group->number_of_triangles++] = number_of_triangles;
						number_of_triangles++;
					}
				}
				else if (sscanf(buffer, "%u/%u", &v, &t) == 2) {
					/* v/t */
					T(number_of_triangles).vertex_indexes[0] = v;
					T(number_of_triangles).triangle_indexes[0] = t;
					fscanf(file, "%u/%u", &v, &t);
					T(number_of_triangles).vertex_indexes[1] = v;
					T(number_of_triangles).triangle_indexes[1] = t;
					fscanf(file, "%u/%u", &v, &t);
					T(number_of_triangles).vertex_indexes[2] = v;
					T(number_of_triangles).triangle_indexes[2] = t;
					group->triangles[group->number_of_triangles++] = number_of_triangles;
					number_of_triangles++;
					while (fscanf(file, "%u/%u", &v, &t) > 0) {
						T(number_of_triangles).vertex_indexes[0] = T(number_of_triangles-1).vertex_indexes[0];
						T(number_of_triangles).triangle_indexes[0] = T(number_of_triangles-1).triangle_indexes[0];
						T(number_of_triangles).vertex_indexes[1] = T(number_of_triangles-1).vertex_indexes[2];
						T(number_of_triangles).triangle_indexes[1] = T(number_of_triangles-1).triangle_indexes[2];
						T(number_of_triangles).vertex_indexes[2] = v;
						T(number_of_triangles).triangle_indexes[2] = t;
						group->triangles[group->number_of_triangles++] = number_of_triangles;
						number_of_triangles++;
					}
				}
				else {
					/* v */
					sscanf(buffer, "%u", &v);
					T(number_of_triangles).vertex_indexes[0] = v;
					fscanf(file, "%u", &v);
					T(number_of_triangles).vertex_indexes[1] = v;
					fscanf(file, "%u", &v);
					T(number_of_triangles).vertex_indexes[2] = v;
					group->triangles[group->number_of_triangles++] = number_of_triangles;
					number_of_triangles++;
					while (fscanf(file, "%u", &v) > 0) {
						T(number_of_triangles).vertex_indexes[0] = T(number_of_triangles-1).vertex_indexes[0];
						T(number_of_triangles).vertex_indexes[1] = T(number_of_triangles-1).vertex_indexes[2];
						T(number_of_triangles).vertex_indexes[2] = v;
						group->triangles[group->number_of_triangles++] = number_of_triangles;
						number_of_triangles++;
					}
				}
				break;

			default:
				/* eat up rest of line */
				fgets(buffer, sizeof(buffer), file);
				break;
		}
	}
	#if 0
	/* announce the memory requirements */
	printf(" Memory: %d bytes\n",
	       vertices_numbers  * 3*sizeof(GLfloat) +
           number_of_normals_in_model   * 3*sizeof(GLfloat) * (number_of_normals_in_model ? 1 : 0) +
           texture_cordinates_number * 3*sizeof(GLfloat) * (texture_cordinates_number ? 1 : 0) +
           number_of_triangles * sizeof(GLMtriangle));
	#endif
}

/* public functions */

/*
 * glmUnitize: "unitize" a model by translating it to the origin and
 * scaling it to fit in a unit cube around the origin.   Returns the
 * scalefactor used.
 *
 * model - properly initialized GLMmodel structure
 */
GLfloat
glmUnitize(GLMmodel *model)
{
	GLfloat maximum_x, minimum_x, maximum_y, minimum_y, maximum_z, minimum_z;
	GLfloat cx, cy, cz, w, h, d;
	GLfloat scale;

	assert(model);
	assert(model->vertex);

	/* get the max/mins */
	maximum_x = minimum_x = model->vertex[3 + 0];
	maximum_y = minimum_y = model->vertex[3 + 1];
	maximum_z = minimum_z = model->vertex[3 + 2];
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		if (maximum_x < model->vertex[3 * i + 0]) {
			maximum_x = model->vertex[3 * i + 0];
		}
		else {
			/* nothing to do */
		}

		if (minimum_x > model->vertex[3 * i + 0]) {
			minimum_x = model->vertex[3 * i + 0];
		}
		else {
			/* nothing to do */
		}

		if (maximum_y < model->vertex[3 * i + 1]) {
			maximum_y = model->vertex[3 * i + 1];
		}
		else {
			/* nothing to do */
		}

		if (minimum_y > model->vertex[3 * i + 1]) {
			minimum_y = model->vertex[3 * i + 1];
		}
		else {
			/* nothing to do */
		}

		if (maximum_z < model->vertex[3 * i + 2]) {
			maximum_z = model->vertex[3 * i + 2];
		}
		else {
			/* nothing to do */
		}

		if (minimum_z > model->vertex[3 * i + 2]) {
			minimum_z = model->vertex[3 * i + 2];
		}
		else {
			/* nothing to do */
		}
	}

	/* calculate model width, height, and depth */
	w = glmAbs(maximum_x) + glmAbs(minimum_x);
	h = glmAbs(maximum_y) + glmAbs(minimum_y);
	d = glmAbs(maximum_z) + glmAbs(minimum_z);

	/* calculate center of the model */
	cx = (maximum_x + minimum_x) / 2.0;
	cy = (maximum_y + minimum_y) / 2.0;
	cz = (maximum_z + minimum_z) / 2.0;

	/* calculate unitizing scale factor */
	scale = 2.0 / glmMax(glmMax(w, h), d);

	/* translate around center then scale */
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		model->vertex[3 * i + 0] -= cx;
		model->vertex[3 * i + 1] -= cy;
		model->vertex[3 * i + 2] -= cz;
		model->vertex[3 * i + 0] *= scale;
		model->vertex[3 * i + 1] *= scale;
		model->vertex[3 * i + 2] *= scale;
	}

	return scale;
}

/*
 * glmDimensions: Calculates the dimensions (width, height, depth) of
 * a model.
 *
 * model   - initialized GLMmodel structure
 * dimensions - array of 3 GLfloats (GLfloat dimensions[3])
 */
GLvoid
glmDimensions(GLMmodel *model, GLfloat *dimensions)
{
	GLfloat maximum_x, minimum_x, maximum_y, minimum_y, maximum_z, minimum_z;

	assert(model);
	assert(model->vertex);
	assert(dimensions);

	/* get the max/mins */
	maximum_x = minimum_x = model->vertex[3 + 0];
	maximum_y = minimum_y = model->vertex[3 + 1];
	maximum_z = minimum_z = model->vertex[3 + 2];
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		if (maximum_x < model->vertex[3 * i + 0]) {
			maximum_x = model->vertex[3 * i + 0];
		}
		else {
			/* nothing to do */
		}

		if (minimum_x > model->vertex[3 * i + 0]) {
			minimum_x = model->vertex[3 * i + 0];
		}
		else {
			/* nothing to do */
		}

		if (maximum_y < model->vertex[3 * i + 1]) {
			maximum_y = model->vertex[3 * i + 1];
		}
		else {
			/* nothing to do */
		}

		if (minimum_y > model->vertex[3 * i + 1]) {
			minimum_y = model->vertex[3 * i + 1];
		}
		else {
			/* nothing to do */
		}

		if (maximum_z < model->vertex[3 * i + 2]) {
			maximum_z = model->vertex[3 * i + 2];
		}
		else {
			/* nothing to do */
		}

		if (minimum_z > model->vertex[3 * i + 2]) {
			minimum_z = model->vertex[3 * i + 2];
		}
		else {
			/* nothing to do */
		}
	}

	/* calculate model width, height, and depth */
	dimensions[0] = glmAbs(maximum_x) + glmAbs(minimum_x);
	dimensions[1] = glmAbs(maximum_y) + glmAbs(minimum_y);
	dimensions[2] = glmAbs(maximum_z) + glmAbs(minimum_z);
}

/*
 * glmScale: Scales a model by a given amount.
 *
 * model - properly initialized GLMmodel structure
 * scale - scalefactor (0.5 = half as large, 2.0 = twice as large)
 */
GLvoid
glmScale(GLMmodel *model, GLfloat scale)
{
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		model->vertex[3 * i + 0] *= scale;
		model->vertex[3 * i + 1] *= scale;
		model->vertex[3 * i + 2] *= scale;
	}
}

/*
 * glmReverseWinding: Reverse the polygon winding for all polygons in
 * this model.   Default winding is counter-clockwise.  Also changes
 * the direction of the normals.
 *
 * model - properly initialized GLMmodel structure
 */
GLvoid
glmReverseWinding(GLMmodel *model)
{
	GLuint swap;

	assert(model);

	for (GLuint i = 0; i < model->number_of_triangles; i++) {
		swap = T(i).vertex_indexes[0];
		T(i).vertex_indexes[0] = T(i).vertex_indexes[2];
		T(i).vertex_indexes[2] = swap;

		if (model->number_of_normals_in_model) {
			swap = T(i).normal_indexes[0];
			T(i).normal_indexes[0] = T(i).normal_indexes[2];
			T(i).normal_indexes[2] = swap;
		}
		else {
			/* nothing to do */
		}

		if (model->texture_cordinates_number) {
			swap = T(i).triangle_indexes[0];
			T(i).triangle_indexes[0] = T(i).triangle_indexes[2];
			T(i).triangle_indexes[2] = swap;
		}
		else {
			/* nothing to do */
		}
	}

	/* reverse facet normals */
	for (GLuint i = 1; i <= model->number_of_facet_normals; i++) {
		model->facet_normals[3 * i + 0] = -model->facet_normals[3 * i + 0];
		model->facet_normals[3 * i + 1] = -model->facet_normals[3 * i + 1];
		model->facet_normals[3 * i + 2] = -model->facet_normals[3 * i + 2];
	}

	/* reverse vertex normals */
	for (GLuint i = 1; i <= model->number_of_normals_in_model; i++) {
		model->normals[3 * i + 0] = -model->normals[3 * i + 0];
		model->normals[3 * i + 1] = -model->normals[3 * i + 1];
		model->normals[3 * i + 2] = -model->normals[3 * i + 2];
	}
}

/*
 * glmFacetNormals: Generates facet normals for a model (by taking the
 * cross product of the two vectors derived from the sides of each
 * triangle).  Assumes a counter-clockwise winding.
 *
 * model - initialized GLMmodel structure
 */
GLvoid
glmFacetNormals(GLMmodel *model)
{
	GLfloat u[3];
	GLfloat v[3];

	assert(model);
	assert(model->vertex);

	/* clobber any old facetnormals */
	if (model->facet_normals){
		free(model->facet_normals);
	}
	else {
		/* nothing to do */
	}

	/* allocate memory for the new facet normals */
	model->number_of_facet_normals = model->number_of_triangles;
	model->facet_normals = (GLfloat*)malloc(sizeof(GLfloat) *
	                     3 * (model->number_of_facet_normals + 1));

	for (GLuint i = 0; i < model->number_of_triangles; i++) {
		model->triangles[i].findex = i+1;

		u[0] = model->vertex[3 * T(i).vertex_indexes[1] + 0] -
			   model->vertex[3 * T(i).vertex_indexes[0] + 0];
		u[1] = model->vertex[3 * T(i).vertex_indexes[1] + 1] -
			   model->vertex[3 * T(i).vertex_indexes[0] + 1];
		u[2] = model->vertex[3 * T(i).vertex_indexes[1] + 2] -
			   model->vertex[3 * T(i).vertex_indexes[0] + 2];

		v[0] = model->vertex[3 * T(i).vertex_indexes[2] + 0] -
			   model->vertex[3 * T(i).vertex_indexes[0] + 0];
		v[1] = model->vertex[3 * T(i).vertex_indexes[2] + 1] -
			   model->vertex[3 * T(i).vertex_indexes[0] + 1];
		v[2] = model->vertex[3 * T(i).vertex_indexes[2] + 2] -
			   model->vertex[3 * T(i).vertex_indexes[0] + 2];

		glmCross(u, v, &model->facet_normals[3 * (i+1)]);
		glmNormalize(&model->facet_normals[3 * (i+1)]);
	}
}

/*
 * glmVertexNormals: Generates smooth vertex normals for a model.
 *
 * First builds a list of all the triangles each vertex is in.	Then
 * loops through each vertex in the the list averaging all the facet
 * normals of the triangles each vertex is in.   Finally, sets the
 * normal index in the triangle for the vertex to the generated smooth
 * normal.   If the dot product of a facet normal and the facet normal
 * associated with the first triangle in the list of triangles the
 * current vertex is in is greater than the cosine of the angle
 * parameter to the function, that facet normal is not added into the
 * average normal calculation and the corresponding vertex is given
 * the facet normal.  This tends to preserve hard edges.  The angle to
 * use depends on the model, but 90 degrees is usually a good start.
 *
 * model - initialized GLMmodel structure
 * angle - maximum angle (in degrees) to smooth across
 */
GLvoid
glmVertexNormals(GLMmodel *model, GLfloat angle)
{
	GLMnode *node;
	GLMnode *tail;
	GLMnode **members;
	GLfloat *normals;
	GLuint  number_of_normals_in_model;
	GLfloat average[3];
	GLfloat dot;
	GLfloat angle_in_cosine;
	GLuint  average_aux;

	assert(model);
	assert(model->facet_normals);

	/* calculate the cosine of the angle (in degrees) */
	angle_in_cosine = cos(angle * M_PI / 180.0);

	/* nuke any previous normals */
	if (model->normals) {
		free(model->normals);
	}
	else {
		/* nothing to do */
	}

	/* allocate space for new normals */
	model->number_of_normals_in_model = model->number_of_triangles * 3; /* 3 normals per triangle */
	model->normals = (GLfloat *)malloc(sizeof(GLfloat) *
	                  3 * (model->number_of_normals_in_model + 1));

	/*
	 * Allocate a structure that will hold a linked list of triangle
	 * indices for each vertex
	 */
	members = (GLMnode**)malloc(sizeof(GLMnode *) * (model->vertices_numbers + 1));
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		members[i] = NULL;
	}

	/* for every triangle, create a node for each vertex in it */
	for (GLuint i = 0; i < model->number_of_triangles; i++) {
		node = (GLMnode *)malloc(sizeof(GLMnode));
		node->index = i;
		node->next  = members[T(i).vertex_indexes[0]];
		members[T(i).vertex_indexes[0]] = node;

		node = (GLMnode *)malloc(sizeof(GLMnode));
		node->index = i;
		node->next  = members[T(i).vertex_indexes[1]];
		members[T(i).vertex_indexes[1]] = node;

		node = (GLMnode *)malloc(sizeof(GLMnode));
		node->index = i;
		node->next  = members[T(i).vertex_indexes[2]];
		members[T(i).vertex_indexes[2]] = node;
	}

	/* calculate the average normal for each vertex */
	number_of_normals_in_model = 1;
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
	/*
	 * Calculate an average normal for this vertex
	 * by averaging the facet normal of every triangle this vertex is in.
	 */
		node = members[i];
		if (!node) {
			fprintf(stderr, "glmVertexNormals(): vertex w/o a triangle\n");
		}
		else {
			/* nothing to do */
		}

		average[0] = 0.0; average[1] = 0.0; average[2] = 0.0;
		average_aux = 0;
		while (node) {
		/*
		 * Only average if the dot product of the angle between the two
		 * facet normals is greater than the cosine of the threshold
		 * angle -- or, said another way, the angle between the two
		 * facet normals is less than (or equal to) the threshold angle
		 */
			dot = glmDot(&model->facet_normals[3 * T(node->index).findex],
				&model->facet_normals[3 * T(members[i]->index).findex]);
			if (dot > angle_in_cosine) {
				node->averaged = GL_TRUE;
				average[0] += model->facet_normals[3 * T(node->index).findex + 0];
				average[1] += model->facet_normals[3 * T(node->index).findex + 1];
				average[2] += model->facet_normals[3 * T(node->index).findex + 2];
				average_aux = 1;            /* we averaged at least one normal! */
			}
			else {
				node->averaged = GL_FALSE;
			}
			node = node->next;
		}

		if (average_aux) {
			/* normalize the averaged normal */
			glmNormalize(average);

			/* add the normal to the vertex normals list */
			model->normals[3 * number_of_normals_in_model + 0] = average[0];
			model->normals[3 * number_of_normals_in_model + 1] = average[1];
			model->normals[3 * number_of_normals_in_model + 2] = average[2];
			average_aux = number_of_normals_in_model;
			number_of_normals_in_model++;
		}
		else {
			/* nothing to do */
		}

		/* set the normal of this vertex in each triangle it is in */
		node = members[i];
		while (node) {
			if (node->averaged) {
				/* if this node was averaged, use the average normal */
				if (T(node->index).vertex_indexes[0] == i) {
					T(node->index).normal_indexes[0] = average_aux;
				}
				else if (T(node->index).vertex_indexes[1] == i) {
					T(node->index).normal_indexes[1] = average_aux;
				}
				else if (T(node->index).vertex_indexes[2] == i) {
					T(node->index).normal_indexes[2] = average_aux;
				}
				else {
					/* nothing to do */
				}
			}
			else {
				/* if this node wasn't averaged, use the facet normal */
				model->normals[3 * number_of_normals_in_model + 0] =
					model->facet_normals[3 * T(node->index).findex + 0];
				model->normals[3 * number_of_normals_in_model + 1] =
					model->facet_normals[3 * T(node->index).findex + 1];
				model->normals[3 * number_of_normals_in_model + 2] =
					model->facet_normals[3 * T(node->index).findex + 2];
				if (T(node->index).vertex_indexes[0] == i) {
					T(node->index).normal_indexes[0] = number_of_normals_in_model;
				}
				else if (T(node->index).vertex_indexes[1] == i) {
					T(node->index).normal_indexes[1] = number_of_normals_in_model;
				}
				else if (T(node->index).vertex_indexes[2] == i) {
					T(node->index).normal_indexes[2] = number_of_normals_in_model;
				}
				else {
					/* nothing to do */
				}
				number_of_normals_in_model++;
			}
			node = node->next;
		}
	}

	model->number_of_normals_in_model = number_of_normals_in_model - 1;

	/* free the member information */
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		node = members[i];
		while (node) {
			tail = node;
			node = node->next;
			free(tail);
		}
	}
	free(members);

	/*
	 * Pack the normals array (we previously allocated the maximum
	 * number of normals that could possibly be created (number_of_triangles * 3),
	 * so get rid of some of them (usually alot unless none of the
	 * facet normals were averaged)).
	 */
	normals = model->normals;
	model->normals = (GLfloat *)malloc(sizeof(GLfloat) *
	                  3 * (model->number_of_normals_in_model+1));
	for (GLuint i = 1; i <= model->number_of_normals_in_model; i++) {
		model->normals[3 * i + 0] = normals[3 * i + 0];
		model->normals[3 * i + 1] = normals[3 * i + 1];
		model->normals[3 * i + 2] = normals[3 * i + 2];
	}
	free(normals);
}

/*
 * glmLinearTexture: Generates texture coordinates according to a
 * linear projection of the texture map.  It generates these by
 * linearly mapping the vertex onto a square.
 *
 * model - pointer to initialized GLMmodel structure
 */
GLvoid
glmLinearTexture(GLMmodel *model)
{
	GLMgroup *group;
	GLfloat dimensions[3];
	GLfloat x;
	GLfloat y;
	GLfloat scalefactor;

	assert(model);

	if (model->texture_cordinates) {
		free(model->texture_cordinates);
	}
	else {
		/* nothing to do */
	}
	model->texture_cordinates_number = model->vertices_numbers;
	model->texture_cordinates=(GLfloat *)malloc(sizeof(GLfloat) *
	                  2 * (model->texture_cordinates_number + 1));

	glmDimensions(model, dimensions);
	scalefactor = 2.0 /
		glmAbs(glmMax(glmMax(dimensions[0], dimensions[1]), dimensions[2]));

	/* do the calculations */
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		x = model->vertex[3 * i + 0] * scalefactor;
		y = model->vertex[3 * i + 2] * scalefactor;
		model->texture_cordinates[2 * i + 0] = (x + 1.0) / 2.0;
		model->texture_cordinates[2 * i + 1] = (y + 1.0) / 2.0;
	}

	/* go through and put texture coordinate indices in all the triangles */
	group = model->groups;
	while (group) {
		for (GLuint i = 0; i < group->number_of_triangles; i++) {
			T(group->triangles[i]).triangle_indexes[0] =
			    T(group->triangles[i]).vertex_indexes[0];
			T(group->triangles[i]).triangle_indexes[1] =
				T(group->triangles[i]).vertex_indexes[1];
			T(group->triangles[i]).triangle_indexes[2] =
				T(group->triangles[i]).vertex_indexes[2];
		}
		group = group->next;
	}

	#if 0
	printf("glmLinearTexture(): generated %d linear texture coordinates\n",
		model->texture_cordinates_number);
	#endif
}

/*
 * glmSpheremapTexture: Generates texture coordinates according to a
 * spherical projection of the texture map.  Sometimes referred to as
 * spheremap, or reflection map texture coordinates.  It generates
 * these by using the normal to calculate where that vertex would map
 * onto a sphere.  Since it is impossible to map something flat
 * perfectly onto something spherical, there is distortion at the
 * poles.  This particular implementation causes the poles along the X
 * axis to be distorted.
 *
 * model - pointer to initialized GLMmodel structure
 */
GLvoid
glmSpheremapTexture(GLMmodel *model)
{
	GLMgroup *group;
	GLfloat theta, phi, rho, x, y, z, r;

	assert(model);
	assert(model->normals);

	if (model->texture_cordinates) {
		free(model->texture_cordinates);
	}
	else {
		/* nothing to do */
	}
	model->texture_cordinates_number = model->number_of_normals_in_model;
	model->texture_cordinates=(GLfloat*)malloc(sizeof(GLfloat) *
	                  2 * (model->texture_cordinates_number + 1));

	for (GLuint i = 1; i <= model->number_of_normals_in_model; i++) {
		z = model->normals[3 * i + 0];  /* re-arrange for pole distortion */
		y = model->normals[3 * i + 1];
		x = model->normals[3 * i + 2];
		r = sqrt((x * x) + (y * y));
		rho = sqrt((r * r) + (z * z));

		if (r == 0.0) {
			theta = 0.0;
			phi = 0.0;
		} else {
			if(z == 0.0) {
				phi = 3.14159265 / 2.0;
			}
			else {
				phi = acos(z / rho);
			}

			if(y == 0.0) {
				theta = 3.141592365 / 2.0;
			}
			else {
				theta = asin(y / r) + (3.14159265 / 2.0);
			}
		}

		model->texture_cordinates[2 * i + 0] = theta / 3.14159265;
		model->texture_cordinates[2 * i + 1] = phi / 3.14159265;
	}

	/* go through and put texcoord indices in all the triangles */
	group = model->groups;
	while (group) {
		for (GLuint i = 0; i < group->number_of_triangles; i++) {
			T(group->triangles[i]).triangle_indexes[0] = T(group->triangles[i]).normal_indexes[0];
			T(group->triangles[i]).triangle_indexes[1] = T(group->triangles[i]).normal_indexes[1];
			T(group->triangles[i]).triangle_indexes[2] = T(group->triangles[i]).normal_indexes[2];
		}
		group = group->next;
	}
}

/*
 * glmDelete: Deletes a GLMmodel structure.
 *
 * model - initialized GLMmodel structure
 */
GLvoid
glmDelete(GLMmodel *model)
{
	GLMgroup *group;

	assert(model);

	if (model->path_name) {
		free(model->path_name);
	}
	else {
		/* nothing to do */
	}

	if (model->mtl_lib_name) {
		free(model->mtl_lib_name);
	}
	else {
		/* nothing to do */
	}

	if (model->vertex) {
		free(model->vertex);
	}
	else {
		/* nothing to do */
	}

	if (model->normals) {
		free(model->normals);
	}
	else {
		/* nothing to do */
	}

	if (model->texture_cordinates) {
		free(model->texture_cordinates);
	}
	else {
		/* nothing to do */
	}

	if (model->facet_normals) {
		free(model->facet_normals);
	}
	else {
		/* nothing to do */
	}

	if (model->triangles) {
		free(model->triangles);
	}
	else {
		/* nothing to do */
	}

	if (model->materials) {
		for (GLuint i = 0; i < model->number_of_materials; i++) {
			free(model->materials[i].name);
		}
	}
	else {
		/* nothing to do */
	}

	free(model->materials);
	while (model->groups) {
		group = model->groups;
		model->groups = model->groups->next;
		free(group->name);
		free(group->triangles);
		free(group);
	}

	free(model);
}

/*
 * glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * glmDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.
 */
GLMmodel*
glmReadOBJ(char *filename)
{
	GLMmodel *model;
	FILE *file;

	/* open the file */
	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "glmReadOBJ() failed: can't open data file \"%s\".\n",
		    filename);
		exit(1);
	}

	/* allocate a new model */
	model = (GLMmodel *)malloc(sizeof(GLMmodel));
	model->path_name = strdup(filename);
	model->mtl_lib_name = NULL;
	model->vertices_numbers = 0;
	model->vertex = NULL;
	model->number_of_normals_in_model = 0;
	model->normals = NULL;
	model->texture_cordinates_number = 0;
	model->texture_cordinates = NULL;
	model->number_of_facet_normals = 0;
	model->facet_normals = NULL;
	model->number_of_triangles = 0;
	model->triangles = NULL;
	model->number_of_materials = 0;
	model->materials = NULL;
	model->number_of_groups = 0;
	model->groups = NULL;
	model->position[0] = 0.0;
	model->position[1] = 0.0;
	model->position[2] = 0.0;

	/*
	 * Make a first pass through the file to get a count of the number
	 * of vertex, normals, texture_cordinates & triangles
	 */
	glmFirstPass(model, file);

	/* allocate memory */
	model->vertex = (GLfloat *)malloc(sizeof(GLfloat) *
		              3 * (model->vertices_numbers + 1));
	model->triangles = (GLMtriangle *)malloc(sizeof(GLMtriangle) *
		               model->number_of_triangles);
	if (model->number_of_normals_in_model) {
		model->normals = (GLfloat*)malloc(sizeof(GLfloat) *
			             3 * (model->number_of_normals_in_model + 1));
	}
	else {
		/* nothing to do */
	}

	if (model->texture_cordinates_number) {
		model->texture_cordinates = (GLfloat *)malloc(sizeof(GLfloat) *
			               2 * (model->texture_cordinates_number + 1));
	}
	else {
		/* nothing to do */
	}

	/* rewind to beginning of file and read in the data this pass */
	rewind(file);

	glmSecondPass(model, file);

	/* close the file */
	fclose(file);

	return model;
}

/*
 * glmWriteOBJ: Writes a model description in Wavefront .OBJ format to
 * a file.
 *
 * model - initialized GLMmodel structure
 * filename - name of the file to write the Wavefront .OBJ format data to
 * mode  - a bitwise or of values describing what is written to the file
 *
 * GLM_NONE     -  render with only vertex
 * GLM_FLAT     -  render with facet normals
 * GLM_SMOOTH   -  render with vertex normals
 * GLM_TEXTURE  -  render with texture coords
 * GLM_COLOR    -  render with colors (color material)
 * GLM_MATERIAL -  render with materials
 * GLM_COLOR and GLM_MATERIAL should not both be specified.
 * GLM_FLAT and GLM_SMOOTH should not both be specified.
 */
GLvoid
glmWriteOBJ(GLMmodel *model, char *filename, GLuint mode)
{
	FILE *file;
	GLMgroup *group;

	assert(model);

	/* do a bit of warning */
	if (mode & GLM_FLAT && !model->facet_normals) {
		printf("glmWriteOBJ() warning: flat normal output requested "
			   "with no facet normals defined.\n");
		mode &= ~GLM_FLAT;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_SMOOTH && !model->normals) {
		printf("glmWriteOBJ() warning: smooth normal output requested "
			   "with no normals defined.\n");
		mode &= ~GLM_SMOOTH;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_TEXTURE && !model->texture_cordinates) {
		printf("glmWriteOBJ() warning: texture coordinate output requested "
			   "with no texture coordinates defined.\n");
		mode &= ~GLM_TEXTURE;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
		printf("glmWriteOBJ() warning: flat normal output requested "
			   "and smooth normal output requested (using smooth).\n");
		mode &= ~GLM_FLAT;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_COLOR && !model->materials) {
		printf("glmWriteOBJ() warning: color output requested "
			   "with no colors (materials) defined.\n");
		mode &= ~GLM_COLOR;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_MATERIAL && !model->materials) {
		printf("glmWriteOBJ() warning: material output requested "
			   "with no materials defined.\n");
		mode &= ~GLM_MATERIAL;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
		printf("glmWriteOBJ() warning: color and material output requested "
			   "outputting only materials.\n");
		mode &= ~GLM_COLOR;
	}
	else {
		/* nothing to do */
	}

	/* open the file */
	file = fopen(filename, "w");
	if (!file) {
		fprintf(stderr,
		        "glmWriteOBJ() failed: can't open file \"%s\" to write.\n",
			    filename);
		exit(1);
	}
	else {
		/* nothing to do */
	}

	/* spit out a header */
	fprintf(file, "#  \n");
	fprintf(file, "#  Wavefront OBJ generated by GLM library\n");
	fprintf(file, "#  \n");
	fprintf(file, "#  GLM library\n");
	fprintf(file, "#  Nate Robins\n");
	fprintf(file, "#  ndr@pobox.com\n");
	fprintf(file, "#  http://www.pobox.com/~ndr\n");
	fprintf(file, "#  \n");

	if (mode & GLM_MATERIAL && model->mtl_lib_name) {
		fprintf(file, "\nmtllib %s\n\n", model->mtl_lib_name);
		glmWriteMTL(model, filename, model->mtl_lib_name);
	}
	else {
		/* nothing to do */
	}

	/* spit out the vertex */
	fprintf(file, "\n");
	fprintf(file, "# %d vertex\n", model->vertices_numbers);
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		fprintf(file, "v %f %f %f\n",
			model->vertex[3 * i + 0],
			model->vertex[3 * i + 1],
			model->vertex[3 * i + 2]);
	}

	/* spit out the smooth/flat normals */
	if (mode & GLM_SMOOTH) {
		fprintf(file, "\n");
		fprintf(file, "# %d normals\n", model->number_of_normals_in_model);
		for (GLuint i = 1; i <= model->number_of_normals_in_model; i++) {
			fprintf(file, "vn %f %f %f\n",
				model->normals[3 * i + 0],
				model->normals[3 * i + 1],
				model->normals[3 * i + 2]);
		}
	}
	else if (mode & GLM_FLAT) {
		fprintf(file, "\n");
		fprintf(file, "# %d normals\n", model->number_of_facet_normals);
		for (GLuint i = 1; i <= model->number_of_normals_in_model; i++) {
			fprintf(file, "vn %f %f %f\n",
			        model->facet_normals[3 * i + 0],
			        model->facet_normals[3 * i + 1],
			        model->facet_normals[3 * i + 2]);
		}
	}
	else {
		/* nothing to do */
	}

	/* spit out the texture coordinates */
	if (mode & GLM_TEXTURE) {
		fprintf(file, "\n");
		fprintf(file, "# %f texture_cordinates\n", *model->texture_cordinates);
		for (GLuint i = 1; i <= model->texture_cordinates_number; i++) {
			fprintf(file, "vt %f %f\n",
			        model->texture_cordinates[2 * i + 0],
			        model->texture_cordinates[2 * i + 1]);
		}
	}
	else {
		/* nothing to do */
	}

	fprintf(file, "\n");
	fprintf(file, "# %d groups\n", model->number_of_groups);
	fprintf(file, "# %d faces (triangles)\n", model->number_of_triangles);
	fprintf(file, "\n");

	group = model->groups;
	while(group) {
		fprintf(file, "g %s\n", group->name);
		if (mode & GLM_MATERIAL) {
			fprintf(file, "usemtl %s\n",
			        model->materials[group->material].name);
		}
		else {
			/* nothing to do */
		}

		for (GLuint i = 0; i < group->number_of_triangles; i++) {
			if (mode & GLM_SMOOTH && mode & GLM_TEXTURE) {
				fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					T(group->triangles[i]).vertex_indexes[0],
					T(group->triangles[i]).normal_indexes[0],
					T(group->triangles[i]).triangle_indexes[0],
					T(group->triangles[i]).vertex_indexes[1],
					T(group->triangles[i]).normal_indexes[1],
					T(group->triangles[i]).triangle_indexes[1],
					T(group->triangles[i]).vertex_indexes[2],
					T(group->triangles[i]).normal_indexes[2],
					T(group->triangles[i]).triangle_indexes[2]);
			}
			else if (mode & GLM_FLAT && mode & GLM_TEXTURE) {
				fprintf(file, "f %d/%d %d/%d %d/%d\n",
					T(group->triangles[i]).vertex_indexes[0],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vertex_indexes[1],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vertex_indexes[2],
					T(group->triangles[i]).findex);
			}
			else if (mode & GLM_TEXTURE) {
				fprintf(file, "f %d/%d %d/%d %d/%d\n",
					T(group->triangles[i]).vertex_indexes[0],
					T(group->triangles[i]).triangle_indexes[0],
					T(group->triangles[i]).vertex_indexes[1],
					T(group->triangles[i]).triangle_indexes[1],
					T(group->triangles[i]).vertex_indexes[2],
					T(group->triangles[i]).triangle_indexes[2]);
			}
			else if (mode & GLM_SMOOTH) {
				fprintf(file, "f %d//%d %d//%d %d//%d\n",
					T(group->triangles[i]).vertex_indexes[0],
					T(group->triangles[i]).normal_indexes[0],
					T(group->triangles[i]).vertex_indexes[1],
					T(group->triangles[i]).normal_indexes[1],
					T(group->triangles[i]).vertex_indexes[2],
					T(group->triangles[i]).normal_indexes[2]);
			}
			else if (mode & GLM_FLAT) {
				fprintf(file, "f %d//%d %d//%d %d//%d\n",
					T(group->triangles[i]).vertex_indexes[0],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vertex_indexes[1],
					T(group->triangles[i]).findex,
					T(group->triangles[i]).vertex_indexes[2],
					T(group->triangles[i]).findex);
			}
			else {
				fprintf(file, "f %d %d %d\n",
					T(group->triangles[i]).vertex_indexes[0],
					T(group->triangles[i]).vertex_indexes[1],
					T(group->triangles[i]).vertex_indexes[2]);
			}
		}
		fprintf(file, "\n");
		group = group->next;
	}

	fclose(file);
}

/*
 * glmDraw: Renders the model to the current OpenGL context using the
 * mode specified.
 *
 * model - initialized GLMmodel structure
 * mode  - a bitwise OR of values describing what is to be rendered.
 *
 * GLM_NONE     -  render with only vertex
 * GLM_FLAT     -  render with facet normals
 * GLM_SMOOTH   -  render with vertex normals
 * GLM_TEXTURE  -  render with texture coords
 * GLM_COLOR    -  render with colors (color material)
 * GLM_MATERIAL -  render with materials
 * GLM_COLOR and GLM_MATERIAL should not both be specified.
 * GLM_FLAT and GLM_SMOOTH should not both be specified.
 */
GLvoid
glmDraw(GLMmodel *model, GLuint mode)
{
	static GLMgroup *group;
	static GLMtriangle *triangle;
	static GLMmaterial *material;

	assert(model);
	assert(model->vertex);

	/* do a bit of warning */
	if (mode & GLM_FLAT && !model->facet_normals) {
		printf("glmDraw() warning: flat render mode requested "
		       "with no facet normals defined.\n");
		mode &= ~GLM_FLAT;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_SMOOTH && !model->normals) {
		printf("glmDraw() warning: smooth render mode requested "
		       "with no normals defined.\n");
		mode &= ~GLM_SMOOTH;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_TEXTURE && !model->texture_cordinates) {
		printf("glmDraw() warning: texture render mode requested "
		       "with no texture coordinates defined.\n");
		mode &= ~GLM_TEXTURE;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
		printf("glmDraw() warning: flat render mode requested "
		       "and smooth render mode requested (using smooth).\n");
		mode &= ~GLM_FLAT;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_COLOR && !model->materials) {
		printf("glmDraw() warning: color render mode requested "
		       "with no materials defined.\n");
		mode &= ~GLM_COLOR;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_MATERIAL && !model->materials) {
		printf("glmDraw() warning: material render mode requested "
		       "with no materials defined.\n");
		mode &= ~GLM_MATERIAL;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
		printf("glmDraw() warning: color and material render mode requested "
		       "using only material mode.\n");
		mode &= ~GLM_COLOR;
	}
	else {
		/* nothing to do */
	}

	if (mode & GLM_COLOR) {
		glEnable(GL_COLOR_MATERIAL);
	}
	else if (mode & GLM_MATERIAL) {
		glDisable(GL_COLOR_MATERIAL);
	}
	else {
		/* nothing to do */
	}

	/*
	 * Perhaps this loop should be unrolled into material, color, flat,
	 * smooth, etc. loops?  since most cpu's have good branch prediction
	 * schemes (and these branches will always go one way), probably
	 * wouldn't gain too much?
	 */

	group = model->groups;
	while (group) {
		if (mode & GLM_MATERIAL) {
			material = &model->materials[group->material];
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
		}

		if (mode & GLM_COLOR) {
			glColor3fv(material->diffuse);
		}
		else {
			/* nothing to do */
		}

		glBegin(GL_TRIANGLES);
		for (GLuint i = 0; i < group->number_of_triangles; i++) {
			triangle = &T(group->triangles[i]);

			if (mode & GLM_FLAT) {
				glNormal3fv(&model->facet_normals[3 * triangle->findex]);
			}
			else {
				/* nothing to do */
			}

			if (mode & GLM_SMOOTH) {
				glNormal3fv(&model->normals[3 * triangle->normal_indexes[0]]);
			}
			else {
				/* nothing to do */
			}

			if (mode & GLM_TEXTURE) {
				glTexCoord2fv(&model->texture_cordinates[2 * triangle->triangle_indexes[0]]);
			}
			else {
				/* nothing to do */
			}

			glVertex3fv(&model->vertex[3 * triangle->vertex_indexes[0]]);

			if (mode & GLM_SMOOTH) {
				glNormal3fv(&model->normals[3 * triangle->normal_indexes[1]]);
			}
			else {
				/* nothing to do */
			}

			if (mode & GLM_TEXTURE) {
				glTexCoord2fv(&model->texture_cordinates[2 * triangle->triangle_indexes[1]]);
			}
			else {
				/* nothing to do */
			}

			glVertex3fv(&model->vertex[3 * triangle->vertex_indexes[1]]);

			if (mode & GLM_SMOOTH) {
				glNormal3fv(&model->normals[3 * triangle->normal_indexes[2]]);
			}
			else {
				/* nothing to do */
			}

			if (mode & GLM_TEXTURE) {
				glTexCoord2fv(&model->texture_cordinates[2 * triangle->triangle_indexes[2]]);
			}
			else {
				/* nothing to do */
			}

			glVertex3fv(&model->vertex[3 * triangle->vertex_indexes[2]]);

		}
		glEnd();

		group = group->next;
	}
}

/*
 * glmList: Generates and returns a display list for the model using
 * the mode specified.
 *
 * model - initialized GLMmodel structure
 * mode  - a bitwise OR of values describing what is to be rendered.
 *
 * GLM_NONE     -  render with only vertex
 * GLM_FLAT     -  render with facet normals
 * GLM_SMOOTH   -  render with vertex normals
 * GLM_TEXTURE  -  render with texture coords
 * GLM_COLOR    -  render with colors (color material)
 * GLM_MATERIAL -  render with materials
 * GLM_COLOR and GLM_MATERIAL should not both be specified.
 * GLM_FLAT and GLM_SMOOTH should not both be specified.
 */
GLuint
glmList(GLMmodel *model, GLuint mode)
{
	GLuint list;

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	glmDraw(model, mode);
	glEndList();

	return list;
}

/*
 * glmWeld: eliminate (weld) vectors that are within an epsilon of
 * each other.
 *
 * model   - initialized GLMmodel structure
 * epsilon     - maximum difference between vertex
 *               ( 0.00001 is a good start for a unitized model)
 *
 */
GLvoid
glmWeld(GLMmodel *model, GLfloat epsilon)
{
	GLfloat *vectors;
	GLfloat *copies;
	GLuint number_of_vectors;

	/* vertex */
	number_of_vectors = model->vertices_numbers;
	vectors  = model->vertex;
	copies = glmWeldVectors(vectors, &number_of_vectors, epsilon);

	#if 0
	printf("glmWeld(): %d redundant vertex.\n",
		model->vertices_numbers - number_of_vectors - 1);
	#endif

	for (GLuint i = 0; i < model->number_of_triangles; i++) {
		T(i).vertex_indexes[0] = (GLuint)vectors[3 * T(i).vertex_indexes[0] + 0];
		T(i).vertex_indexes[1] = (GLuint)vectors[3 * T(i).vertex_indexes[1] + 0];
		T(i).vertex_indexes[2] = (GLuint)vectors[3 * T(i).vertex_indexes[2] + 0];
	}

	/* free space for old vertex */
	free(vectors);

	/* allocate space for the new vertex */
	model->vertices_numbers = number_of_vectors;
	model->vertex = (GLfloat*)malloc(sizeof(GLfloat) *
	                  3 * (model->vertices_numbers + 1));

	/* copy the optimized vertex into the actual vertex list */
	for (GLuint i = 1; i <= model->vertices_numbers; i++) {
		model->vertex[3 * i + 0] = copies[3 * i + 0];
		model->vertex[3 * i + 1] = copies[3 * i + 1];
		model->vertex[3 * i + 2] = copies[3 * i + 2];
	}

	free(copies);
}

/*
 * glmReadPPM: read a PPM raw (type P6) file.  The PPM file has a header
 * that should look something like:
 *
 *     P6
 *     # comment
 *     width height max_value
 *     rgbrgbrgb...
 *
 * where "P6" is the magic cookie which identifies the file type and
 * should be the only characters on the first line followed by a
 * carriage return.  Any line starting with a # mark will be treated
 * as a comment and discarded.   After the magic cookie, three integer
 * values are expected: width, height of the image and the maximum
 * value for a pixel (max_value must be < 256 for PPM raw files).  The
 * data section consists of width*height rgb triplets (one byte each)
 * in binary format (i.e., such as that written with fwrite() or
 * equivalent).
 *
 * The rgb data is returned as an array of unsigned chars (packed
 * rgb).  The malloc()'d memory should be free()'d by the caller.  If
 * an error occurs, an error message is sent to stderr and NULL is
 * returned.
 *
 * filename - name of the .ppm file.
 * width    - will contain the width of the image on return.
 * height   - will contain the height of the image on return.
 */
GLubyte*
glmReadPPM(char *filename, int *width, int *height)
{
	FILE *fp;
	int i, w, h, d;
	unsigned char *image;
	char head[70]; /* max line <= 70 in PPM (per spec) */

	fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return NULL;
	}
	else {
		/* nothing to do */
	}

	/*
	 * grab first two chars of the file and make sure that it has the
	 * correct magic cookie for a raw PPM file.
	 */
	fgets(head, 70, fp);
	if (strncmp(head, "P6", 2)) {
		fprintf(stderr, "%s: Not a raw PPM file\n", filename);
		return NULL;
	}
	else {
		/* nothing to do */
	}

	/* grab the three elements in the header (width, height, maxval). */
	i = 0;
	while(i < 3) {
		fgets(head, 70, fp);
		if (head[0] == '#') {     /* skip comments. */
			continue;
		}
		else {
			/* nothing to do */
		}

		if (i == 0) {
			i += sscanf(head, "%d %d %d", &w, &h, &d);
		}
		else if (i == 1) {
			i += sscanf(head, "%d %d", &h, &d);
		}
		else if (i == 2) {
			i += sscanf(head, "%d", &d);
		}
		else {
			/* nothing to do */
		}
	}

	/* grab all the image data in one fell swoop. */
	image = (unsigned char *)malloc(sizeof(unsigned char) * w * h * 3);
	fread(image, sizeof(unsigned char), w * h * 3, fp);
	fclose(fp);

	*width = w;
	*height = h;
	return image;
}

#if 0
/* normals */
if (model->number_of_normals_in_model) {
	number_of_vectors = model->number_of_normals_in_model;
	vectors  = model->normals;
	copies = glmOptimizeVectors(vectors, &number_of_vectors);

	printf("glmOptimize(): %d redundant normals.\n",
		   model->number_of_normals_in_model - number_of_vectors);

	for (i = 0; i < model->number_of_triangles; i++) {
		T(i).normal_indexes[0] = (GLuint)vectors[3 * T(i).normal_indexes[0] + 0];
		T(i).normal_indexes[1] = (GLuint)vectors[3 * T(i).normal_indexes[1] + 0];
		T(i).normal_indexes[2] = (GLuint)vectors[3 * T(i).normal_indexes[2] + 0];
	}

	/* free space for old normals */
	free(vectors);

	/* allocate space for the new normals */
	model->number_of_normals_in_model = number_of_vectors;
	model->normals = (GLfloat*)malloc(sizeof(GLfloat) *
		             3 * (model->number_of_normals_in_model + 1));

	/* copy the optimized vertex into the actual vertex list */
	for (i = 1; i <= model->number_of_normals_in_model; i++) {
		model->normals[3 * i + 0] = copies[3 * i + 0];
		model->normals[3 * i + 1] = copies[3 * i + 1];
		model->normals[3 * i + 2] = copies[3 * i + 2];
	}

	free(copies);
}
else {
	/* nothing to do */
}

/* texture_cordinates */
if (model->texture_cordinates_number) {
	number_of_vectors = model->texture_cordinates_number;
	vectors  = model->texture_cordinates;
	copies = glmOptimizeVectors(vectors, &number_of_vectors);

	printf("glmOptimize(): %d redundant texture_cordinates.\n",
	       model->texture_cordinates_number - number_of_vectors);

	for (i = 0; i < model->number_of_triangles; i++) {
		for (j = 0; j < 3; j++) {
			T(i).triangle_indexes[j] = (GLuint)vectors[3 * T(i).triangle_indexes[j] + 0];
		}
	}

	/* free space for old texture_cordinates */
	free(vectors);

	/* allocate space for the new texture_cordinates */
	model->texture_cordinates_number = number_of_vectors;
	model->texture_cordinates = (GLfloat*)malloc(sizeof(GLfloat) *
		               2 * (model->texture_cordinates_number + 1));

	/* copy the optimized vertex into the actual vertex list */
	for (i = 1; i <= model->texture_cordinates_number; i++) {
		model->texture_cordinates[2 * i + 0] = copies[2 * i + 0];
		model->texture_cordinates[2 * i + 1] = copies[2 * i + 1];
	}

	free(copies);
}
else {
	/* nothing to do */
}
#endif

#if 0
/* look for unused vertex  */
/* look for unused normals   */
/* look for unused texture_cordinates */
for (i = 1; i <= model->vertices_numbers; i++) {
	for (j = 0; j < model->number_of_triangles; i++) {
		if (T(j).vertex_indexes[0] == i ||
			T(j).vertex_indexes[1] == i ||
			T(j).vertex_indexes[1] == i)
		{
			break;
		}
		else {
			/* nothing to do */
		}
	}
}
#endif
