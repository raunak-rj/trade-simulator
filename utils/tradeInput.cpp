#include <bits/stdc++.h>
using namespace std;
class tradeInput
{
    private:
        string exchange;
        string spotAsset;
        string orderType;
        double quantity;
        double volatility;
        double feeTier;

    public:
        tradeInput(string exch, string asset, string type, double qty, double vol, double fee)
            : exchange(exch), spotAsset(asset), orderType(type), quantity(qty), volatility(vol), feeTier(fee) {}

        string getExchange() const { return exchange; }
        string getSpotAsset() const { return spotAsset; }
        string getOrderType() const { return orderType; }
        double getQuantity() const { return quantity; }
        double getVolatility() const { return volatility; }
        double getFeeTier() const { return feeTier; }

        void setExchange(const string &exch) { exchange = exch; }
        void setSpotAsset(const string &asset) { spotAsset = asset; }
        void setOrderType(const string &type) { orderType = type; }
        void setQuantity(double qty) { quantity = qty; }
        void setVolatility(double vol) { volatility = vol; }
        void setFeeTier(double fee) { feeTier = fee; }
};