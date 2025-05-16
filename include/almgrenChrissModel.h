#ifndef ALMGREN_CHRISS_H
#define ALMGREN_CHRISS_H

#include "L2Snapshot.h"

class almgrenChrissModel
{
    public:
        almgrenChrissModel(double eta = 0.1, double gamma = 1.0);
        double estimateMarketImpact(double Q, double sigma, const L2Snapshot &snapshot, bool isBuy) const;

    private:
        double eta;
        double gamma;
        double computeVolume(const std::vector<OrderLevel> &levels, int N = 5) const;
};

#endif
