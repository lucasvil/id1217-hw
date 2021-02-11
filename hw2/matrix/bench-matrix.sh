#! /bin/bash
gcc matrixSum-openmp.c -o ms.o -fopenmp
for threads in 1 2 3 4 5 6 7 8; do
  echo -n "" >./data/median$threads.dat
  size=2
  while [ $size -le 8192 ]; do
    echo -n "" >./data/tmp.txt
    for n in $(seq 15); do
      echo $(./ms.o $size $threads | grep -Eo '[0-9]+\.[0-9]+') >>./data/tmp.txt
    done
    sort ./data/tmp.txt >./data/sort.txt
    echo $size $(sed -n 8p ./data/sort.txt) >>./data/median$threads.dat
    size=$((size * 2))
  done
done
