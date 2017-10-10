#!/bin/bash
if [ $1 = 0 ]; then
	g++ -std=c++11 -O3 -march=native model.cpp -o model 
	time ./model 0 
	python3 regime.py 1
else	
	g++ -std=c++11 -fopenmp -O2 -march=native model_feedback2.cpp -o modelf2 
	time ./modelf2 1 
	# python3 regime.py 8
fi