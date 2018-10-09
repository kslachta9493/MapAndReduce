
//Im just doing this the way my TA explained it. Which was very unintuitive.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"

int app = 0;
int impl = 0;
int num_maps = 2;
int num_reduces = 2;
char* inputfile = NULL;
char* outfile = NULL;
int total= 0;

/*
	Map will be called once per pthread/process where # of pthreads/processes is equal to num_maps
	Map for wordcount will generate a key/value pair where key = unique word and value = # of times it is seen
	Map will put its list of key/value pairs into shared memory for reduces threads/processes to use
*/

/*
	Merge sort implementation from geeksforgeeks.com
*/
void split(node* source, node** frontRef, node** backRef)
{
	node* fast;
	node* slow;

	slow = source;
	fast = source->next;
	
	while (fast != NULL)
	{
		fast = fast->next;
		if (fast != NULL)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}

	*frontRef = source;
	*backRef = slow->next;
	slow->next = NULL;
}
node* merge(node *a, node* b, int type)
{
	node *result = NULL;
	if (a == NULL)
		return b;
	else if (b == NULL)
		return a;
	if (type == 0)
	{
		if (strcmp(a->word, b->word) <= 0)
		{
			result = a;
			result->next = merge(a->next, b, type);
		}
		else
		{
			result = b;
			result->next = merge(a, b->next, type);
		}
	}
	else
	{
		if (a->val <= b->val)
		{
			result = a;
			result->next = merge(a->next, b, type);
		}
		else
		{
			result = b;
			result->next = merge(a, b->next, type);
		}
	}
	return result;
}
void mergeSort(node** headRef, int type)
{
	node *head = *headRef;
	node* l;
	node* r;
	if (head == NULL || head->next == NULL)
	{
		return;
	}
	split(head, &l, &r);
	mergeSort(&l, type);
	mergeSort(&r, type);
	*headRef = merge(l,r, type);
}

void *mapint(void *args)
{
	//must sort the list on integers it is given
	mapargs *temp = (mapargs *) args;
	int i;
	int size = 0;
	int type = 0;
	node* curr;
	size = temp->size;
	curr = temp->head;
	FILE* fp;
	fp = temp->fp;
	node *head = NULL;
	node *next = NULL;
	node *p = NULL;
	for (i = 0; i < size; i++)
	{
		if (head == NULL)
		{
			head = (node *) malloc (sizeof(node));
			head->val = curr->val;
			head->next = NULL;
		}
		else
		{
			p = head;
			while (p->next != NULL)
			{
				p = p->next;
			}
			next = (node *) malloc (sizeof(node));
			next->next = NULL;
			next->val = curr->val;
			p->next = next;
		}
		curr = curr->next;
	}
	mergeSort(&head, 1);
	for (i = 0; i < size; i++)
	{
		fprintf(fp, "%d \n", head->val);
		head = head->next;
		fflush(fp);
	}
	fclose(fp);
}
void tester(node *head)
{
    int swapped, i; 
    node* temp; 
    node* tail = NULL; 
    do
    { 
        swapped = 0; 
        temp = head; 
  
        while (temp->next != tail) 
        { 
            if (temp->val == temp->next->val) 
            {  
                //swap(temp, temp->next); 
		printf("Duplicate found %d\n", temp->val);
                swapped = 1; 
            } 
            temp = temp->next; 
        } 
        tail = temp; 
    } 
    while (swapped); 
	
}
void *mapword(void *args)
{
	mapargs *temp = (mapargs *) args;
	int size = 0;
	int type = 0;
	node* curr;
	size = temp->size;
	curr = temp->head;
	FILE* fp;
	fp = temp->fp;
	node* table;
	int i = 0;
	int j = 0;
	//table = (node *) malloc (sizeof(node));
	node *head = NULL;
	node *next = NULL;
	node *p = NULL;
	for (i = 0; i < size; i++)
	{
		if (head == NULL)
		{
			head = (node *) malloc (sizeof(node));
			head->word = strdup(curr->word);
			head->next = NULL;
		}
		else
		{
			p = head;
			while (p->next != NULL)
			{
				p = p->next;
			}
			next = (node *) malloc (sizeof(node));
			next->next = NULL;
			next->word = strdup(curr->word);
			p->next = next;
		}
		curr = curr->next;
	}
	curr = head;
	if (curr == NULL)
		printf("NO HEAD\n");
	int count = 0;
	int found = 0;
	printf("Size is %d\n", size);
	next = (node *) malloc(sizeof(node));
	next->next = NULL;
	next->word = curr->word;
	next->count = 1;
	table = next;
	curr = curr->next;
	while (curr->next != NULL)
	{
		p = table;
		while (p->next != NULL)
		{
			if (strcmp(p->word, curr->word) == 0)
			{
				found = 1;
				p->count++;
			}
			p = p->next;
		}
		
		if (found == 0)
		{
			next = (node *) malloc(sizeof(node));
			next->next = NULL;
			next->word = curr->word;
			next->count = 1;
			printf("%s \n", next->word);
			p->next = next;
		}
		
		found = 0;
		curr = curr->next;
	}
	
	printf("Count is %d\n", count);
	mergeSort(&table, 0);
	while (table->next != NULL)
	{
		fprintf(fp, "%s, %d \n", table->word, table->count);
		table = table->next;
		fflush(fp);
	}
	
	fclose(fp);
}

/*
	Inputreader should take the inputfile and break it into similar size chunks for use in the parallel map functions
*/
//" .,;:!-"
node* inputreader(char* filename, int wordcount)
{
	FILE* fp = fopen(filename, "r");
	char word[250];
	char *temp;
	// counting words
	node* head = NULL;
	node* next = NULL;
	node* p = NULL;
	int val = 0;
	if(wordcount == 0){
		int i = 0;
		while(fscanf(fp,"%s",word)==1){
			for (i = 0; i < strlen(word); i++)
			{
				word[i] = tolower(word[i]);
			}
			temp = strtok(word, " .,;:!-");
			if(head == NULL){
				head = (node*)malloc(sizeof(node));
				head->word = strdup(temp);
			}
			else{
				p = head;
				while (p->next != NULL)
				{
					p = p->next;
				}
				next = (node*)malloc(sizeof(node));
				next->word = strdup(temp);
				p->next = next;
			}
			total++;
		}
	}
	//not counting words
	else{
		while(fscanf(fp,"%d",&val)==1){
			
			if(head == NULL){
				head = (node*)malloc(sizeof(node));
				head->val = val;
			}
			else{
				p = head;
				while (p->next != NULL)
				{
					p = p->next;
				}
				next = (node*)malloc(sizeof(node));
				next->val = val;
				p->next = next;
			}
			total++;
		}
			/*
			FILE* fd;
			fd = fopen("out.txt", "w");
			node* temp = head;
			while (temp->next != NULL)
			{
				fprintf(fd, "%d\n", temp->val);
				temp = temp->next;
				fflush(fp);
			}
			fclose(fd);
			*/
	}
	fclose(fp);
	return head;
}
/*
Mapper splits the code and either sends it into map threads or processes
*/
node* mapper(node* head, int wordcount, int procs,int maps, int reduces){
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
	int currmap = 0;
	FILE* fd;
	FILE* fp;
	fd = fopen("123.txt", "w");
	fp = fopen("321.txt", "w");
	pthread_t tid; 
	//processes
	if(procs == 0){
		int pids[maps];
		int ischild = -1;
		for (i = 0; i < maps; i++)
		{
			printf("Created thread %d\n", i);
			pids[i] = fork();
			mapargs* ma = (mapargs *) malloc (sizeof(mapargs));
			if (pids[i] == 0)
			{
				if (i > 0)
				{
					for (i = 0; i < size; i++)
					{
						curr = curr->next;
					}
					ma->fp = fp;
				}
				else
				{
					ma->fp = fd;
				}
					//ischild = pids[i];
				ma->size = size;
				ma->head = curr;
				ischild = 1;
				if (wordcount == 0)
					mapword((void *) ma);
				else
					mapint((void *) ma);
				break;
			}
			else
			{
				ischild = 0;
			}
		}
		if (ischild == 0)
		{
			printf("wait started\n");
			for (i = 0; i < maps; i++)
			{
				int test = waitpid(pids[i], NULL, WUNTRACED);
			}
			printf("wait ended\n");
		}

	}
	//threads
	else{
		pthread_t *threads;
		threads = (pthread_t *) malloc (maps * sizeof(pthread_t));
		int i;
		mapargs* ma = (mapargs *) malloc (sizeof(mapargs) * maps);
		while (currmap < maps)
		{
			printf("About to do thread create\n");
			curr = head;
			if (currmap != 0)
			{
				for (i = 0; i < size; i++)
				{
					curr = curr->next;
				}
				(ma + currmap)->fp = fp;
			}
			else
			{
				(ma + currmap)->fp = fd;
			}
			(ma + currmap)->size = size;
			(ma + currmap)->head = curr;
			if (wordcount == 0)
				pthread_create(&threads[currmap], NULL, mapword, (void *) (ma + currmap));
			else
				pthread_create(&threads[currmap], NULL, mapint, (void *) (ma + currmap));
				

			currmap++;
		}
		//wait for threads
		for (i = 0; i < maps; i++)
		{
			pthread_join(threads[i], NULL);
		}
		/*
		currmaps = 0
		while (currmaps < reduces)
		{
			if (type == 0)
				pthread_create(&threads[currmap], NULL, reduceword, (void *) (ma + currmap));
			else
				pthread_create(&threads[currmap], NULL, reduceint, (void *) (ma + currmap));
		}
		*/
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
void print(node *head)
{
	FILE* fd = fopen("test.txt", "w");
	node* curr = head;
	if (curr == NULL)
		printf("NO HEAD\n");
	
	while (curr != NULL)
	{
		fprintf(fd, "%s \n", curr->word);
		curr = curr->next;
	}
	fflush(fd);
	fclose(fd);
	
}
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

    head = mapper(head,type,impl,maps,reduces);
    return 0;

}
