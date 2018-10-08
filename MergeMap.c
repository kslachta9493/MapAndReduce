
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
void swap(node* a, node* b) 
{ 
    int temp = a->val; 
    a->val = b->val; 
    b->val = temp; 
} 


node* mergeSort(node*head,int length){
	if(head == NULL){
		//do we throw exception?
		return;
	}
	//nothing to sort so just returns
	if(length == 1){
		return head;
	}
	
	//splits list into 2
	int count = length/2;
	int i = 0;
	node* ptr = head;
	node* prev;
	while(i < count){
		prev = ptr;
		ptr = ptr->next;
		i++;
	}
	prev->next = NULL;
	
	//sorts both halves of function
	node* llFirst =  mergeSort(ptr, length-count); 
	node* llSecond = mergeSort(head,count);
	
	node* temp = NULL;
	node* end = NULL;
	
	while(llFirst!=NULL && llSecond!=NULL){
		//will sort in ascending order. Equal values is resolved by putting whichever value was read in first, first in the list.
		if (llFirst->val > llSecond->val){
			if(temp == NULL){
				temp = llSecond;
				end = temp;
			}
			else{
				end->next = llSecond;
				end = end->next;
			}
			llSecond = llSecond->next;
			//end.next = NULL; not necessary. 
		}
		else{
			if(temp == NULL){
				temp = llFirst;
				end = temp;
			}
			else{
				end->next = llFirst;
				end = end->next;
			}
			llFirst = llFirst->next;
			//end.next = NULL; not necessary.
		}
		
	}
	//puts in the remaining values
	while(llFirst != NULL){
			end->next = llFirst;
			end = end->next;
			llFirst = llFirst->next;
	}
	while(llSecond != NULL){
			end->next = llSecond;
			end = end->next;
			llSecond = llSecond->next;
	}

	return temp;
}
/*
void bubblesort(node* head, int size) 
{ 
    int swapped, i; 
    node* temp; 
    node* tail = NULL; 
  
  if(head ->next = NULL){
  	return head;
  }
  else{
  	temp = head;
  	i = 0;
  	while(i<size/2)
  }
    do
    { 
        swapped = 0; 
        temp = head; 
  
        while (temp->next != tail) 
        { 
            if (temp->val > temp->next->val) 
            {  
                swap(temp, temp->next); 
                swapped = 1; 
            } 
            temp = temp->next; 
        } 
        tail = temp; 
    } 
    while (swapped); 
} 
*/
void *mapint(void *args)
{
	//must sort the list on integers it is given
	mapargs *temp = (mapargs *) args;
	int size = 0;
	int type = 0;
	node* curr;
	size = temp->size;
	curr = temp->head;
	FILE* fp;
	fp = temp->fp;
	//bubblesort(curr, size);
	mergeSort(curr,size);
	int i = 0;	
	for (i = 0; i < size; i++)
	{
		fprintf(fp, "%d \n", curr->val);
		curr = curr->next;
		fflush(fp);
	}
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
	kv* table;
	int i = 0;
	int j = 0;
	table = (kv *) malloc (sizeof(kv) * size);
	for (i = 0; i < size; i++)
	{
		(table + i)->value = 1;
	}
	if (curr == NULL)
		printf("NO HEAD\n");
	int count = 0;
	int found = 0;
	printf("Size is %d\n", size);
	for (i = 0; i < size; i++)
	{

		for (j = 0; j < count; j++)
		{
			if (strcmp((table + j)->key, curr->word) == 0)
			{
				found = 1;
				(table + j)->value++;
			}
		}
		if (found == 0)
		{
			(table + count)->key = strdup(curr->word);
			(table + count)->value = 1;
			count++;
		}
		found = 0;
		curr = curr->next;
	}
	printf("Count is %d\n", count);
	for (i = 0; i < count; i++)
	{
			fprintf(fp, "%s, %d \n", (table + i)->key, (table + i)->value);
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
		while(fscanf(fp,"%s",word)==1){
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
	int pid;
	int pid_t = pid;
	FILE* fd;
	FILE* fp;
	fd = fopen("123.txt", "w");
	fp = fopen("321.txt", "w");
	pthread_t tid; 
	//processes
	if(procs == 0){
		int pids[maps];
		int ischild = -1;
		mapargs* ma = (mapargs *) malloc (sizeof(mapargs));
		for (i = 0; i < maps; i++)
		{
			printf("Created thread %d\n", i);
			pids[i] = fork();
			if (pids[i] == 0)
			{
				if (i != 0)
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
		/*
		while(curr != NULL)	{
			if(currmap == maps){
				break;
			}
			if(size%i==0){
				next = curr->next;
				curr->next = NULL;
				pid = fork();
				if(pid == 0){
					//map(head,wordcount);
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
			//map(head,wordcount);
		}
		*/
		//JOIN PROCESSES
	}
	//threads
	else{
		pthread_t *threads;
		threads = (pthread_t *) malloc (maps * sizeof(pthread_t));
		/*
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
		*/
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
