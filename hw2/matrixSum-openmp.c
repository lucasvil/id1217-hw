/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];
void* Worker(void*);

/* read command line, initialize, and create threads */
int main(int argc, char* argv[]) {
  int i, j, total, maxX, maxY, minX, minY = 0;
  int max = -1;
  int min = MAXSIZE;

  /* read command line args if any */
  size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
#ifdef DEBUG
    printf("[ ");
#endif
    for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
#ifdef DEBUG
      printf(" %d", matrix[i][j]);
#endif
    }
#ifdef DEBUG
    printf(" ]\n");
#endif
  }

  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++) {
      if (max < matrix[i][j]) {
        max = matrix[i][j];
        maxX = i;
        maxY = j;
      }
      if (min > matrix[i][j]) {
        min = matrix[i][j];
        minX = i;
        minY = j;
      }
      total += matrix[i][j];
    }
  // implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("the maximum value is: %d at (%d, %d)\n", max, maxX, maxY);
  printf("the minumum value is: %d at (%d, %d)\n", min, minX, minY);
  printf("it took %g ms\n", (end_time - start_time) * 1000);

}
