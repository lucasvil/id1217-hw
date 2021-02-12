#!/usr/bin/gnuplot -persist
set title "parallel matrix sum speedup"
set terminal "png"
set logscale x 2
set format x '2^{%L}'
set xlabel "size (x by x matrix)"
set ylabel "speedup (sequentail speed / parallel speed)"

set rmargin 15
set key at screen 1, graph 1

# plot for large matrix sizes
set output "../plots/matrix.png"
plot "../result/matrix-speedup1.dat" using 1:2 w lp pt 11 lt 1 title "1 thread", \
  "../result/matrix-speedup2.dat" using 1:2 w lp pt 11 title "2 thread", \
  "../result/matrix-speedup3.dat" using 1:2 w lp pt 11 title "3 thread", \
  "../result/matrix-speedup4.dat" using 1:2 w lp pt 11 title "4 thread", \
  "../result/matrix-speedup5.dat" using 1:2 w lp pt 11 title "5 thread", \
  "../result/matrix-speedup6.dat" using 1:2 w lp pt 11 title "6 thread", \
  "../result/matrix-speedup7.dat" using 1:2 w lp pt 11 title "7 thread", \
  "../result/matrix-speedup8.dat" using 1:2 w lp pt 11 title "8 thread"
