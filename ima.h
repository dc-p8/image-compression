#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>	
#include <GL/glut.h>

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
};
typedef struct Image Image;
int parse(int argc, char **argv, unsigned char *mode, char **input, char **output, unsigned int *seed, int *nb_sites);
int ImageLoad_PPM(char *filename, Image *image);
void imagesave_PPM(char *filename, Image *image);

void voronoi(char *input, char *output, unsigned int seed, int nb_sites);
void devoronoi(char *input, char *output);
