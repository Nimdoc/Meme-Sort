#ifndef ANALYSISHELPER_H
#define ANALYSISHELPER_H

#include <QtMath>
#include <math.h>

class AnalysisHelper
{
public:
    AnalysisHelper();
    static int getLowerBound(int n);
    static int getUpperBound(int n);
private:
    static float zigZag(float x);
};

#endif // ANALYSISHELPER_H
