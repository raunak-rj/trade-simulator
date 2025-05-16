#ifndef TRADE_SIMULATOR_H
#define TRADE_SIMULATOR_H

#include "L2Snapshot.h"

class labelOrderbook
{
    public:
        // Returns simulated slippage in USD for a market buy
        static double simulateSlippage(const std::vector<OrderLevel> &asks, double usdQty, double mmidPrice);
};

#endif
