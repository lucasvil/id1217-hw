/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

typedef struct Elem {
  int val;
  int x;
  int y;
} Elem;

double start_time, end_time;
int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];
Elem min, max;
void* Worker(void*);

/* read command line, initialize, and create threads */
int main(int argc, char* argv[]) {
  int i, j = 0;
  int total = 0;
  max.val = -1;
  min.val = MAXSIZE;
  max.x, max.y, min.x, min.y = 0;

  // init rand
  time_t t;
  srand((unsigned)time(&t));

  /* read command line args if any */
  size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);
  int total_seq = 0;
  /* initialize the matrix */
  for (i = 0; i < size; i++) {
#ifdef DEBUG
    printf("[ ");
#endif
    for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      total_seq += matrix[i][j];
#ifdef DEBUG
      printf(" %d", matrix[i][j]);
#endif
    }
#ifdef DEBUG
    printf(" ]\n");
#endif
  }
#ifdef DEBUG
  printf("Seq total is %d\n", total_seq);
#endif

  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) private(j) shared(min, max)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++) {
      if (max.val < matrix[i][j]) {
#pragma omp critical (max)
        if (max.val < matrix[i][j]) {
          max.val = matrix[i][j];
          max.x = i;
          max.y = j;
        }
      }
      if (min.val > matrix[i][j]) {
#pragma omp critical (min)
        if (min.val > matrix[i][j]) {
          min.val = matrix[i][j];
          min.x = i;
          min.y = j;
        }
      }
      total += matrix[i][j];
    }

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("the maximum value is: %d at (%d, %d)\n", max.val, max.x, max.y);
  printf("the minumum value is: %d at (%d, %d)\n", min.val, min.x, min.y);
  printf("it took %g ms\n", (end_time - start_time) * 1000);

}
