#!/bin/bash

for OPTIM in O0 O O1 O2 O3 Os 
do
    make static_test OPTIMIZATION=$OPTIM
    make shared_test OPTIMIZATION=$OPTIM
    make dl_test OPTIMIZATION=$OPTIM
done

make clean
