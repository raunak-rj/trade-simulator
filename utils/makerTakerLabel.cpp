#include "makerTakerLabel.h"
#include <cmath>
#include <algorithm>
makerTakerLabeler::makerTakerLabeler(double minOffset, double maxOffset)
    : offsetDist(minOffset, maxOffset), rng(std::random_device{}()) {}

double makerTakerLabeler::computeDepth(const std::vector<OrderLevel> &levels, int depth)
{
    double sum = 0.0;
    for (int i = 0; i < std::min(depth, static_cast<int>(levels.size())); ++i)
    {
        sum += levels[i].price * levels[i].size;
    }
    return sum;
}

std::vector<double> makerTakerLabeler::extractFeatures(const L2Snapshot &snap, double limitPrice, double orderSizeUSD, bool isBuy)
{
    std::vector<double> features;
    double bestAsk = snap.asks[0].price;
    double bestBid = snap.bids[0].price;

    double spread = bestAsk - bestBid;
    double midPrice = (bestAsk + bestBid) / 2.0;
    double distanceToQuote = isBuy ? (limitPrice - bestAsk) : (bestBid - limitPrice);
    double logOrderSize = std::log(orderSizeUSD + 1e-9);
    double depthBid = 0.0, depthAsk = 0.0;
    int N = std::min(5, std::min((int)snap.bids.size(), (int)snap.asks.size()));
    for (int i = 0; i < N; ++i) {
        depthBid += snap.bids[i].size;
        depthAsk += snap.asks[i].size;
    }

    double imbalance = (depthBid - depthAsk) / (depthBid + depthAsk + 1e-9);
    double askGap1 = snap.asks.size() > 1 ? snap.asks[1].price - snap.asks[0].price : 0;
    double bidGap1 = snap.bids.size() > 1 ? snap.bids[0].price - snap.bids[1].price : 0;
    double sizeRatio = orderSizeUSD / (depthAsk * bestAsk + 1e-9);
    // Features
    features.push_back(1.0); // Bias
    features.push_back(spread);
    features.push_back(distanceToQuote);
    features.push_back(depthBid);
    features.push_back(depthAsk);
    features.push_back(askGap1);
    features.push_back(bidGap1);
    features.push_back(imbalance);
    features.push_back(sizeRatio);
    features.push_back(logOrderSize);

    return features;
}

int makerTakerLabeler::computeLabel(double limitPrice, double bestBid, double bestAsk, bool isBuy)
{
    if (isBuy)
        return limitPrice >= bestAsk ? 1 : 0;
    else
        return limitPrice <= bestBid ? 1 : 0;
}

std::pair<std::vector<double>, int> makerTakerLabeler::labelSnapshot(const L2Snapshot &snap, double orderSizeUSD, bool isBuy)
{
    // Skip malformed snapshot
    if (snap.asks.empty() || snap.bids.empty())
    {
        return {{}, -1}; 
    }
    double delta = 0.1;
    std::uniform_real_distribution<double> dist(snap.bids[0].price - delta, snap.asks[0].price + delta);
    double limitPrice = dist(rng);
    auto features = extractFeatures(snap, limitPrice, orderSizeUSD, isBuy);
    int label = computeLabel(limitPrice, snap.bids[0].price, snap.asks[0].price, isBuy);
    return {features, label};
}

double makerTakerLabeler::calculateFee(const int tier, bool isMaker, const double quantity)
{
    double rate=isMaker?makerTierPrice[tier]:takerTierPrice[tier];
    return rate*0.01*quantity;
}