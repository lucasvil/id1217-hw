#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpich/mpi.h>

#define FREE 0

void teacher(int size);
void student(int size, int rank);
int nextPartner(int students[], int size);
int nextExists(int students[], int size);

int main(int argc, char* argv[]) {
  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  srand(time(NULL) + rank);

  if (rank == 0) {
    teacher(size);
  } else {
    student(size, rank);
  }

  MPI_Finalize();
  return 0;
}

void teacher(int size) {
  int proc[size];

  // initialize process array
  proc[0] = 1;
  for (int i = 1; i < size; i++)
    proc[i] = FREE;

  // randomly choose first student to start
  int firstStudent = nextPartner(proc, size);
  MPI_Send(&proc, size, MPI_INT, firstStudent, 0, MPI_COMM_WORLD);
}

void student(int size, int rank) {
  int proc[size];

  MPI_Recv(&proc, size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  if (proc[rank] == FREE) {
    // doesn't have a partner
    proc[rank] = rank;
    if (nextExists(proc, size)) {
      int partner = nextPartner(proc, size);
      proc[partner] = rank;
      proc[rank] = partner;
      printf("student %d is working with %d\n", rank, partner);

      // let partner know it's taken
      MPI_Send(&proc, size, MPI_INT, partner, 0, MPI_COMM_WORLD);

      // let next person choose partner (if there is one)
      if (nextExists(proc, size)) {
        int next = nextPartner(proc, size);
        MPI_Send(&proc, size, MPI_INT, next, 0, MPI_COMM_WORLD);
      }
    } else {
      // could not find partner
      printf("student %d is working alone\n", rank);
    }
  } else {
    // already has partner
    printf("student %d is working with %d\n", rank, proc[rank]);
  }
}

// randomly find next
int nextPartner(int proc[], int size) {
  int partner = rand()%size;
  while (proc[partner] != FREE) {
    partner = ((partner+1)%size);
  }
  return partner;
}

// check if any students not taken
int nextExists(int proc[], int size) {
  for (int i = 1; i < size; i++) {
    if (proc[i] == FREE)
      return 1;
  }
  return 0;
}