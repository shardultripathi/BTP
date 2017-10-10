import numpy as np
from matplotlib import pyplot as plt
from sklearn import linear_model, datasets
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm
import statistics
import sys
from math import log

threshold1 = 0.0
threshold2 = -1
numProcesses = int(sys.argv[1])


size = 500
fname = '../data/nodesVsTime'
for iter in range(numProcesses):
	y = [0.0]*500
	plt.clf()
	with open(fname+str(iter)+'.dat') as file:
	    for line in file: 
	        line = line.strip().split()
	        for i,x in enumerate(line):
	        	x = int(x)
	        	if x == 0:
	        		logx = 0.0
	        	else:
	        		logx = log(x)
	        		# logx = x
	        	y[i] = max(y[i], x)
	        	size = i


	x = [i for i in range(1,size-22)]
	# x = [log(i) for i in range(1,size+2)]
	# x = [i for i in range(1,501)]
	y = y[24:size+1]
	# y = y[:size+1]

	# normalise the data
	meanx = statistics.mean(x)
	sdx = statistics.stdev(x)
	# x = [(a-meanx)/sdx for a in x]
	meany = statistics.mean(y)
	sdy = statistics.stdev(y)
	# y = [(a-meany)/sdy for a in y]
	# y.sort(reverse=True)

	# Check for viral
	X = np.array(x).reshape((len(x),1))
	Y = np.array(y)
	# Y = -np.sort(-Y)
	print(len(Y))

	ransac = linear_model.LinearRegression()
	ransac.fit(X, Y)
	# inlier_mask = ransac.inlier_mask_
	# outlier_mask = np.logical_not(inlier_mask)

	# Predict data of estimated models
	line_X = np.arange(X.min(), X.max()+0.99)[:, np.newaxis]
	line_y_ransac = ransac.predict(line_X)
	
	mse = mean_squared_error(ransac.predict(X), Y)
	print('mse',mse)
	if mse < threshold1:
		print(str(iter)+': Viral Regime')
		plt.plot(X, Y, 'o', label='log(Highest Peak)')
		plt.plot(line_X, line_y_ransac, color='yellow', linewidth=2, label='Fitted curve')
		plt.legend(loc='upper right')
		plt.xlabel('log(Topic rank)')
		plt.savefig('../figures/fig'+str(iter)+'.png')
		# plt.show()
		continue
	lowess = sm.nonparametric.lowess(y, x, frac=.04)
	lowess_x = list(zip(*lowess))[0]
	lowess_y = list(zip(*lowess))[1]
	f = interp1d(lowess_x, lowess_y, bounds_error=False)

	xnew = [i for i in np.arange(min(x),max(x)+.01,0.01)]
	ynew = f(xnew)

	dy = [(ynew[i+1]-ynew[i])*10. for i in range(len(xnew)-1)]
	if min(dy) < threshold2:
		print(str(iter)+':Super-viral Regime')
	else:
		print(str(iter)+':Sub-viral Regime')
	print('slope',min(dy))
	plt.plot(x, y, 'o', label='log(Highest Peak)')
	plt.plot(xnew, ynew, 'y-', label='Fitted curve')
	plt.legend(loc='upper right')
	plt.xlabel('log(Topic rank)')
	plt.savefig('../figures/fig'+str(iter)+'.png')
	# plt.show()