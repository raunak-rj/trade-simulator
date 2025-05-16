#include "almgrenChrissModel.h"
#include <cmath>

almgrenChrissModel::almgrenChrissModel(double eta, double gamma)
    : eta(eta), gamma(gamma) {}

double almgrenChrissModel::computeVolume(const std::vector<OrderLevel> &levels, int N) const
{
    double volume = 0.0;
    for (int i = 0; i < std::min(N, (int)levels.size()); ++i)
        volume += levels[i].size;
    return volume;
}

double almgrenChrissModel::estimateMarketImpact(double Q, double sigma, const L2Snapshot &snapshot, bool isBuy) const
{
    const auto &side = isBuy ? snapshot.asks : snapshot.bids;
    double V = computeVolume(side); 

    if (V < 1e-9)
        return 0.0; 

    double term1 = eta * (Q / V);
    double term2 = gamma * sigma * std::sqrt(Q / V);
    return term1 + term2;
}
