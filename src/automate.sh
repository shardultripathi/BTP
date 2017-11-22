#!/bin/bash
n=15000
k=75
p=0.2
g++ -std=c++11 -fopenmp -O2 -march=native model_auto.cpp -o model_auto
for ((i=0;i<275;i++))
do
	./model_auto 1 $n $i
	python3 classify_auto.py 8 $i
done
#python3 plot_auto.py
