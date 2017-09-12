import numpy as np
from matplotlib import pyplot as plt
from sklearn import linear_model, datasets
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm
import statistics
from math import log

peak = [0.0]*500

with open("nodesVsTime.dat") as file:
    for line in file: 
        line = line.strip().split()
        for i,x in enumerate(line):
        	x = int(x)
        	if x == 0:
        		logx = 0.0
        	else:
        		logx = log(x)
        	peak[i] = max(peak[i], logx)

# peak.sort(reverse=True)
x = [log(i) for i in range(1,501)]

plt.plot(x, peak, '-')
plt.show()