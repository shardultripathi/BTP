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

threshold1 = 1
threshold2 = 5
numProcesses = int(sys.argv[1])

y = [0.0]*500

for iter in range(numProcesses):
	with open('data/nodesVsTime'+str(iter)+'.dat') as file:
	    for line in file: 
	        line = line.strip().split()
	        for i,x in enumerate(line):
	        	x = int(x)
	        	if x == 0:
	        		logx = 0.0
	        	else:
	        		logx = log(x)
	        	y[i] = max(y[i], logx)


	x = [log(i) for i in range(1,501)]

	# normalise the data
	meanx = statistics.mean(x)
	sdx = statistics.stdev(x)
	x = [(a-meanx)/sdx for a in x]
	meany = statistics.mean(y)
	sdy = statistics.stdev(y)
	y = [(a-meany)/sdy for a in y]

	# Check for viral
	X = np.array(x).reshape((len(x),1))
	Y = np.array(y)
	Y = -np.sort(-Y)

	ransac = linear_model.RANSACRegressor()
	ransac.fit(X, Y)
	inlier_mask = ransac.inlier_mask_
	outlier_mask = np.logical_not(inlier_mask)

	# Predict data of estimated models
	line_X = np.arange(X.min(), X.max())[:, np.newaxis]
	line_y_ransac = ransac.predict(line_X)

	mse = mean_squared_error(ransac.predict(X), Y)
	if mse < threshold1:
		print(str(iter)+': Viral Regime')
		plt.plot(X, Y, 'o', label='log(Highest Peak)')
		plt.plot(line_X, line_y_ransac, color='yellow', linewidth=2, label='Fitted curve')
		plt.legend(loc='upper right')
		plt.xlabel('log(Topic rank)')
		plt.savefig('figures/fig'+str(iter)+'.png')
		# plt.show()
		exit()
	lowess = sm.nonparametric.lowess(y, x, frac=.03)
	lowess_x = list(zip(*lowess))[0]
	lowess_y = list(zip(*lowess))[1]
	f = interp1d(lowess_x, lowess_y, bounds_error=False)

	xnew = [i for i in np.arange(min(x),max(x)+.1,0.1)]
	ynew = f(xnew)

	dy = [(ynew[i+1]-ynew[i])*10. for i in range(len(xnew)-1)]
	if max(dy) > threshold2:
		print(str(iter)+':Super-viral Regime')
	else:
		print(str(iter)+':Sub-viral Regime')
	print(max(dy))
	plt.plot(x, y, 'o', label='log(Highest Peak)')
	plt.plot(xnew, ynew, 'y-', label='Fitted curve')
	plt.legend(loc='upper right')
	plt.xlabel('log(Topic id)')
	plt.savefig('figures/fig'+str(iter)+'.png')
	# plt.show()