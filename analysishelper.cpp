#include "analysishelper.h"

AnalysisHelper::AnalysisHelper()
{

}

int AnalysisHelper::getLowerBound(int n)
{
    float a = (n / 2.0) * (qFloor(log2(n)) + 1);
    float b = 0;

    for(int k = 0; k <= qFloor(log2(n)); k++)
    {
        b += (powf(2, k)) * (zigZag(n/(powf(2, k + 1))));
    }

    return qFloor(a - b);
}

int AnalysisHelper::getUpperBound(int n)
{
    return qCeil(n * qCeil(log2(n)) - powf(2.0f, qCeil(log2(n))) + 1);
}

float AnalysisHelper::zigZag(float x)
{
    return fmin(x - qFloor(x), qCeil(x) - x);
}
