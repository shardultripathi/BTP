import numpy as np
from matplotlib import pyplot as plt
from sklearn import linear_model, datasets
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm
import statistics

threshold1 = 1
threshold2 = 2

# read x and y
x = []
y = []
with open("file.dat") as file:
    for line in file: 
        xy = line.strip().split() #or some other preprocessing
        x.append(float(xy[0]))
        y.append(float(xy[1]))
meanx = statistics.mean(x)
sdx = statistics.stdev(x)
x = [(a-meanx)/sdx for a in x]
meany = statistics.mean(y)
sdy = statistics.stdev(y)
y = [(a-meany)/sdy for a in y]
X = np.array(x).reshape((len(x),1))
Y = np.array(y)

# normalise the data

ransac = linear_model.RANSACRegressor()
ransac.fit(X, Y)
inlier_mask = ransac.inlier_mask_
outlier_mask = np.logical_not(inlier_mask)

# Predict data of estimated models
line_X = np.arange(X.min(), X.max())[:, np.newaxis]
line_y_ransac = ransac.predict(line_X)

mse = mean_squared_error(ransac.predict(X), Y)
if mse < threshold1:
	print('Viral Regime')
	exit()
lowess = sm.nonparametric.lowess(y, x, frac=.3)
lowess_x = list(zip(*lowess))[0]
lowess_y = list(zip(*lowess))[1]
f = interp1d(lowess_x, lowess_y, bounds_error=False)

xnew = [i/10. for i in range(400)]
ynew = f(xnew)

dy = [(ynew[i+1]-ynew[i])*10. for i in range(399)]
if max(dy) > threshold2:
	print('Super-viral Regime')
else:
	print('Sub-viral Regime')