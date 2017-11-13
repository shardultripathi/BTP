import numpy as np
from sklearn import linear_model, datasets
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm
import statistics
import sys
from math import log

threshold1 = 40
threshold2 = 20
threshold3 = 10
numProcesses = int(sys.argv[1])
iterator = int(sys.argv[2])


fname = '../data/nodesVsTime'
with open('results.txt', 'a') as outfile:
    for iter in range(numProcesses):
        size = 1000
        y = [-1.0]*size
        init = [0]*size
        maxtime = [0]*size
        first = [0]*size
        last = [0]*size
        time = 0
        plt.clf()
        with open(fname+str(iter)+'.dat') as file:
            for line in file: 
                time = time + 1
                line = line.strip().split()
                for i,x in enumerate(line):
                    x = int(x)
                    if x > y[i]:
                        if y[i] == -1.0:
                            init[i] = time
                        y[i] = x
                        maxtime[i] = time
                    if (x != 0) and (first[i] == 0):
                        first[i] = time
                    if (x < 4):
                        if (first[i] != 0) and (last[i] == 0):
                            last[i] = time
                    else:
                        if (last[i] != 0):
                            last[i] = 0
                    size = i


        x = [i for i in range(1,size-22)]
        y = y[24:size+1]
        init = init[24:size+1]
        first = first[24:size+1]
        last = last[24:size+1]
        for i in range(size-23):
            if last[i]==0 and first[i] != 0:
                last[i] = time+1
        maxtime = maxtime[24:size+1]
        maxtime = [maxtime[i] - init[i] for i in range(size-23)]
        lifetime = [last[i] - first[i] for i in range(size-23)]

        neg = 0
        for i in range(size-23):
            if lifetime[i] < threshold3:
                neg = neg + 1

        sdy = statistics.stdev(maxtime)
        if sdy < threshold1:
            print(iterator, iter, '1', file=outfile)
        elif neg > threshold2:
            print(iterator, iter, '3', file=outfile)
        else:
            print(iterator, iter, '2', file=outfile)