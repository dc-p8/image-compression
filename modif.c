#include "ima.h"

int nearest_site(GLubyte **sites, int nb_sites, int x, int y, int z)
{
	int i, ret;
	int d, min = 0;
	
	for(i = 0; i < nb_sites; i++)
	{
		d = (x - sites[i][0]) * (x -sites[i][0]) + (y - sites[i][1]) * (y - sites[i][1]) + (z - sites[i][2]) * (z - sites[i][2]);
		if(!i || d < min)
			min = d, ret = i;
	}
	return ret;
}

void devoronoi(char *input, char *output)
{
	FILE *f_in;
	Image *img;
	unsigned long size_img;
	unsigned int b, c, size;
	char format[3];
	unsigned char nb_pixel, size_spots;
	GLubyte **sites, *img_ptr, tmp, * ptrdeb, *ptrfin, *lastline;
	unsigned int nb_sites = 0, site;
	
	img = malloc(sizeof(Image));
	f_in = fopen(input, "rb");
	if(!f_in)
	{
		perror(input);
		return;
	}
	if(fread(format, 1, 2, f_in) != 2)
	{
		fprintf(stderr, "Error while parsing the file (format)\n");
		fclose(f_in);
		free(img);
		return;
	}
	format[2] = '\0';
	if(strcmp("VC", format))
	{
		fprintf(stderr, "Attempted format : \"VC\"\n");
		printf("FORMAT : %s\n", format);
		fclose(f_in);
		free(img);
		return;
	}
	if(!fread(&(img->sizeX), 8, 1, f_in) || !fread(&(img->sizeY), 8, 1, f_in))
	{
		fprintf(stderr, "Error while parsing the file (x y)\n");
		fclose(f_in);
		free(img);
		return;
	}
	
	//printf("sizeX : %ld, sizeY : %ld\n", img->sizeX, img->sizeY);
	size_img = img->sizeX * img->sizeY;
	img->data = malloc(sizeof(GLubyte) * size_img * 3);
	if(!fread(&nb_sites, 4, 1, f_in))
	{
		fprintf(stderr, "Error while parsing the file (nb sites)\n");
		fclose(f_in);
		free(img->data);
		free(img);
		
	}
	size_spots = 1 + (nb_sites > 0xFF);
	printf("nombers of sites : %d\n", nb_sites);

	sites = malloc(sizeof(GLubyte*) * nb_sites);
	for(site = 0; site < nb_sites; site++)
	{
		sites[site] = malloc(sizeof(GLubyte) * 3);
		if(fread(sites[site], 1, 3, f_in) != 3)
		{
			while(site)
				free(sites[site--]);
			free(sites);

			free(img->data);
			free(img);
			fprintf(stderr, "Error while parsing the file (colors)\n");
		}
		//printf("Color %2d : %02x %02x %02x\n", site, sites[site][0], sites[site][1], sites[site][2]);
	}

	img_ptr = img->data;
	while(1)
	{
		if(!fread(&nb_pixel, 1, 1, f_in) || !fread(&site, size_spots, 1, f_in))
			break;
		while(nb_pixel--)
		{
			img_ptr[0] = sites[site][0];
			img_ptr[1] = sites[site][1];
			img_ptr[2] = sites[site][2];
			img_ptr += 3;
		}
	}

	size = img->sizeX * img->sizeY * 3;
	lastline = img->data + size - img->sizeX * 3;
	for (b = 0; b < img->sizeY / 2; b++) {
	  ptrdeb = img->data + b * img->sizeX * 3;
	  ptrfin = lastline - (b * img->sizeX * 3);
	  for (c = 0; c < 3 * img->sizeX; c++) {
		  tmp = *ptrdeb;
		  *ptrdeb = *ptrfin;
		  *ptrfin = tmp;
		  ptrfin++;
		  ptrdeb++;
	  }		
	}

	imagesave_PPM(output, img);
	free(img->data);
	free(img);
	fclose(f_in);
	for(site = 0; site < nb_sites; site++)
		free(sites[site]);
	free(sites);
}

void voronoi(char *input, char *output, unsigned int seed, int nb_sites)
{
	srand(seed);
	FILE *f_out;
	Image *img;
	GLubyte **sites, *im_ptr;
	unsigned char size_spots, j;
	char *format = "VC";
	int *allcolors, i,size_img, couleur;
	unsigned int site, prec;

	size_spots = 1 + (nb_sites > 0xFF);

	f_out = fopen(output, "wb");
	if(!f_out)
	{
		perror(output);
		return;
	}
	img = malloc(sizeof(Image));
	ImageLoad_PPM(input, img);
	size_img = img->sizeX * img->sizeY,
	im_ptr = img->data;
	allcolors = calloc(sizeof(int), 0x1000000);

	sites = malloc(sizeof(char *)* nb_sites);
	for(i = 0; i < nb_sites; i++)
	{

		sites[i] = malloc(sizeof(GLubyte) * 3);
		GLubyte *pos = img->data + (3 * (rand() % (size_img)));
		sites[i][0] = pos[0];
		sites[i][1] = pos[1];
		sites[i][2] = pos[2];
	}
	
	//LIMITES DE LALGO PAS OUBLIER
	fwrite(format, 1, 2, f_out);
	fwrite(&img->sizeX, 8, 1, f_out);
	fwrite(&img->sizeY, 8, 1, f_out);
	fwrite(&nb_sites, 4, 1, f_out);
	for(i = 0; i < nb_sites; i++)
	{
		fwrite(sites[i], 1, 3, f_out);
	}

	for(i = 0; i < 0x1000000; i++)
		allcolors[i] = -1;

	couleur = (im_ptr[0] << 16) + (im_ptr[1] << 8) + im_ptr[2];
	prec = allcolors[couleur] = site = nearest_site(sites, nb_sites, im_ptr[0], im_ptr[1], im_ptr[2]);
	im_ptr += 3;
	for(i = 1, j = 1; i < size_img; i++, j++, im_ptr += 3, prec = site)
	{
		couleur = (im_ptr[0] << 16) + (im_ptr[1] << 8) + im_ptr[2];

		if(allcolors[couleur] == -1)
			allcolors[couleur] = nearest_site(sites, nb_sites, im_ptr[0], im_ptr[1], im_ptr[2]);

		site = allcolors[couleur];
		if(site != prec || j == 0xFF)
		{
			fwrite(&j, 1, 1, f_out);
			fwrite(&prec, size_spots, 1, f_out);
			j = 0;
		}
	}
	fwrite(&j, 1, 1, f_out);
	fwrite(&prec, size_spots, 1, f_out);

	for(i = 0; i < nb_sites; i++)
		free(sites[i]);
	free(sites);
	free(allcolors);
	fclose(f_out);
	free(img->data);
	free(img);

	return;
}
