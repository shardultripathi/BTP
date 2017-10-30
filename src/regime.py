import numpy as np
from sklearn import linear_model, datasets
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm
import statistics
import sys
from math import log

threshold1 = 0.3
threshold2 = -1
numProcesses = int(sys.argv[1])


fname = '../data/nodesVsTime'
for iter in range(numProcesses):
	size = 1000
	y = [0.0]*size
	y1 = [0.0]*size
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
	        	y[i] = max(y[i], logx)
	        	y1[i] = max(y1[i],x)
	        	size = i


	# x = [log(i) for i in range(1,size+2)]
	x = [log(i) for i in range(1,size-22)]
	# y = y[:size+1]
	y = y[24:size+1]
	y1 = y1[24:size+1]
	print(size)

	# normalise the data
	meanx = statistics.mean(x)
	sdx = statistics.stdev(x)
	# x = [(a-meanx)/sdx for a in x]
	meany = statistics.mean(y)
	sdy = statistics.stdev(y)
	# y = [(a-meany)/sdy for a in y]
	y.sort(reverse=True);

	# Check for viral
	X = np.array(x).reshape((len(x),1))
	Y = np.array(y)
	# Y = -np.sort(-Y)
	print('min =', min(y1))
	print('max =', max(y1))

	ransac = linear_model.LinearRegression()
	ransac.fit(X, Y)
	# inlier_mask = ransac.inlier_mask_
	# outlier_mask = np.logical_not(inlier_mask)

	# Predict data of estimated models
	line_X = np.arange(X.min(), X.max()+0.99)[:, np.newaxis]
	line_y_ransac = ransac.predict(line_X)

	mse = mean_squared_error(ransac.predict(X), Y)
	print('mse',mse)
	if iter == 3:
		print(str(iter)+': Viral Regime')
		plt.plot(X, Y, 'o', label='log(Highest Peak)')
		plt.plot(line_X, line_y_ransac, color='yellow', linewidth=2, label='Fitted curve')
		plt.legend(loc='upper right')
		plt.xlabel('log(Topic rank)')
		plt.savefig('../figures/fig'+str(iter)+'.png')
		# plt.show()
		continue
	lowess = sm.nonparametric.lowess(y, x, frac=.09)
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
	plt.xlabel('log(Topic id)')
	plt.savefig('../figures/fig'+str(iter)+'.png')
	# plt.show()