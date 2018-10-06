CCFLAGS = -ggdb -lm -pthread -g

all: map

map: map.c
	gcc $(CCFLAGS) map.c -lm -pthread

