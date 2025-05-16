#ifndef MAKER_TAKER_LABELER_H
#define MAKER_TAKER_LABELER_H

#include <vector>
#include <string>
#include <random>
#include "L2Snapshot.h"

class makerTakerLabeler
{
public:
    makerTakerLabeler(double minOffset = 0.01, double maxOffset = 0.05);
    // Extract features and label for buy or sell
    std::pair<std::vector<double>, int> labelSnapshot(const L2Snapshot &snap, double orderSizeUSD, bool isBuy);
    double calculateFee(const int level, bool isMaker, const double quantity);

private:
    std::default_random_engine rng;
    std::uniform_real_distribution<double> offsetDist;
    std::vector<double> makerTierPrice = {0.08, 0.075, 0.070, 0.065, 0.06}, takerTierPrice = {0.1, 0.09, 0.08, 0.07, 0.06};
    double computeDepth(const std::vector<OrderLevel> &levels, int depth);
    std::vector<double> extractFeatures(const L2Snapshot &snap, double limitPrice, double orderSizeUSD, bool isBuy);
    int computeLabel(double limitPrice, double bestBid, double bestAsk, bool isBuy);
};

#endif
