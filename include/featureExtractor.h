#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H
#include "L2Snapshot.h"
class featureExtractor
{
    public:
        static std::vector<double> extractFeatures(const L2Snapshot &snapshot, double orderSizeUSD);
};

#endif
