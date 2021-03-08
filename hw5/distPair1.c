#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>

void teacher(int numTasks);
void student(int rank);

int main(int argc, char* argv[]) {
  int numTasks, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    teacher(numTasks);
  } else {
    student(rank);
  }

  MPI_Finalize();
  return 0;
}

void teacher(int numTasks) {
  int group[2];
  int numStudents = 0;

  // loop through each student
  for (int i = 1; i < (numTasks); i++) {
    // wait for student message
    MPI_Recv(&group[numStudents++], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (numStudents > 1) {
      // two students chosen
      MPI_Send(&group[1], 1, MPI_INT, group[0], 0, MPI_COMM_WORLD);
      MPI_Send(&group[0], 1, MPI_INT, group[1], 0, MPI_COMM_WORLD);
      numStudents = 0;
    } else if ((i == (numTasks - 1))) {
      // no partner, no students left
      MPI_Send(&group[0], 1, MPI_INT, group[0], 0, MPI_COMM_WORLD);
    }
  }
}

void student(int rank) {
  int partner;
  // send request to teacher
  MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  // wait for teacher to assign a partner
  MPI_Recv(&partner, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // print if working with partner or alone
  if (rank == partner) {
    printf("student %d is working alone\n", rank);
  } else {
    printf("student %d is working with %d\n", rank, partner);
  }
}