#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
#include <iomanip>
#include <memory>
using namespace std;

#include "aaa.h"

AAA::AAA() {
}

void AAA::getDetrend(double *y, int m)
{
    double xmean, ymean;
    int i;
    double temp;
    double Sxy;
    double Sxx;

    double grad;
    double yint;

    std::unique_ptr<double[]> x(new double[m]);

    /********************************
    Set the X axis Liner Values
    *********************************/
    for (i = 0; i < m; i++)
        x[i] = i;

    /********************************
    Calculate the mean of x and y
    *********************************/
    xmean = 0;
    ymean = 0;
    for (i = 0; i < m; i++)
    {
        xmean += x[i];
        ymean += y[i];
    }
    xmean /= m;
    ymean /= m;

    /********************************
    Calculate Covariance
    *********************************/
    temp = 0;
    for (i = 0; i < m; i++)
        temp += x[i] * y[i];
    Sxy = temp / m - xmean * ymean;

    temp = 0;
    for (i = 0; i < m; i++)
        temp += x[i] * x[i];
    Sxx = temp / m - xmean * xmean;

    /********************************
    Calculate Gradient and Y intercept
    *********************************/
    grad = Sxy / Sxx;
    yint = -grad * xmean + ymean;

    /********************************
    Removing Linear Trend
    *********************************/
    for (i = 0; i < m; i++)
        y[i] = y[i] - (grad * i + yint);
}

