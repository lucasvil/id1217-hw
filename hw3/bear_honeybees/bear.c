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
#define MAX_PORTIONS 128
#define MAX_BEES 15

sem_t full, potlock;
int portions, numBees;
int honeypot = 0;

void* Bee(void* arg);
void* Bear(void* arg);

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: ./bear <portions> <bees>\n");
    exit(-1);
  }
  portions = (atoi(argv[1]) < MAX_PORTIONS) ? atoi(argv[1]) : MAX_PORTIONS;
  numBees = (atoi(argv[2]) < MAX_BEES) ? atoi(argv[2]) : MAX_BEES;

  pthread_t bear;
  pthread_t bees[numBees];

  srand(time(NULL));
  sem_init(&potlock, FALSE, 1);
  sem_init(&full, FALSE, 0);

  pthread_create(&bear, NULL, Bear, NULL);
  for (long i = 0; i < numBees; i++)
    pthread_create(&bees[i], NULL, Bee, (void*)i);

  pthread_exit(NULL);
}

void* Bee(void* arg) {
  long id = (long)arg;
  while (TRUE) {
    // fetch honey
#ifndef NOSLEEP
    sleep((rand()%5 + 1));
#endif

    // take pot lock
    sem_wait(&potlock);
    // put honey in pot if possible
    if (honeypot < portions) {
      honeypot++;
      printf("bee %ld has added a portion! honeypot %d/%d\n", id, honeypot, portions);

      // check if pot full
      if (honeypot < portions) {
        sem_post(&potlock);
      } else {
        sem_post(&potlock);
        printf("**bee %ld is alerting the bear**\n\n", id);
        sem_post(&full);
      }
    } else
      sem_post(&potlock);
  }
}

void* Bear(void* arg) {
  while (TRUE) {
    sem_wait(&full);
    printf("bear is awake! eating the honey...\n");

    // no bees in pot, eat honey
    sem_wait(&potlock);
#ifndef NOSLEEP
    sleep((rand()%4 + 2));
#endif
    honeypot = 0;

    // pot is empty
    printf("bear going back to sleep.\n\n");
    sem_post(&potlock);
  }
}