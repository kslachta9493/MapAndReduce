#ifndef MAP_H
#define MAP_H

typedef struct node {
    int val;
    char* word;
    int count;
    struct node * next;
}node;
typedef struct mapargs
{
	int size;
	node* head;
	FILE* fp;
	void* memptr;
	pthread_mutex_t lock;
} mapargs;
typedef struct redargs
{
	int size;
	node** head;
} redargs;
typedef struct kv
{
	char *key;
	int value;
	struct kv * next;
} kv;
#endif
