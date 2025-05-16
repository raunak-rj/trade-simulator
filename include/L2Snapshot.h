#ifndef L2SNAPSHOT_H
#define L2SNAPSHOT_H
#include <vector>
#include <utility>
#include <fstream>
#include <cmath>


struct OrderLevel
{
    double price;
    double size;
};

struct L2Snapshot
{
    double timestamp;
    std::vector<OrderLevel> bids;
    std::vector<OrderLevel> asks;
};
#endif