#include <bits/stdc++.h>
using namespace std;
class tradeOutput
{
    private:
        double slippage;
        double fees;
        double marketImpact;
        double netCost;
        double proportion;
        double latency;

    public:

        tradeOutput(double sl, double f, double mi, double prop, double lat)
            : slippage(sl), fees(f), marketImpact(mi), proportion(prop), latency(lat)
        {
            netCost = slippage + fees + marketImpact;
        }
        //Getter
        double getSlippage() const { return slippage; }
        double getFees() const { return fees; }
        double getMarketImpact() const { return marketImpact; }
        double getNetCost() const { return netCost; }
        double getProportion() const { return proportion; }
        double getLatency() const { return latency; }
        //Setter
        void setSlippage(double sl)
        {
            slippage = sl;
            updateNetCost();
        }
        void setFees(double f)
        {
            fees = f;
            updateNetCost();
        }
        void setMarketImpact(double mi)
        {
            marketImpact = mi;
            updateNetCost();
        }
        void setProportion(double prop) { proportion = prop; }
        void setLatency(double lat) { latency = lat; }

        void updateNetCost() { netCost = slippage + fees + marketImpact; }
};