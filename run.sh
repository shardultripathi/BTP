#!/bin/bash
if [ $1 = 0 ]; then
	g++ -std=c++11 -fopenmp -O3 -march=native model.cpp -o model
	time ./model 0
	python3 regime.py 1
else	
	g++ -std=c++11 -fopenmp -march=native model.cpp -o model
	time ./model 1
	python3 regime.py 8
fi