#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define TRUE 1
#define FALSE 1
#define MAX_WORMS 128
#define MAX_BABY 7

sem_t empty, full;
int worms;

void* parrent(void* arg);
void* baby(void* arg);

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./birds <worms> <babies>\n");
    exit(-1);
  }
  int numWorm = (atoi(argv[1]) < MAX_WORMS) ? atoi(argv[1]) : MAX_WORMS;
  int numBaby = (atoi(argv[2]) < MAX_BABY) ? atoi(argv[2]) : MAX_BABY;
  pthread_t p;
  pthread_t b[numBaby];

  worms = numWorm;

  srand(time(NULL));
  sem_init(&empty, FALSE, 0);
  sem_init(&full, FALSE, 1);

  printf("Hungry birds starting with %d worms, and %d baby bird(s)\n", numWorm, numBaby);
  pthread_create(&p, NULL, parrent, &numWorm);
  for (long i = 0; i < numBaby; i++)
    pthread_create(&b[i], NULL, baby, (void*)i);

  pthread_exit(NULL);
}


void* parrent(void* arg) {
  int* refill = arg;
  while (TRUE) {
    sem_wait(&empty);
    printf("\nparrent bird is fetching worms... \n");
    sleep((rand()%4 + 2));
    worms = *refill;
    printf("worms refilled!\n\n");
    sem_post(&full);
  }
}

void* baby(void* arg) {
  long id = (long)arg;
  while (TRUE) {
    sem_wait(&full);
    printf("%d worms left. baby bird %ld has woken up\n", worms, id);
    if (worms > 0) {
      worms--;
      sem_post(&full);
      printf("%d worms left. baby bird %ld is sleeping\n", worms, id);
      sleep((rand()%4 + 2));
    } else {
      sem_post(&empty);
      printf("\n**bird %ld is chirping!**\n", id);
    }
  }
}