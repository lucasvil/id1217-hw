#! /bin/bash
gcc matrixSum-openmp.c -o ms.o -fopenmp
for threads in 1 2 3 4; do
  echo -n "" >./data/ms$threads.dat
  size=2
  while [ $size -le 4096 ]; do
    for n in $(seq 10); do
      echo $(./ms.o $size $threads | grep -Eo '[0-9]+\.[0-9]+') $size >>./data/ms$threads.dat
    done
    size=$((size * 2))
  done
done
