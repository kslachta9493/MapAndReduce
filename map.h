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
} mapargs;
typedef struct kv
{
	char *key;
	int value;
	struct kv * next;
} kv;
#endif
