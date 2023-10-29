CC = gcc
CFLAGS = -g -Wall -lm -std=c99
 
build: graph.c
	$(CC) $(CFLAGS) -o graph graph.c

run: graph
	./graph

.PHONY : clean
clean :
	rm graph
