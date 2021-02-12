#! /bin/bash
gcc palindrome.c -o pl.o -fopenmp

iterations=$(seq 5)
threads=$(seq 8)
res_tmp="../tmp/res.tmp.txt"
words_tmp="../tmp/words.tmp.txt"

size=4
# clear result file
for thread in $threads; do
  echo -n "" >../result/palindrome-speedup$thread.dat
done

while [ $size -le 16384 ]; do
  echo "Computing for size: $size"
  sed -n "1,$size p" words >$words_tmp
  for thread in $threads; do
    echo -n "" >$res_tmp

    # run iterations
    for n in $iterations; do
      echo $(./pl.o $words_tmp $thread | grep -Eo '[0-9]+\.[0-9]+') >>$res_tmp
    done

    # calculate median
    median_speed=$(sort -g $res_tmp | awk 'NR==3')
    if [ $thread -eq 1 ]; then
      seq_speed=$median_speed
    fi
    speedup=$(echo $seq_speed / $median_speed | bc -l)
    echo $size $speedup $median_speed >>../result/palindrome-speedup$thread.dat
    echo -n "$thread..."
  done
  echo ""
  size=$((size * 2))
done
