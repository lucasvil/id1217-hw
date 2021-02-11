#!/usr/bin/gnuplot -persist
set title "parallel matrix sum benchmark"
set terminal "png"
set logscale x 2
set format x '2^{%L}'
set xlabel "size (x by x matrix)"
set ylabel "time (ms)"

set rmargin 15
set key at screen 1, graph 1

# plot for large matrix sizes
set output "./plots/matrixplotbig.png"
set yrange [0:130]
set xrange [900:10000]
plot "<(sed -n '10,13p' ./data/median1.dat)" using 1:2 w lp pt 11 lt 1 title "1 thread", \
  "<(sed -n '10,13p' ./data/median2.dat)" using 1:2 w lp pt 11 title "2 thread", \
  "<(sed -n '10,13p' ./data/median3.dat)" using 1:2 w lp pt 11 title "3 thread", \
  "<(sed -n '10,13p' ./data/median4.dat)" using 1:2 w lp pt 11 title "4 thread", \
  "<(sed -n '10,13p' ./data/median5.dat)" using 1:2 w lp pt 11 title "5 thread", \
  "<(sed -n '10,13p' ./data/median6.dat)" using 1:2 w lp pt 11 title "6 thread", \
  "<(sed -n '10,13p' ./data/median7.dat)" using 1:2 w lp pt 11 title "7 thread", \
  "<(sed -n '10,13p' ./data/median8.dat)" using 1:2 w lp pt 11 title "8 thread"
