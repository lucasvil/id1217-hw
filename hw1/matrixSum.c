/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define MAXVAL 1000; /* maximum value of element in matrix */
#define MINVAL 0; /* minimum value of element in matrix */

pthread_mutex_t barrier, sumLock, maxLock, minLock;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */
int min, max, maxX, maxY, minX, minY;
int total = 0;

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

/* timer */
double read_timer() {
  static bool initialized = false;
  static struct timeval start;
  struct timeval end;
  if (!initialized)
  {
    gettimeofday(&start, NULL);
    initialized = true;
  }
  gettimeofday(&end, NULL);
  return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

void* Worker(void*);

/* read command line, initialize, and create threads */
int main(int argc, char* argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];
  srand(time(NULL)); /* rand init */

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_mutex_init(&sumLock, NULL);
  pthread_mutex_init(&maxLock, NULL);
  pthread_mutex_init(&minLock, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%MAXVAL;
    }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
    printf("[ ");
    for (j = 0; j < size; j++) {
      printf(" %d", matrix[i][j]);
    }
    printf(" ]\n");
}
#endif
  /* init max and min values */
  min = MAXVAL;
  max = MINVAL - 1;

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void*)l);

  /* wait for workers to complete */
  for (l = 0; l < numWorkers; l++)
    pthread_join(workerid[l], NULL);

  /* get end time */
  end_time = read_timer();
  /* print results */
  printf("The total is %d\n", total);
  printf("The maximum is %d at (%d, %d), and the minimum is %d at (%d, %d)", max, maxX, maxY, min, minX, minY);
  printf("The execution time is %g sec\n", end_time - start_time);

  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void* Worker(void* arg) {
  long myid = (long)arg;
  int localSum, localMax, localMin, localMaxX, localMaxY, localMinX, localMinY, i, j, first, last;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  /* local sum, min, max values */
  localSum = 0;
  localMin = MAXVAL;
  localMax = MINVAL;
  for (i = first; i <= last; i++) {
    for (j = 0; j < size; j++) {
      localSum += matrix[i][j];
      if (localMax < matrix[i][j]) {
        localMax = matrix[i][j];
        localMaxX = i;
        localMaxY = j;
      }
      if (localMin > matrix[i][j]) {
        localMin = matrix[i][j];
        localMinX = i;
        localMinY = j;
      }
    }
  }
  pthread_mutex_lock(&sumLock);
  total += localSum;
  pthread_mutex_unlock(&sumLock);
  if (localMax > max) {
    pthread_mutex_lock(&maxLock);
    if (localMax > max) {
      max = localMax;
      maxX = localMaxX;
      maxY = localMaxY;
    }
    pthread_mutex_unlock(&maxLock);
  }
  if (localMin < min) {
    pthread_mutex_lock(&minLock);
    if (localMin < min) {
      min = localMin;
      minX = localMinX;
      minY = localMinY;
    }
    pthread_mutex_unlock(&minLock);
  }

  // pthread_mutex_lock(&sumLock);
  // total += localSum;
  // if (localMax > max) {
  //   max = localMax;
  //   maxX = localMaxX;
  //   maxY = localMaxY;
  // }
  // if (localMin < min) {
  //   min = localMin;
  //   minX = localMinX;
  //   minY = localMinY;
  // }
  // pthread_mutex_unlock(&sumLock);
  pthread_exit(NULL);
}