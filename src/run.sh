#!/bin/bash
n=15000
k=75
p=0.2
if [ $1 = 0 ]; then
	# python3 wsg.py $n $k $p
	g++ -std=c++11 -fopenmp -O2 -march=native model.cpp -o model 
	time ./model 0  $n
	python3 regime.py 1
	python3 topicEvolution.py 1
else	
	python3 wsg.py $n $k $p
	g++ -std=c++11 -fopenmp -O2 -march=native model.cpp -o model 
	time ./model 1 $n
	python3 regime.py 8
	python3 topicEvolution.py 8
fi