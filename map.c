
//Im just doing this the way my TA explained it. Which was very unintuitive.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
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
	node *head = NULL;
	node *next = NULL;
	void* memptr;
	memptr = temp->memptr;
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
	while (head->next != NULL)
	{
		
		//fflush(fp);
		sprintf(memptr, "%d\n", head->val);
		memptr += sizeof(int);
		head = head->next;
	}
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
			//printf("%s \n", next->word);
			p->next = next;
		}
		
		found = 0;
		curr = curr->next;
	}
	
	printf("Count is %d\n", count);
	mergeSort(&table, 0);
	void* memptr;
	memptr = temp->memptr;
	pthread_mutex_t lock = temp->lock;
	//pthread_mutex_lock(&lock);
	char str[5];
	head = table;
	while (table->next != NULL)
	{
		
		//fflush(fp);
		if (table->count > 9)
		{
			sprintf(memptr, "%s, %d\n", table->word, table->count);
			memptr += strlen(table->word) + (sizeof(char) * 5);
		}
		else
		{
			sprintf(memptr, "%s, %d\n", table->word, table->count);
			memptr += strlen(table->word) + (sizeof(char) * 4);
		}
		table = table->next;
	}
	while (head->next != NULL)
	{
		p = head;
		head = head->next;
		free(p);
	}
	//pthread_mutex_unlock(&lock);
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
	}
	fclose(fp);
	return head;
}
void *reduceWord(void * args)
{
	printf("REDUCIND THIS BITCH\n\n");
	redargs *temp = (redargs *) args;
	node* p;
	node** headRef;
	headRef = temp->head;
	node *head = *headRef;
	int i;
	int size = temp->size;
	p = head->next;
	printf("Starting at %s\n", head->word);
	while (head->next != NULL)
	{
		//printf("comparing %s and %s\n", p->word, head->word);
		if (strcmp(p->word, head->word) == 0)
		{
			//printf("combining %s and %s\n", p->word, head->word);
			head->count += p->count;
			head->next = p->next;
			p = p->next;
			size--;
		} else if (head->next != NULL)
		{
			head = head->next;
			p = p->next;
		}
	}
}
void *reduceInt()
{
}

/*
Mapper splits the code and either sends it into map threads or processes
*/
node* mapper(node* head, int wordcount, int procs,int maps, int reduces, FILE* outfile){
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

	//Shared memory vars
	int memsize = 0;
	int shm_fd;
	void* memptr;
	void* memtemp;

	pthread_t tid; 
	//processes
	if(procs == 0){
		int pids[maps];
		int pidsr[reduces];
		int ischild = -1;
		int shm_fd;
		shm_fd = shm_open("blocks", O_CREAT | O_RDWR, 0666);
		if (wordcount == 0)
			memsize = ((sizeof(char*) * total) + (sizeof(char) * 5));
		else
			memsize = sizeof(int) * total;
		ftruncate(shm_fd, memsize);
		memptr = mmap(0, memsize, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		memtemp = memptr;
		mapargs* ma = (mapargs *) malloc (sizeof(mapargs));
		for (i = 0; i < maps; i++)
		{
			printf("Created thread %d\n", i);
			ma->memptr = memptr + (size * i);
			pids[i] = fork();
			if (pids[i] == 0)
			{
				if (i > 0)
				{
					for (i = 0; i < size; i++)
					{
						curr = curr->next;
					}
				}
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
			shm_fd = shm_open("block", O_RDONLY, 0666);
			memptr = mmap(0, memsize, PROT_READ, MAP_SHARED, shm_fd, 0);
			int testmem = 0;
			memtemp = memptr;
			char input[memsize];
			char *splitint;
			char *splitword;
			node *p;
			curr = NULL;
			if (wordcount == 0)
			{
				while (memtemp < (memptr + memsize))
				{
					sprintf(input, "%s", (char*)memtemp);
					//printf("%s\n", input);
					i++;
			
					splitword = strtok(input, " ,");
					splitint = strtok(NULL, " \n");
					if (curr == NULL)
					{
						curr = (node*) malloc (sizeof(node));
						curr->word = strdup(splitword);
						curr->count = atoi(splitint);
						curr->next = NULL;
						p = curr;
					}
					while (splitword != NULL || splitint != NULL)
					{
						while (p->next != NULL)
						{
							p = p->next;
						}
				
						splitword = strtok(NULL, " ,\n");
						splitint = strtok(NULL, " ,\n");
						if (splitword == NULL || splitint == NULL)
						{
							printf("Null found %s %s\n", splitword, splitint);
							break;
						} else
						{
							//printf("%s, %d\n", splitword, atoi(splitint));
							next = (node*) malloc (sizeof(node));
							next->word = strdup(splitword);
							next->count = atoi(splitint);
							p->next = next;
						}
					}			
					memtemp = memtemp + (size * sizeof(char*));
				}
				head = curr;
				while (curr->next != NULL)
				{
					if (curr->word == NULL)
					{
						p->next = curr->next;
					}
					//fprintf(fpp, "%s, %d\n", curr->word, curr->count);
					//fflush(fpp);
					p = curr;
					curr = curr->next;
				}
				mergeSort(&head, 0);
			} else
			{
			
				while (memtemp < (memptr + memsize))
				{
					sprintf(input, "%s", (char*)memtemp);
					//printf("%s\n", input);
					i++;
			
					splitint = strtok(input, " \n");
					if (curr == NULL)
					{
						curr = (node*) malloc (sizeof(node));
						curr->val = atoi(splitint);
						curr->next = NULL;
						p = curr;
					}
					while (splitint != NULL)
					{
						while (p->next != NULL)
						{
							p = p->next;
						}
				
						splitint = strtok(NULL, " \n");
						if (splitint == NULL)
						{
							printf("Null found %s %s\n", splitword, splitint);
							break;
						} else
						{
							//printf("%s, %d\n", splitword, atoi(splitint));
							next = (node*) malloc (sizeof(node));
							next->val = atoi(splitint);
							p->next = next;
						}
					}
			
					memtemp = memtemp + (size * sizeof(char*));
				}
				head = curr;
				while (curr->next != NULL)
				{
					if (curr->word == NULL)
					{
						p->next = curr->next;
					}
					//fprintf(fpp, "%s, %d\n", curr->word, curr->count);
					//fflush(fpp);
					p = curr;
					curr = curr->next;
				}
				mergeSort(&head, 1);
			}
			currmap = 0;
			redargs *ra;
			ra = (redargs *) malloc (sizeof(redargs));
			int count = 0;
			p = head;
			while (p->next != NULL)
			{
				p = p->next;
				count++;
			}
			p = head;
			i = 0;
			int ischild = -1;
			while (currmap < reduces)
			{
				if(ceil(count)*(reduces-1)<total){
					size = ceil(count);
				}
				else{
					size = floor(count);
				}
				ra->size = count;
				if (currmap != 0)
				{
					for (i = 0; i < count / reduces; i++)
					{
						head = head->next;
					}
					ra->head = &head;
				} else
				{
					next = p;
					ra->head = &next;
				}

				printf("HEAD IS HERE %s\n", head->word);
				pidsr[i] = fork();
				if (pidsr[i] == 0)
				{
					ischild = 0;
					if (wordcount == 0)
					{
						reduceWord((void*) ra);
					}
					else
					{
						reduceInt((void*) ra);
					}
					break;
				}
				else
				{
					//am parent
					int j = 0;
					ischild = 1;
					for (j = 0; j < reduces; j++)
					{
						int test = waitpid(pidsr[j], NULL, WUNTRACED);
					}
				}
				i++;
				currmap++;
			}
			if (wordcount == 0 && ischild == 1)
			{
				while (p->next != NULL)
				{

					fprintf(outfile, "%s %d\n", p->word, p->count);
					fflush(outfile);
					p = p->next;
				}
				fclose(outfile);
			} else if (wordcount == 1 && ischild == 1)
			{
				while (p->next != NULL)
				{

					fprintf(outfile, "%d\n", p->val);
					fflush(outfile);
					p = p->next;
				}
				fclose(outfile);
			}
		}

	}
	//threads
	else{
		pthread_t *threads;
		pthread_t *threadsred;
		threads = (pthread_t *) malloc (maps * sizeof(pthread_t));
		threadsred = (pthread_t *) malloc (reduces * sizeof(pthread_t));
		int i;
		mapargs* ma = (mapargs *) malloc (sizeof(mapargs) * (maps + reduces));
		shm_fd = shm_open("block", O_CREAT | O_RDWR, 0666);
		if (wordcount == 0)
			memsize = ((sizeof(char*) * total) + (sizeof(char) * 5));
		else
			memsize = sizeof(int) * total;
		ftruncate(shm_fd, memsize);
		memptr = mmap(0, memsize, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		void* memtemp;
		memtemp = memptr;
		pthread_mutex_t lock;
		pthread_mutex_init(&lock, NULL);
			curr = head;
		while (currmap < maps)
		{
			printf("About to do thread create\n");
			if (currmap != 0)
			{
				for (i = 0; i < size; i++)
				{
					curr = curr->next;
				}
			}

			(ma + currmap)->size = size;
			(ma + currmap)->head = curr;
			(ma + currmap)->memptr = memtemp;
			(ma + currmap)->lock = lock;
			memtemp = memtemp + (size * sizeof(char*));
			if (wordcount == 0)
			{
				printf("first word %s\n", (ma+currmap)->head->word);
				pthread_create(&threads[currmap], NULL, mapword, (void *) (ma + currmap));
			}
			else
			{
				pthread_create(&threads[currmap], NULL, mapint, (void *) (ma + currmap));
			}
				

			currmap++;
		}
		//wait for threads
		for (i = 0; i < maps; i++)
		{
			pthread_join(threads[i], NULL);
		}
		shm_fd = shm_open("block", O_RDONLY, 0666);
		memptr = mmap(0, memsize, PROT_READ, MAP_SHARED, shm_fd, 0);
		int testmem = 0;
		memtemp = memptr;
		FILE* fpp;
		fpp = fopen("out.txt", "w");
		char input[memsize];
		char* splitword;
		char* splitint;
		curr = NULL;
		node* p;
		i = 0;
		int j = 0;
		int length = 0;
		if (wordcount == 0)
		{
			while (memtemp < (memptr + memsize))
			{
				sprintf(input, "%s", (char*)memtemp);
				//printf("%s\n", input);
				i++;
			
				splitword = strtok(input, " ,");
				splitint = strtok(NULL, " \n");
				if (curr == NULL)
				{
					curr = (node*) malloc (sizeof(node));
					curr->word = strdup(splitword);
					curr->count = atoi(splitint);
					curr->next = NULL;
					p = curr;
				}
				while (splitword != NULL || splitint != NULL)
				{
					while (p->next != NULL)
					{
						p = p->next;
					}
				
					splitword = strtok(NULL, " ,\n");
					splitint = strtok(NULL, " ,\n");
					if (j < 4)
					{
						printf("%s, %d %d\n", splitword, atoi(splitint), j);
					}
					j++;
					if (splitword == NULL || splitint == NULL)
					{
						printf("Null found %s %s\n", splitword, splitint);
						break;
					} else
					{
						//printf("%s, %d\n", splitword, atoi(splitint));
						next = (node*) malloc (sizeof(node));
						next->word = strdup(splitword);
						next->count = atoi(splitint);
						p->next = next;
					}
				}
				printf("%d %d %d\n", memtemp, memsize, memptr);
			
				memtemp = memtemp + (size * sizeof(char*));
				//fprintf(fpp, "%s", input);
				fflush(fpp);
			}
			head = curr;
			while (curr->next != NULL)
			{
				if (curr->word == NULL)
				{
					p->next = curr->next;
				}
				//fprintf(fpp, "%s, %d\n", curr->word, curr->count);
				//fflush(fpp);
				p = curr;
				curr = curr->next;
			}
			mergeSort(&head, 0);
		}
		else
		{
			
			while (memtemp < (memptr + memsize))
			{
				sprintf(input, "%s", (char*)memtemp);
				//printf("%s\n", input);
				i++;
			
				splitint = strtok(input, " \n");
				if (curr == NULL)
				{
					curr = (node*) malloc (sizeof(node));
					curr->val = atoi(splitint);
					curr->next = NULL;
					p = curr;
				}
				while (splitint != NULL)
				{
					while (p->next != NULL)
					{
						p = p->next;
					}
				
					splitint = strtok(NULL, " \n");
					if (splitint == NULL)
					{
						printf("Null found %s %s\n", splitword, splitint);
						break;
					} else
					{
						//printf("%s, %d\n", splitword, atoi(splitint));
						next = (node*) malloc (sizeof(node));
						next->val = atoi(splitint);
						p->next = next;
					}
				}
			
				memtemp = memtemp + (size * sizeof(char*));
			}
			head = curr;
			while (curr->next != NULL)
			{
				if (curr->word == NULL)
				{
					p->next = curr->next;
				}
				//fprintf(fpp, "%s, %d\n", curr->word, curr->count);
				//fflush(fpp);
				p = curr;
				curr = curr->next;
			}
			mergeSort(&head, 1);
			
		}

		/*
		while (head->next != NULL)
		{

			fprintf(fpp, "%s, %d\n", head->word, head->count);
			fflush(fpp);
			head = head->next;
		}
		*/
		
		currmap = 0;
		redargs *ra;
		ra = (redargs *) malloc (sizeof(redargs) * reduces);
		int count = 0;
		p = head;
		while (p->next != NULL)
		{
			p = p->next;
			count++;
		}
		p = head;
		while (currmap < reduces)
		{
			if(ceil(count)*(reduces-1)<total){
				size = ceil(count);
			}
			else{
				size = floor(count);
			}
			(ra + currmap)->size = count;
			if (currmap != 0)
			{
				for (i = 0; i < count / reduces; i++)
				{
					head = head->next;
				}
				(ra + currmap)->head = &head;
			} else
			{
				next = p;
				(ra + currmap)->head = &next;
			}

			printf("HEAD IS HERE %s\n", head->word);
			if (wordcount == 0)
				pthread_create(&threadsred[currmap], NULL, reduceWord, (void *) (ra + currmap));
			else
				pthread_create(&threadsred[currmap], NULL, reduceInt, NULL);

			currmap++;
		}
		for (i = 0; i < reduces; i++)
		{
			pthread_join(threadsred[i], NULL);
		}
		if (wordcount == 0)
		{
			while (p->next != NULL)
			{

				fprintf(outfile, "%s %d\n", p->word, p->count);
				fflush(outfile);
				p = p->next;
			}
		} else
		{
			while (p->next != NULL)
			{

				fprintf(outfile, "%d\n", p->val);
				fflush(outfile);
				p = p->next;
			}
		}
		fclose(outfile);
		
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
	FILE* fp;
	fp = fopen(outFile, "w");
    head = mapper(head,type,impl,maps,reduces, fp);
    return 0;

}
