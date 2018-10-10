CCFLAGS = -ggdb -lm -pthread -g -lrt

all: map

map: map.c
	gcc $(CCFLAGS) map.c -lm -pthread

