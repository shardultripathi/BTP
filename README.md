## Classification Algorithm:
1. Worked on tuning the parameters manually which will help us know where to start the automation from and devise some heuristics for updating the parameters in future.
2. Wrote the program to find the regime, given a distribution (haven't tested yet):
    1. For viral regime, learn a straight line best fitted for the given data and calculate the deviation from the line. If it is smaller than a threshold, the regime is viral (using Robust fitting to take care of the outliers)
    2. Otherwise, to distinguish between sub-viral and super-viral regimes, locally weighted linear regression is used to find the distribution. Assuming that the distribution will be unimodal, the derivative is computed for the distribution and if the peak is greater than the threshold, the regime is super-viral otherwise sub-viral

## TODO:
1. Understand which distribution(graph)
2. Finish code for regime.py
3. Run model.cpp for different parameters and observe distributions (Write a code to draw graph of desired values)