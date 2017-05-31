#!/bin/bash

start=1000
max=100000000
for ((i=start;i<=max;i=i*2)); do 
./kernel -b fileparse -s $i
done
