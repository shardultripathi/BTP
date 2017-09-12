import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import statsmodels.api as sm

x = list(range(3, 33)) + [3.2, 6.2]
y = [1,2,1,2,1,1,3,4,5,4,5,6,5,6,7,8,9,10,11,11,12,11,11,10,12,11,11,10,9,8,2,13]

lowess = sm.nonparametric.lowess(y, x, frac=.3)

lowess_x = list(zip(*lowess))[0]
lowess_y = list(zip(*lowess))[1]
f = interp1d(lowess_x, lowess_y, bounds_error=False)

xnew = [i/10. for i in range(400)]
ynew = f(xnew)

dy = [(ynew[i+1]-ynew[i])*10. for i in range(399)]
print(ynew)
print(dy)


plt.plot(x, y, 'o')
# plt.plot(lowess_x, lowess_y, '*')
plt.plot(xnew, ynew, '-')
plt.plot(xnew[1:],dy,'--')
plt.show()