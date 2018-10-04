#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int app = 0;
/*
	0 = sort
	1 = wordcount

	0 = threads
	1 = processes
*/
int impl = 0;
int num_maps = 2;
int num_reduces = 2;
char* inputfile = NULL;
char* outfile = NULL;
void validflags(int argc, char* argv[])
{
	int i = 0;
	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--app") == 0)
		{
			if (strcmp(argv[i + 1], "wordcount") == 0)
			{
				app = 1;
			}
			else if (strcmp(argv[i + 1], "sort") == 0)
			{
				app = 0;
			}
		}
		if (strcmp(argv[i], "--impl") == 0)
		{
			if (strcmp(argv[i + 1], "procs") == 0)
			{
				impl = 1;
			}
			else if (strcmp(argv[i + 1], "threads") == 0)
			{
				impl = 0;
			}
			
		}
		if (strcmp(argv[i], "--maps") == 0)
		{
			num_maps = atoi(argv[i + 1]);
			//printf("%d\n", num_maps);
		}
		if (strcmp(argv[i], "--reduces") == 0)
		{
			num_reduces = atoi(argv[i + 1]);
			//printf("%d\n", num_reduces);
		}
		if (strcmp(argv[i], "--input") == 0)
		{
			inputfile = strdup(argv[i + 1]);
			//printf("in %s\n", inputfile);
		}
		if (strcmp(argv[i], "--output") == 0)
		{
			outfile = strdup(argv[i + 1]);
			//printf("out %s\n", outfile);
		}
	}

	if (outfile == NULL)
	{
		printf("No output file selected, defaulting to out.txt\n");
		outfile = "out.txt";
	}
	if (inputfile == NULL)
	{
		printf("No input file selected...exiting\n");
		exit(0);
	}
}
int main (int argc, char* argv[]) {
	validflags(argc, argv);
}
