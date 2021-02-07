#ifndef _REENTRANT 
#define _REENTRANT 
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define MAX_LINE_LENGTH 100
#define MAX_BUFF_SIZE 10

typedef struct Producer {
  char* buff[MAX_BUFF_SIZE];
  int size;
  FILE* fp;
} Producer;

pthread_mutex_t buff_lock;
pthread_cond_t buff_full, buff_empty;
Producer prod1;
Producer prod2;
int done = 0;

void* producer(void* arg);
void* consumer(void* arg);

int main(int argc, char* argv[]) {
  /* check for wrong usage */
  if (argc < 3) {
    fprintf(stderr, "Usage: ./diff <file1> <file2>\nExiting...\n");
    exit(-1);
  }

  /* allocate buffers */
  for (int i = 0; i < MAX_BUFF_SIZE; i++) {
    prod1.buff[i] = (char*)malloc(MAX_LINE_LENGTH);
    prod2.buff[i] = (char*)malloc(MAX_LINE_LENGTH);
  }
  /* assign file pointers */
  prod1.fp = fopen(argv[1], "r");
  prod2.fp = fopen(argv[2], "r");
  if ((prod1.fp == NULL) || (prod2.fp == NULL)) {
    fprintf(stderr, "File not found.\n");
    exit(-1);
  }

  /* init size */
  prod1.size = 0;
  prod2.size = 0;

  /* initialise variables */
  pthread_t prod1_thread, prod2_thread, cons_thread;
  pthread_mutex_init(&buff_lock, NULL);
  pthread_cond_init(&buff_full, NULL);
  pthread_cond_init(&buff_empty, NULL);

  /* create threads */
  pthread_create(&prod1_thread, NULL, producer, &prod1);
  pthread_create(&prod2_thread, NULL, producer, &prod2);
  pthread_create(&cons_thread, NULL, consumer, NULL);

  /* join thread */
  pthread_join(cons_thread, NULL);

  pthread_exit(NULL);
}

void* producer(void* arg) {
  pthread_detach(pthread_self());
  Producer* prod = (Producer*)arg;
  int i = 0;
  while (TRUE) {
    pthread_mutex_lock(&buff_lock);
    /* wait if buffer full */
    if (prod->size == MAX_BUFF_SIZE)
      pthread_cond_wait(&buff_full, &buff_lock);
    pthread_mutex_unlock(&buff_lock);

    if (fgets(prod->buff[i], MAX_LINE_LENGTH, prod->fp) != NULL) {
      pthread_mutex_lock(&buff_lock);
      prod->size++;
      i = (i+1) % MAX_BUFF_SIZE;

      /* signal consumer */
      pthread_cond_signal(&buff_empty);
      pthread_mutex_unlock(&buff_lock);
    } else {
      /* producer is done, signal consumer */
      pthread_mutex_lock(&buff_lock);
      done++;
      pthread_mutex_unlock(&buff_lock);
      pthread_cond_signal(&buff_empty);
      pthread_exit(NULL);
    }
  }
}

void* consumer(void* arg) {
  int i = 0;

  while (TRUE) {
    pthread_mutex_lock(&buff_lock);
    /* one (or both) buffer is empty and neither is done, wait for producers */
    while (!done && (prod1.size == 0 || prod2.size == 0))
      pthread_cond_wait(&buff_empty, &buff_lock);
    if ((done == 1) && (prod1.size == 0) && (prod2.size == 0)) // did not include in submission...
      pthread_cond_wait(&buff_empty, &buff_lock);
    pthread_mutex_unlock(&buff_lock);

    /* buffers empty and done */
    if ((done == 2) && prod1.size == 0 && prod2.size == 0)
      pthread_exit(NULL);

    /* compare lines */
    if ((prod1.size != 0) && (prod2.size != 0)) {
      if (strcmp(prod1.buff[i], prod2.buff[i]))
        printf("%s%s\n", prod1.buff[i], prod2.buff[i]);
    } else if ((prod1.size != 0) && done) // other is done
      printf("%s\n", prod1.buff[i]);
    else
      printf("%s\n", prod2.buff[i]);
    /* decrement size (if needed) */
    pthread_mutex_lock(&buff_lock);
    if (prod1.size)
      prod1.size--;
    if (prod2.size)
      prod2.size--;

    pthread_cond_broadcast(&buff_full);
    pthread_mutex_unlock(&buff_lock);

    i = (i+1) % MAX_BUFF_SIZE;
  }
}