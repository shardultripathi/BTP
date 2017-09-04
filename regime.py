import numpy as np
from matplotlib import pyplot as plt
from sklearn import linear_model, datasets
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm

threshold1 = 1
threshold2 = 2

# read x and y
# normalise the data

ransac = linear_model.RANSACRegressor()
ransac.fit(X, y)
inlier_mask = ransac.inlier_mask_
outlier_mask = np.logical_not(inlier_mask)

# Predict data of estimated models
line_X = np.arange(X.min(), X.max())[:, np.newaxis]
line_y_ransac = ransac.predict(line_X)

mse = mean_squared_error(ransac.predict(X), y)
if mse < threshold1:
	print('Viral Regime')
	return

lowess = sm.nonparametric.lowess(y, X, frac=.3)
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