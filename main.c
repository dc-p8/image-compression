// gcc -02 main.c modif.c ppm.c
// ./a.out -c -n 50 -s 42 -i Refuges.ppm -o tmp
// ./a.out -d -i tmp

#include "ima.h"

extern char * optarg; 
extern int optind, opterr;

int parse(int argc, char **argv, unsigned char *mode, char **input, char **output, unsigned int *seed, int *nb_sites)
{
	int i_flag = 0, o_flag = 0, s_flag = 0, c_flag = 0, d_flag = 0, n_flag = 0, errflg = 0;
	char *i_arg, *o_arg, *s_arg, *n_arg, c;

	while ((c = getopt(argc , argv, "cds:i:o:n:")) != -1)
	{
		switch (c)
		{
			case 'i':
				i_flag = 1;
				i_arg = optarg;
				break;
			case 'o':
				o_flag = 1;
				o_arg = optarg;
				break;
			case 'c':
				c_flag = 1;
				break;
			case 'd':
				d_flag = 1;
				break;
			case 's':
				s_flag++;
				s_arg = optarg;
				break;
			case 'n':
				n_flag++;
				n_arg = optarg;
				break;
			case '?':
				errflg++;
				break;
		}
	}

	if(!(c_flag ^ d_flag))
		errflg++;
	if(!c_flag && (n_flag || s_flag))
		errflg++;
	if(c_flag && !n_flag)
		errflg++;
	if(!i_flag)
		errflg++;

	if (errflg)
		return errflg;

	*input = strdup(i_arg);

	

	if(n_flag)
	{
		*nb_sites = atoi(n_arg);
		if(*nb_sites < 1 || *nb_sites > 0xFFF)
			errflg++;
	}

	*mode = c_flag != 0;

	if(o_flag)
		*output = strdup(o_arg);
	else
	{
		*output = strdup(*input);
		char *append;
		switch(*mode)
		{
			case 0 : append = strdup(".ppm");break;
			case 1 : append = strdup(".compress");break;
		}
		*output = realloc(*output, sizeof(char) * (strlen(*input) + strlen(append) + 1));
		strcat(*output, append);
		free(append);
	}

	if(s_flag)
		*seed = atoi(s_arg);
	else
		*seed = 0;

	return errflg;
}

int main(int argc , char **argv)
{
	unsigned int seed = 0;
	int nb_sites = 0;
	unsigned char mode = 0;
	char *input, *output, modes[][50] = {"decompression", "compression"};

	if(parse(argc, argv, &mode, &input, &output, &seed, &nb_sites))
	{
		fprintf(stderr, "usage: %s {[-c] [-n <numbers of sites, 0-255>] (-s <seed number>) | [-d]} [-i <input file>] (-o <output>)\n[] = mandatory arg. () = facultative arg. {} = group of args.\n", argv[0]);
		fprintf(stderr, "c for compress\nd for decompress\ni for input file\nn for number of sites\ns for random seed (0 by default)\no for output file ($(input).compressed by default)\n");
		return 1;
	}

	printf("Mode : %s\n", modes[mode]);
	printf("Input : %s\n", input);
	printf("Output : %s\n", output);
	printf("Seed : %u\n", seed);
	printf("sites : %u\n", nb_sites);
    

	int i = 100;
    switch(mode)
    {
    	case 0 :
    	{
    		//while(i--)
    			devoronoi(input, output);
    		break;
    	}
    	case 1: 
    	{
    		//while(i--)
    			voronoi(input, output, seed, nb_sites);
    		break;
    	}
    	
    }

	free(input);
	free(output);

	return 0;
}      

