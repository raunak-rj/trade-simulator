#include "featureExtractor.h"
#include <cmath>

std::vector<double> featureExtractor::extractFeatures(const L2Snapshot& snapshot, double orderSizeUSD) {
    std::vector<double> features;

    double bestBid = snapshot.bids.empty() ? 0 : snapshot.bids[0].price;
    double bestAsk = snapshot.asks.empty() ? 0 : snapshot.asks[0].price;
    double spread = bestAsk - bestBid;
    double midPrice = (bestAsk + bestBid) / 2;

    double depthBid = 0.0, depthAsk = 0.0;
    int N = std::min(5, std::min((int)snapshot.bids.size(), (int)snapshot.asks.size()));
    for (int i = 0; i < N; ++i) {
        depthBid += snapshot.bids[i].size;
        depthAsk += snapshot.asks[i].size;
    }

    double imbalance = (depthBid - depthAsk) / (depthBid + depthAsk + 1e-9);
    double askGap1 = snapshot.asks.size() > 1 ? snapshot.asks[1].price - snapshot.asks[0].price : 0;
    double bidGap1 = snapshot.bids.size() > 1 ? snapshot.bids[0].price - snapshot.bids[1].price : 0;
    double sizeRatio = orderSizeUSD / (depthAsk * bestAsk + 1e-9);

    features.push_back(1.0);
    features.push_back(spread);
    features.push_back(depthBid);
    features.push_back(depthAsk);
    features.push_back(imbalance);
    features.push_back(askGap1);
    features.push_back(bidGap1);
    features.push_back(sizeRatio);
    features.push_back(std::log(orderSizeUSD + 1e-9));

    return features;
}
