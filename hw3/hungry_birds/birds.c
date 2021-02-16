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

sem_t empty, wormlock;
int worms;

void* Parent(void* arg);
void* Baby(void* arg);

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
  sem_init(&wormlock, FALSE, 1);

  printf("Hungry birds starting with %d worms, and %d baby bird(s)\n", numWorm, numBaby);
  pthread_create(&p, NULL, Parent, &numWorm);
  for (long i = 0; i < numBaby; i++)
    pthread_create(&b[i], NULL, Baby, (void*)i);

  pthread_exit(NULL);
}


void* Parent(void* arg) {
  int* refill = arg;
  while (TRUE) {
    // wait for dish to be empty
    sem_wait(&empty);
    printf("\nparent bird is fetching worms... \n");
#ifndef NOSLEEP
    sleep((rand()%4 + 2));
#endif
    worms = *refill;
    printf("worms refilled!\n\n");
    sem_post(&wormlock);
  }
}

void* Baby(void* arg) {
  long id = (long)arg;
  while (TRUE) {
    // take lock
    sem_wait(&wormlock);
    printf("baby bird %ld wants to eat... ", id);

    // any worms left?
    if (worms > 0) {
      worms--;
      printf("It ate a worm! %d worms left\n", worms);
      sem_post(&wormlock);

      // go to sleep.
#ifndef NOSLEEP
      sleep((rand()%4 + 2));
#endif
    } else {
      sem_post(&empty);
      printf("No worms! bird %ld is chirping!\n", id);
    }
  }
}