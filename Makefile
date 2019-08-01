CC = gcc
FLAGS = 

all: clean main merge qsort

main: main.c
	$(CC) $(FLAGS) main.c -o main

merge:
	$(CC) $(FLAGS) merge.c -o merge

qsort:
	$(CC) $(FLAGS) qsort.c -o qsort

.PHONY: clean

clean:
	rm -f main merge qsort *.o
