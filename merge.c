/* ----------------------------------------------------------- */
/* NAME : John Mortimore                     User ID: jgmortim */
/* DUE DATE : 03/01/2019                                       */
/* PROGRAM ASSIGNMENT 2                                        */
/* FILE NAME : merge.c                                         */
/* PROGRAM PURPOSE :                                           */
/*    Performs binary merge with int arrays stored in shared   */
/*    memory.                                                  */
/* ----------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>

int binarySearch(int* a, int value, int lo, int hi);
void mergeX(int index);
void mergeY(int index);

int shmIDX, shmIDY, shmIDZ;
char output[1024];
int m, n;
int *x, *y, *z;

/* ----------------------------------------------------------- */
/* FUNCTION : main                                             */
/*    The main method for the program. Creates the child       */
/*    processes to sort/merge the arrays.                      */
/* PARAMETER USAGE :                                           */
/*    argc {int} - number of argument.                         */
/*    argv {char**} - list of arguments.                       */
/* FUNCTIONS CALLED :                                          */
/*    atoi(), fork(), getpid(), mergeX(), mergeY(), shmat(),   */
/*    shmdt(), sprintf(), strlen(), wait()                     */
/* ----------------------------------------------------------- */
int main(int argc, char** argv) {
	int i;
	/* Read in the argument. */
	m = atoi(argv[1]);
	n = atoi(argv[2]);
	shmIDX = atoi(argv[3]);
	shmIDY = atoi(argv[4]);
	shmIDZ = atoi(argv[5]);
	/* attach the shared memory. */
	x = shmat(shmIDX, NULL, 0);
	y = shmat(shmIDY, NULL, 0);
	z = shmat(shmIDZ, NULL, 0);
	/* Create processes to handle x[]. */
	for(i=0; i < m; i++) {
		if (fork()==0) {
			sprintf(output, "      $$$ M-PROC(%d): handling x[%d] = %d", getpid(), i, x[i]);
			sprintf(output+strlen(output), "\n          ..........\n");
			write(1, output, strlen(output));
			mergeX(i);
		}
	}
	/* Create processes to handle y[]. */
	for(i=0; i < n; i++) {
		if (fork()==0) {
			sprintf(output, "      $$$ M-PROC(%d): handling y[%d] = %d", getpid(), i, y[i]);
			sprintf(output+strlen(output), "\n          ..........\n");
			write(1, output, strlen(output));
			mergeY(i);
		}
	}
	/* Wait for children to finish, detach shared memory, and exit.*/
	while(wait(NULL)>0);
	shmdt(x);
	shmdt(y);
	shmdt(z);
	return 0;
}

/* ----------------------------------------------------------- */
/* FUNCTION : mergeX                                           */
/*    Inserts the elements of x[] into the correct position    */
/*    in the output array.                                     */
/* PARAMETER USAGE :                                           */
/*    index {int} - the index of the element in x[] to handle. */
/* FUNCTIONS CALLED :                                          */
/*    binarySearch(), exit(), getpid(), sprintf(), strlen(),   */
/*    write()                                                  */
/* ----------------------------------------------------------- */
void mergeX(int index) {
	int finalIndex, bin;
	/* Perform the binary search. */
	bin = binarySearch(y, x[index], 0, n);
	sprintf(output, "      $$$ M-PROC(%d): x[%d] = %d is found", getpid(), index, x[index]);
	/* Print location. */
	if (bin > 0 && bin < n) {
		sprintf(output+strlen(output), " between y[%d] = %d and y[%d] = %d", bin-1, y[bin-1], bin, y[bin]);
	} else if (bin == 0) {
		sprintf(output+strlen(output), " to be smaller than y[0] = %d", y[0]);
	} else {
		sprintf(output+strlen(output), " to be larger than y[%d] = %d", n-1, y[n-1]);
	}
	sprintf(output+strlen(output), "\n          ..........\n");
	write(1, output, strlen(output));
	/* Insert into the output array, print relevant info, and exit. */
	finalIndex=index + bin;
	sprintf(output, "      $$$ M-PROC(%d): about to write x[%d] = %d into position %d of the output array", getpid(), index, x[index], finalIndex);
	sprintf(output+strlen(output), "\n          ..........\n");
	write(1, output, strlen(output));
	z[finalIndex] = x[index];
	exit(0);
}

/* ----------------------------------------------------------- */
/* FUNCTION : mergeY                                           */
/*    Inserts the elements of y[] into the correct position    */
/*    in the output array.                                     */
/* PARAMETER USAGE :                                           */
/*    index {int} - the index of the element in y[] to handle. */
/* FUNCTIONS CALLED :                                          */
/*    binarySearch(), exit(), getpid(), sprintf(), strlen(),   */
/*    write()                                                  */
/* ----------------------------------------------------------- */
void mergeY(int index) {
	int finalIndex, bin;
	/* Perform the binary search. */
	bin = binarySearch(x, y[index], 0, m);
	sprintf(output, "      $$$ M-PROC(%d): y[%d] = %d is found", getpid(), index, y[index]);
	/* Print location. */
	if (bin > 0 && bin < m) {
		sprintf(output+strlen(output), " between x[%d] = %d and x[%d] = %d", bin-1, x[bin-1], bin, x[bin]);
	} else if (bin == 0) {
		sprintf(output+strlen(output), " to be smaller than x[0] = %d", x[0]);
	} else {
		sprintf(output+strlen(output), " to be larger than x[%d] = %d", m-1, x[m-1]);
	}
	sprintf(output+strlen(output), "\n          ..........\n");
	write(1, output, strlen(output));
	/* Insert into the output array, print relevant info, and exit. */
	finalIndex=index + bin;
	sprintf(output, "      $$$ M-PROC(%d): about to write y[%d] = %d into position %d of the output array", getpid(), index, y[index], finalIndex);
	sprintf(output+strlen(output), "\n          ..........\n");
	write(1, output, strlen(output));
	z[finalIndex] = y[index];
	exit(0);
}

/* ----------------------------------------------------------- */
/* FUNCTION : binarySearch                                     */
/*    A modified binary search to find the correct location    */
/*    to insert an element in a[lo..hi].                       */
/* PARAMETER USAGE :                                           */
/*    a {int*} - the array to search through.                  */
/*    value {int} - the value to search for.                   */
/*    lo {int} - the lower bound for the search.               */
/*    hi {int} - the upper bound for the search.               */
/* FUNCTIONS CALLED :                                          */
/*                                                             */
/* ----------------------------------------------------------- */
int binarySearch(int* a, int value, int lo, int hi) {
	int mid;
	while (lo < hi) {
		mid = (lo+hi)/2;
		if (value > a[mid]) {
			lo = mid+1;
		} else {
			hi = mid;
		}
	}
	return lo;
}

