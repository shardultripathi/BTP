from matplotlib import pyplot as plt
import numpy as np

from sklearn.linear_model import RANSACRegressor
from sklearn.metrics import mean_squared_error
from sklearn.preprocessing import PolynomialFeatures
from sklearn.pipeline import make_pipeline

np.random.seed(42)

X = np.random.normal(size=400)
y = X
# Make sure that it X is 2D
X = X[:, np.newaxis]

y_errors = y.copy()
for i in range(400):
    if i%20 == 0:
        y_errors[i] = np.random.randint(-2,10)

x_plot = np.linspace(X.min(), X.max())
title = 'Linear with outliers'
this_X = X
this_y = y_errors

plt.figure(figsize=(5, 4))
plt.plot(this_X[:, 0], this_y, 'b+')

name = 'RANSAC'
estimator = RANSACRegressor(random_state=42)

model = make_pipeline(PolynomialFeatures(1), estimator)
model.fit(this_X, this_y)
mse = mean_squared_error(model.predict(X), y)
y_plot = model.predict(x_plot[:, np.newaxis])
plt.plot(x_plot, y_plot, color='lightgreen', linestyle='-', linewidth=2, label='%s: error = %.3f' % (name, mse))

legend_title = 'Error of Mean\nAbsolute Deviation\nto Non-corrupt Data'
legend = plt.legend(loc='upper right', frameon=False, title=legend_title,
                        prop=dict(size='x-small'))
plt.xlim(-4, 10.2)
plt.ylim(-2, 10.2)
plt.title(title)
plt.show()