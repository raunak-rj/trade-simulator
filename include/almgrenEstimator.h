#ifndef ALMGREN_PARAMETER_ESTIMATOR_H
#define ALMGREN_PARAMETER_ESTIMATOR_H

#include "L2Snapshot.h"

class almgrenParameterEstimator {
public:
    almgrenParameterEstimator(int depthLevel = 5);

    std::pair<double, double> estimate(
        const L2Snapshot& snapshot,
        double volatility,           
        double dailyVolumeUSD,       
        double orderSizeUSD          
    ) const;

private:
    int depthLevel;

    double computeTopDepthUSD(const std::vector<OrderLevel>& levels, int depth) const;
};

#endif
