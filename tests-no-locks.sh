#!/bin/bash

N=1000000  # Set the value of N
THREAD_COUNTS=(1 2 3 4 5 6 7 8)  # Array of thread counts

for T in "${THREAD_COUNTS[@]}"
do
    for i in {1..10}
    do
        echo "[nolock] Running with $T threads (run $i)..."
        ./mt-collatz $N $T -nolock >> ${i}threads-nolock.csv 2>> ${i}threads-nolock-time.csv
    done
done