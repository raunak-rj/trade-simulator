#include "almgrenEstimator.h"
#include <algorithm>
#include <cmath>
#include <iostream>

almgrenParameterEstimator::almgrenParameterEstimator(int depthLevel)
    : depthLevel(depthLevel) {}

double almgrenParameterEstimator::computeTopDepthUSD(const std::vector<OrderLevel> &levels, int depth) const
{
    double total = 0.0;
    for (int i = 0; i < std::min(depth, (int)levels.size()); ++i)
    {
        total += levels[i].price * levels[i].size;
    }
    return total;
}

std::pair<double, double> almgrenParameterEstimator::estimate(const L2Snapshot &snapshot, double volatility, double dailyVolumeUSD, double orderSizeUSD) const
{
    if (snapshot.asks.empty() || snapshot.bids.empty())
    {
        std::cerr << "Invalid snapshot: empty book\n";
        return {0.0, 0.0};
    }

    // Estimate spread (temporary cost of taking liquidity)
    double spread = snapshot.asks[0].price - snapshot.bids[0].price;

    // Market depth: use top-N levels
    double totalBookDepth = computeTopDepthUSD(snapshot.bids, depthLevel) +
                            computeTopDepthUSD(snapshot.asks, depthLevel);

    // Heuristic: If we trade 1% of daily volume, temp impact = spread
    double eta = spread / (0.01 * dailyVolumeUSD + 1e-9);

    // Heuristic: If we trade 10% of daily volume, perm impact = spread
    double gamma = spread / (0.1 * dailyVolumeUSD + 1e-9);

    return {eta, gamma};
}
