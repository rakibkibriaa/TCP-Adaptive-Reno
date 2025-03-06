set terminal png
set key box
set key width 1
set key font "Arial,14"
set grid
set border 3
set tics nomirror
set output ARG1
set title ARG2
set xlabel "time"
set ylabel "Congestion Window"
plot ARG3 using int(ARG5):int(ARG6)  with linespoints title "new reno",ARG4 using int(ARG7):int(ARG8) with linespoints title ARG9