#!/bin/bash
ulimit -n 8192
#mpstat 3 -P 2 >> load-gen-iostat.txt &

echo "User,Request,Throughput,AverageRTT," > load-gen-output1.csv
    
for ((i=200; i<=2500; i=i+200 ))
do
    port=$((i+6000))
    echo $port
    taskset -c 7 ./server $port &
    sleep 4    
    taskset -c 0-5 ./load_gen localhost $port $i 0.1 30 >> load-gen-output1.csv
    killall -2 server
done
#killall mpstat
python3 load_gen_plot.py load-gen-output1.csv load-gen-iostat1.txt
