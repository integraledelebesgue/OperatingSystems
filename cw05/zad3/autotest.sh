#!/bin/bash

for acc in 0.1 0.01 0.001 0.0001
do
    for n_proc in {1..10}
    do
        ./integral.out $acc $n_proc >> results.txt
    done
done
