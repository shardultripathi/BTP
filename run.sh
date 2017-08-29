#!/bin/bash
if [ $1 = 0 ]; then
	g++ -std=c++11 -fopenmp -O3 -march=native model.cpp -o model
else	
	g++ -std=c++11 -fopenmp -march=native model.cpp -o model
fi
time ./model $1
