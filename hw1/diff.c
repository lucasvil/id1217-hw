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

typedef struct buffer {
  char* buff[MAX_BUFF_SIZE];
  int size;
  FILE* fp;
} buffer;

pthread_mutex_t buff_lock;
pthread_cond_t buff_full, buff_empty;
buffer buff1;
buffer buff2;
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
    buff1.buff[i] = (char*)malloc(MAX_LINE_LENGTH);
    buff2.buff[i] = (char*)malloc(MAX_LINE_LENGTH);
  }
  /* assign file pointers */
  buff1.fp = fopen(argv[1], "r");
  buff2.fp = fopen(argv[2], "r");
  if ((buff1.fp == NULL) || (buff2.fp == NULL)) {
    fprintf(stderr, "File not found.\n");
    exit(-1);
  }

  /* init size */
  buff1.size = 0;
  buff2.size = 0;

  /* initialise variables */
  pthread_t prod1, prod2, cons;
  pthread_mutex_init(&buff_lock, NULL);
  pthread_cond_init(&buff_full, NULL);
  pthread_cond_init(&buff_empty, NULL);

  /* create threads */
  pthread_create(&prod1, NULL, producer, &buff1);
  pthread_create(&prod2, NULL, producer, &buff2);
  pthread_create(&cons, NULL, consumer, NULL);

  /* join thread */
  pthread_join(cons, NULL);

  pthread_exit(NULL);
}

void* producer(void* arg) {
  pthread_detach(pthread_self());
  buffer* buff = (buffer*)arg;
  int i = 0;
  while (TRUE) {
    pthread_mutex_lock(&buff_lock);
    /* wait if buffer full */
    if (buff->size == MAX_BUFF_SIZE)
      pthread_cond_wait(&buff_full, &buff_lock);
    pthread_mutex_unlock(&buff_lock);

    if (fgets(buff->buff[i], MAX_LINE_LENGTH, buff->fp) != NULL) {
      pthread_mutex_lock(&buff_lock);
      buff->size++;
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
    while (!done && (buff1.size == 0 || buff2.size == 0))
      pthread_cond_wait(&buff_empty, &buff_lock);
    pthread_mutex_unlock(&buff_lock);

    /* buffers empty and done */
    if ((done == 2) && buff1.size == 0 && buff2.size == 0)
      pthread_exit(NULL);

    /* compare lines */
    if ((buff1.size != 0) && (buff2.size != 0)) {
      if (strcmp(buff1.buff[i], buff2.buff[i]))
        printf("%s%s\n", buff1.buff[i], buff2.buff[i]);
    } else if ((buff1.size != 0) && done) // other is done
      printf("%s\n", buff1.buff[i]);
    else
      printf("%s\n", buff2.buff[i]);
    /* decrement size (if needed) */
    pthread_mutex_lock(&buff_lock);
    if (buff1.size)
      buff1.size--;
    if (buff2.size)
      buff2.size--;

    pthread_cond_broadcast(&buff_full);
    pthread_mutex_unlock(&buff_lock);

    i = (i+1) % MAX_BUFF_SIZE;
  }
}