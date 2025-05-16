#include "labelOrderbook.h"

double labelOrderbook::simulateSlippage(const std::vector<OrderLevel> &asks, double targetSizeUSD, double midPrice)
{
    double totalCost = 0.0;
    double filled = 0.0;
    double volume = 0.0;
    double best_ask = asks[0].price;
    for (const auto &level : asks)
    {
        double price = level.price;
        double sizeUSD = price * level.size;

        if (filled + sizeUSD >= targetSizeUSD)
        {
            double remaining = targetSizeUSD - filled;
            totalCost += price * (remaining / price);
            volume += (remaining / price);
            break;
        }

        totalCost += sizeUSD;
        filled += sizeUSD;
        volume += level.size;
    }
    double vwap=totalCost / volume ;
    return ((vwap-midPrice)/midPrice)*targetSizeUSD;
}

