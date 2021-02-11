#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_WORD_LENGTH 50
#define MAX_WORD_COUNT 30000
#define MAX_THREADS 8

typedef struct WordBuffer {
  char* buff[MAX_WORD_COUNT];
  int size;
} WordBuffer;

WordBuffer words;
WordBuffer palindromes;

int isPalindromic(char* word1, char* word2);

int main(int argc, char* argv[]) {
  // check correct usage
  if (argc < 2) {
    fprintf(stderr, "Usage: ./palindrome <file> <threads>\nExiting...\n");
    exit(-1);
  }

  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) {
    // File not found
    fprintf(stderr, "File not found.\n");
    exit(-1);
  }
  int numThreads = (argc > 2) ? atoi(argv[2]) : MAX_THREADS;
  if (numThreads > MAX_THREADS) numThreads = MAX_THREADS;

  omp_set_num_threads(numThreads);
  double start_time, end_time;

  words.size = 0;
  palindromes.size = 0;

  // read words from file
  char* word = malloc(MAX_WORD_LENGTH);
  int wlen = 0;
  while (fgets(word, MAX_WORD_LENGTH, fp) != NULL) {
    wlen = strlen(word);
    if (word[wlen-1] == '\n') wlen--;
    words.buff[words.size] = malloc(wlen);
    memcpy(words.buff[words.size], word, wlen);
    words.size++;
  }
  free(word);

  start_time = omp_get_wtime();
  int j;
  int i;
#pragma omp parallel for private(j)
  for (i = 0; i < words.size; i++) {
    // check against self
    if (isPalindromic(words.buff[i], words.buff[i])) {
#pragma omp critical
      palindromes.buff[palindromes.size++] = words.buff[i];
    } else {
      for (j = i+1; j < words.size; j++) {
        if (isPalindromic(words.buff[i], words.buff[j])) {
#pragma omp critical
          {
            palindromes.buff[palindromes.size++] = words.buff[i];
            palindromes.buff[palindromes.size++] = words.buff[j];
          }
          break;
        }
      }
    }
  }
  end_time = omp_get_wtime();

  // Output result
  printf("there are %d palindromic words in the file.\n", palindromes.size);
#ifdef DEBUG
  printf("\nthey are as follows:\n");
  for (int i = 0; i < palindromes.size; i++) {
    printf("%s\n", palindromes.buff[i]);
  }
#endif
  printf("time %g\n", end_time - start_time);
}

// returns 1 if palindromic, 0 else
int isPalindromic(char* word1, char* word2) {
  // get length of strings
  int w1len = strlen(word1);
  int w2len = strlen(word2);

  // if equal length, continue comparison
  if (w1len == w2len) {
    char c1;
    char c2;
    for (int i = 0; i < w1len; i++) {
      c1 = tolower(word1[w1len-i-1]);
      c2 = tolower(word2[i]);
      if (c1 != c2)
        return 0;
    }
    return 1;
  } else
    return 0;
}