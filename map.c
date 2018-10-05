
//Im just doing this the way my TA explained it. Which was very unintuitive.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int app = 0;
int impl = 0;
int num_maps = 2;
int num_reduces = 2;
char* inputfile = NULL;
char* outfile = NULL;
int total= 0;
typedef struct node {
    int val;
    char* word;
    int count;
    struct node * next;
}node;
/*
	Map will be called once per pthread/process where # of pthreads/processes is equal to num_maps
	Map for wordcount will generate a key/value pair where key = unique word and value = # of times it is seen
	Map will put its list of key/value pairs into shared memory for reduces threads/processes to use
*/
void map(node* head,int wordcount)
{
}

/*
	Inputreader should take the inputfile and break it into similar size chunks for use in the parallel map functions
*/
node* inputreader(char* filename, int wordcount)
{
	FILE* fp = fopen(filename, "r");
	char word[250];
	// counting words
	if (fp == NULL)
		printf("FAIL FAIL FAIL\n");
	node* head = NULL;
	node* tail = NULL;
	node* next = NULL;
	int val;
	if(wordcount == 0){
		while(fscanf(fp,"%s",word)==1){
			if(head = NULL){
				head = (node*)malloc(sizeof(node));
				head->word = word;
				tail = head;
			}
			else{
				next = (node*)malloc(sizeof(node));
				next->word = word;
				//this line seems odd
				//tail->next = next;
				tail = next;
			}
			//printf("STARTING %d\n", total);
			total++;
		}
	}
	//not counting words
	else{
		while(fscanf(fp,"%d",val)==1){
			if(head = NULL){
				head = (node*)malloc(sizeof(node));
				head->val = val;
				tail = head;
			}
			else{
				next = (node*)malloc(sizeof(node));
				next->val = val;
				tail->next = next;
				tail = next;
			}
			total++;
		}
	}
	//fclose(fp);
	return head;
}
/*
Mapper splits the code and either sends it into map threads or processes
*/
node* mapper(node* head, int wordcount, int procs,int maps){
	node*curr = head;
	node*next;
	int i = 1;
	double s = total/maps;
	int size = 0;

	if(ceil(s)*(maps-1)<total){
		size = ceil(s);
	}
	else{
		size = floor(s);
	}
	int currmap = 1;
	int pid;
	int pid_t = pid;
	pthread_t tid; 
	//processes
	if(procs == 0){
		while(curr != NULL)	{
			if(currmap == maps){
				break;
			}
			if(size%i==0){
				next = curr->next;
				curr->next = NULL;
				pid = fork();
				if(pid == 0){
					map(head,wordcount);
				}
				else{
					head = next;
					curr = head;
				}
				
			}
			
			i++;
			curr = curr->next;
		}
		pid = fork();
		if(pid == 0){
			map(head,wordcount);
		}
		//JOIN PROCESSES
	}
	//threads
	else{
		while(curr != NULL)	{
			if(currmap == maps){
				break;
			}
			if(size%i==0){
				next = curr->next;
				curr->next = NULL;
				//CREATE THREAD send head
				head = next;
				curr = head;
			
			}
				
			i++;
			curr = curr->next;
		}
		//CREATE THREAD send head
		//JOIN THREAD
	}
}




/*
	There will be num_reduces of threads/processes running.
	Reduce will take all the key/value pairs of a unique word and sum it
	Example:
		out, 3
		out, 4
		out, 1
	Reduced:
		out, 8
*/
void reduce()
{
}

/*

dont need this at least according to my TA. He said they wont test us on the inputs


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
*/
int main (int argc, char* argv[]) {
	//validflags(argc, argv);
	//inputreader();
	
    int type; //[0] for wordcount [1] sort
    int impl; //[0] for procs [1] thread
    int maps; // num of maps
    int reduces; // num of reduces
    char inFile[250]; // input file
    char outFile[250]; // output file
    
    if(strcmp(argv[2],"wordcount") == 0){
        type = 0;
    }else{
        type = 1;
    }
    
    if(strcmp(argv[4],"procs") == 0){
        impl = 0;
    }else{
        impl = 1;
    }
    
    maps = atoi(argv[6]);
    reduces = atoi(argv[8]);
    strcpy(inFile, argv[10]);
    strcpy(outFile, argv[12]);
    
    node* head = inputreader(inFile,type);
    head = mapper(head,type,impl,maps);
    
    return 0;

}
