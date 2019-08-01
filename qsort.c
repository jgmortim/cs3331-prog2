/* ----------------------------------------------------------- */
/* NAME : John Mortimore                     User ID: jgmortim */
/* DUE DATE : 03/01/2019                                       */
/* PROGRAM ASSIGNMENT 2                                        */
/* FILE NAME : qsort.c                                         */
/* PROGRAM PURPOSE :                                           */
/*    Performs quick sort with an int array stored in shared   */
/*    memory.                                                  */
/* ----------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void quicksort(int lo, int hi);
int partition(int lo, int hi);
void swap(int a, int b);

int size;
int shmID;
int* a;
char output[1024];

/* ----------------------------------------------------------- */
/* FUNCTION : main                                             */
/*    The main method for the program.                         */
/* PARAMETER USAGE :                                           */
/*    argc {int} - number of argument.                         */
/*    argv {char**} - list of arguments.                       */
/* FUNCTIONS CALLED :                                          */
/*    atoi(), getpid(), quicksort(), shmat(), shmdt(),         */
/*    sprintf(), strlen(), write()                             */
/* ----------------------------------------------------------- */
int main(int argc, char** argv) {
	int left, right;
	/* Read in the arguments. */
	left = atoi(argv[1]);
	right = atoi(argv[2]);
	size = atoi(argv[3]);
	shmID = atoi(argv[4]);
	/* Attach the shared memory. */
	a = shmat(shmID, NULL, 0);
	/* Begin the sort. */
	quicksort(left, right);
	/* Detach the shared memory. Print that the process is finished and exit. */
	shmdt(a);
	sprintf(output, "   ### Q-PROC(%d): exits", getpid());
	sprintf(output+strlen(output), "\n       ..........\n");
	write(1, output, strlen(output));
	return 0;
}

/* ----------------------------------------------------------- */
/* FUNCTION : child                                            */
/*    Creates a child process to work on a subproblem of       */
/*    quick sort.                                              */
/* PARAMETER USAGE :                                           */
/*    lo {int} - the lower bound of the subproblem.            */
/*    hi {int} - the upper bound of the subproblem.            */
/* FUNCTIONS CALLED :                                          */
/*    execvp(), fork(), free(), getpid(), malloc(), sizeof(),  */
/*    sprintf(), strlen(), wait(), write()                     */
/* ----------------------------------------------------------- */
void child( int lo, int hi) {
	int i;
	/* Allocate some memory for the argument list. */
	char **arg=malloc(sizeof(char*)*6);
	for(i=0; i<5; i++) {
		arg[i]=malloc(sizeof(char)*10);
	}
	/* Set the arguments. */
	sprintf(arg[0], "qsort");
	sprintf(arg[1], "%d", lo);
	sprintf(arg[2], "%d", hi);
	sprintf(arg[3], "%d", size);
	sprintf(arg[4], "%d", shmID);
	arg[5]=NULL;
	/* Create the child and print the relevant info. */
	if (fork()==0) {
		sprintf(output, "   ### Q-PROC(%d): entering with a[%d..%d]\n       ", getpid(), lo, hi);
		for (i=lo; i<=hi; i++) {
			sprintf(output+strlen(output), "%d  ", a[i]);
		}
		sprintf(output+strlen(output), "\n       ..........\n");
		write(1, output, strlen(output));
		execvp("./qsort", arg);
	}
	/* Free allocated memory and return. */
	for(i=0; i<5; i++) {
		free(arg[i]);
	}
	free(arg);
	return;
}


/* ----------------------------------------------------------- */
/* FUNCTION : quicksort                                        */
/*    An implementation of quick sort. Sorts a[lo..hi]         */
/* PARAMETER USAGE :                                           */
/*    lo {int} - the lower bound for the sort.                 */
/*    hi {int} - the upper bound for the sort.                 */
/* FUNCTIONS CALLED :                                          */
/*    child(), getpid(), partition(), sprintf(), strlen(),     */
/*    write()                                                  */
/* ----------------------------------------------------------- */
void quicksort(int lo, int hi) {
	int i, p = 0;
	if (lo < hi) {
		/* Partition the array and print the relevant info. */
		p = partition(lo, hi);
		sprintf(output, "   ### Q-PROC(%d): pivot element is a[%d] = %d", getpid(), p, a[p]);
		sprintf(output+strlen(output), "\n       ..........\n");
		write(1, output, strlen(output));
		/* Create children to handle the subproblems. */
		if (lo < p-1) child(lo, p-1);
		if (p+1 < hi) child(p+1, hi);
		/* Wait for the children to finish, print the relevant info, and return. */
		while(wait(NULL)>0);
		sprintf(output, "   ### Q-PROC(%d): section a[%d..%d] sorted\n       ", getpid(), lo, hi);
		for (i=lo; i<=hi; i++) {
			sprintf(output+strlen(output), "%d  ", a[i]);
		}
		sprintf(output+strlen(output), "\n       ..........\n");
		write(1, output, strlen(output));
	}
	return;
}

/* ----------------------------------------------------------- */
/* FUNCTION : partition                                        */
/*    Used by quicksort() to partition a[lo..hi]               */
/* PARAMETER USAGE :                                           */
/*    lo {int} - the lower bound for the partition.            */
/*    hi {int} - the upper bound for the partition.            */
/* FUNCTIONS CALLED :                                          */
/*    swap()                                                   */
/* ----------------------------------------------------------- */
int partition(int lo, int hi) {
	int pivot = a[hi];
	int j, i = lo;
	for (j = lo; j < hi; j++) {
		if (a[j] < pivot) {
			swap(i, j);
			i++;
		}
	}
	swap(i, hi);
	return i;
}

/* ----------------------------------------------------------- */
/* FUNCTION : swap                                             */
/*    Used by quicksort() to swap a[i] with a[j].              */
/* PARAMETER USAGE :                                           */
/*    i {int} - An index of an element to swap.                */
/*    j {int} - An index of an element to swap.                */
/* FUNCTIONS CALLED :                                          */
/*                                                             */
/* ----------------------------------------------------------- */
void swap(int i, int j) {
	int temp = a[i];
	a[i] = a[j];
	a[j] = temp;
	return;
}
