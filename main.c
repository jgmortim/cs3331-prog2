/* ----------------------------------------------------------- */
/* NAME : John Mortimore                     User ID: jgmortim */
/* DUE DATE : 03/01/2019                                       */
/* PROGRAM ASSIGNMENT 2                                        */
/* FILE NAME : main.c                                          */
/* PROGRAM PURPOSE :                                           */
/*    Reads three integer arrays from stdin. The first array   */
/*    is sorted via quick sort. The the other two are merged   */
/*    using binary merge.                                      */
/* ----------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

void cleanup();
void setupSHM();
void qsortChild();
void mergeChild();

int k, m, n, i;
int *a, *x, *y, *z;	
key_t keyA, keyX, keyY, keyZ;
int shmIDA, shmIDX, shmIDY, shmIDZ;
char output[1024];
char **argQ, **argM;

/* ----------------------------------------------------------- */
/* FUNCTION : main                                             */
/*    The main method for the program.                         */
/* PARAMETER USAGE :                                           */
/*    N/A                                                      */
/* FUNCTIONS CALLED :                                          */
/*    cleanup(), mergeChild(), setupSHM(), sprintf(),          */
/*    strlen(), qsortChild(), wait(), write()                  */
/* ----------------------------------------------------------- */
int main() {
	sprintf(output, "Quicksort and Binary Merge with Multiple Processes:\n\n");
	write(1, output, strlen(output));
	/* Setup shared memory. */
	setupSHM();

	/* Print a[]. */
	sprintf(output, "\n*** MAIN: Input array for qsort has %d elements:\n    ", k);
	for(i=0; i<k; i++) {
		sprintf(output + strlen(output), "%d  ", a[i]);
	}
	sprintf(output + strlen(output), "\n\n");
	write(1, output, strlen(output));

	/* Print x[]. */
	sprintf(output, "*** MAIN: Input array x[] for merge has %d elements:\n    ", m);
	for(i=0; i<m; i++) {
		sprintf(output + strlen(output), "%d  ", x[i]);
	}
	sprintf(output + strlen(output), "\n\n");
	write(1, output, strlen(output));

	/* Print y[]. */
	sprintf(output, "*** MAIN: Input array y[] for merge has %d elements:\n    ", n);
	for(i=0; i<n; i++) {
		sprintf(output + strlen(output), "%d  ", y[i]);
	}
	sprintf(output + strlen(output), "\n\n");
	write(1, output, strlen(output));

	/* CREATE CHILD PROCESSES */
	qsortChild();
	mergeChild();

	/* Wait for child processes to finish. */
	while(wait(NULL)>0);
	/* Print the result of qsort. */
	sprintf(output, "*** MAIN: sorted array by qsort:\n     ");
	for (i=0; i< k; i++) {
		sprintf(output + strlen(output), "%d  ", a[i]);
	}
	sprintf(output + strlen(output), "\n\n");
	write(1, output, strlen(output));
	/* Print the result of merge. */
	sprintf(output, "*** MAIN: merged array:\n     ");
	for (i=0; i< (m+n); i++) {
		sprintf(output + strlen(output), "%d  ", z[i]);
	}
	sprintf(output + strlen(output), "\n\n");
	write(1, output, strlen(output));
	
	/* Clean up and exit. */
	cleanup();
	return 0;
}

/* ----------------------------------------------------------- */
/* FUNCTION : qsortChild                                       */
/*    Create child to run qsort.                               */
/* PARAMETER USAGE :                                           */
/*    N/A                                                      */
/* FUNCTIONS CALLED :                                          */
/*    execvp(), fork(), getpid(), malloc(), sprintf(),         */
/*    strlen(), write()                                        */
/* ----------------------------------------------------------- */
void qsortChild() {
	sprintf(output, "*** MAIN: about to spawn the process for qsort\n");
	write(1, output, strlen(output));
	/* Setup the arg list. */
	argQ=malloc(sizeof(char*)*6);
	for(i=0; i<5; ++i) {
		argQ[i]=malloc(sizeof(char)*100);
	}
	sprintf(argQ[0], "qsort");
	sprintf(argQ[1], "%d", 0);
	sprintf(argQ[2], "%d", k-1);
	sprintf(argQ[3], "%d", k);
	sprintf(argQ[4], "%d", shmIDA);
	argQ[5]=NULL;
	/* Create child. */
	if (fork()==0) {
		sprintf(output, "   ### Q-PROC(%d): entering with a[0..%d]\n       ", getpid(), k-1);
		for (i=0; i<k; i++) {
			sprintf(output+strlen(output), "%d  ", a[i]);
		}
		sprintf(output+strlen(output), "\n       ..........\n");
		write(1, output, strlen(output));
		execvp("./qsort", argQ);
	}
	return;
}

/* ----------------------------------------------------------- */
/* FUNCTION : mergeChild                                       */
/*    Create child to run merge.                               */
/* PARAMETER USAGE :                                           */
/*    N/A                                                      */
/* FUNCTIONS CALLED :                                          */
/*    execvp(), fork(), getpid(), malloc(), sprintf(),         */
/*    strlen(), write()                                        */
/* ----------------------------------------------------------- */
void mergeChild() {
	sprintf(output, "*** MAIN: about to spawn the process for merge\n");
	write(1, output, strlen(output));
	/* Setup the arg list. */
	argM=malloc(sizeof(char*)*7);
	for(i=0; i<6; i++) {
		argM[i]=malloc(sizeof(char)*100);
	}
	sprintf(argM[0], "merge");
	sprintf(argM[1], "%d", m);
	sprintf(argM[2], "%d", n);
	sprintf(argM[3], "%d", shmIDX);
	sprintf(argM[4], "%d", shmIDY);
	sprintf(argM[5], "%d", shmIDZ);
	argM[6]=NULL;
	/* Create child. */
	if (fork()==0) {
		execvp("./merge", argM);
	}
	return;
}

/* ----------------------------------------------------------- */
/* FUNCTION : setupSHM                                         */
/*    Created/attach shared memory and read from stdin.        */
/* PARAMETER USAGE :                                           */
/*    N/A                                                      */
/* FUNCTIONS CALLED :                                          */
/*    ftok(), scanf(), shmat(), shmget(), sprintf(), strlen(), */
/*    write()                                                  */
/* ----------------------------------------------------------- */
void setupSHM() {
	/** Create and write to A[k]. **/
	scanf("%d", &k);
	keyA=ftok("./", 'a');
	sprintf(output, "*** MAIN: qsort shared memory key = %d\n", keyA);
	write(1, output, strlen(output));
	shmIDA = shmget(keyA, k*sizeof(int), IPC_CREAT | 0666);;
	sprintf(output, "*** MAIN: qsort shared memory created\n");
	write(1, output, strlen(output));
	a = shmat(shmIDA, NULL, 0);
	sprintf(output, "*** MAIN: qsort shared memory attached and is ready to use\n");
	write(1, output, strlen(output));
	for(i=0; i<k; i++) {
		scanf("%d", &(a[i]));
	}
	/**  Create and write to X[m]. **/
	scanf("%d", &m);
	keyX=ftok("./", 'x');
	sprintf(output, "*** MAIN: merge shared memory key 1 of 3 (for x[]) = %d\n", keyX);
	write(1, output, strlen(output));
	shmIDX = shmget(keyX, m*sizeof(int), IPC_CREAT | 0666);
	sprintf(output, "*** MAIN: merge shared memory 1 of 3 created\n");
	write(1, output, strlen(output));
	x = shmat(shmIDX, NULL, 0);
	sprintf(output, "*** MAIN: merge shared memory 1 of 3 attached and is ready to use\n");
	write(1, output, strlen(output));
	for(i=0; i<m; i++) {
		scanf("%d", &(x[i]));
	}
	/**  Create and write to Y[n] **/
	scanf("%d", &n);
	keyY=ftok("./", 'y');
	sprintf(output, "*** MAIN: merge shared memory key 2 of 3 (for y[]) = %d\n", keyY);
	write(1, output, strlen(output));
	shmIDY = shmget(keyY, n*sizeof(int), IPC_CREAT | 0666);
	sprintf(output, "*** MAIN: merge shared memory 2 of 3 created\n");
	write(1, output, strlen(output));
	y = shmat(shmIDY, NULL, 0);
	sprintf(output, "*** MAIN: merge shared memory 2 of 3 attached and is ready to use\n");
	write(1, output, strlen(output));
	for(i=0; i<n; i++) {
		scanf("%d", &(y[i]));
	}
	/** Create Z[m+n]. **/
	keyZ=ftok("./", 'z');
	sprintf(output, "*** MAIN: merge shared memory key 3 of 3 (for the merged array) = %d\n", keyZ);
	write(1, output, strlen(output));
	shmIDZ = shmget(keyZ, (m+n)*sizeof(int), IPC_CREAT | 0666);
	sprintf(output, "*** MAIN: merge shared memory 3 of 3 created\n");
	write(1, output, strlen(output));
	z = shmat(shmIDZ, NULL, 0);
	sprintf(output, "*** MAIN: merge shared memory 3 of 3 attached and is ready to use\n");
	write(1, output, strlen(output));
	return;
}

/* ----------------------------------------------------------- */
/* FUNCTION : cleanup                                          */
/*    Free up allocated memory. Detach and remove shared       */
/*    memory.                                                  */
/* PARAMETER USAGE :                                           */
/*    N/A                                                      */
/* FUNCTIONS CALLED :                                          */
/*    free(), shmctl(), shmdt(), sprintf(), strlen(), write()  */
/* ----------------------------------------------------------- */
void cleanup() {
	int i;
	/** Free up allocated memory. **/
	for(i=0; i<5; i++) {
		if (argQ[i]!=NULL) free(argQ[i]);
	}
	if (argQ!=NULL) free(argQ);
	for(i=0; i<6; i++) {
		if (argM[i]!=NULL) free(argM[i]);
	}
	if (argM!=NULL) free(argM);
	/* Clean-up memory for qsort. */
	shmdt(a);
	sprintf(output, "*** MAIN: qsort shared memory successfully detached\n");
	write(1, output, strlen(output));
	shmctl(shmIDA, IPC_RMID, NULL);
	sprintf(output, "*** MAIN: qsort shared memory successfully removed\n");
	write(1, output, strlen(output));
	/* Clean up memory for merge 1 of 3. */
	shmdt(x);
	sprintf(output, "*** MAIN: merge shared memory 1 of 3 successfully detached\n");
	write(1, output, strlen(output));
	shmctl(shmIDX, IPC_RMID, NULL);
	sprintf(output, "*** MAIN: merge shared memory 1 of 3 successfully removed\n");
	write(1, output, strlen(output));
	/* Clean up memory for merge 2 of 3. */
	shmdt(y);
	sprintf(output, "*** MAIN: merge shared memory 2 of 3 successfully detached\n");
	write(1, output, strlen(output));
	shmctl(shmIDY, IPC_RMID, NULL);
	sprintf(output, "*** MAIN: merge shared memory 2 of 3 successfully removed\n");
	write(1, output, strlen(output));
	/* Clean up memory for merge 3 of 3. */
	shmdt(z);
	sprintf(output, "*** MAIN: merge shared memory 3 of 3 successfully detached\n");
	write(1, output, strlen(output));
	shmctl(shmIDZ, IPC_RMID, NULL);
	sprintf(output, "*** MAIN: merge shared memory 3 of 3 successfully removed\n");
	write(1, output, strlen(output));
	return;
}

