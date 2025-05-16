#include "snapshotHandler.h"
#include "featureExtractor.h"
#include "labelOrderbook.h"
#include "makerTakerLabel.h"
#include "json/json.hpp"
#include <fstream>
#include <vector>
#include <iostream>

using json = nlohmann::json;

bool snapshotHandler::processSnapshots(const std::string &jsonFilePath, const std::string &slippageOutputCSV, const std::string &makerTakerOutputCSV, double orderSizeUSD)
{
    std::ifstream in(jsonFilePath);
    if (!in.is_open())
    {
        std::cerr << "Failed to open JSON snapshot file\n";
        std::cerr << "Error: " << std::strerror(errno) << "\n";
        return false;
    }
    std::ofstream slipOut(slippageOutputCSV);
    std::ofstream takerOut(makerTakerOutputCSV);
    std::string line;
    while (std::getline(in, line))
    {
        try
        {
            json j = json::parse(line);
            L2Snapshot snap;

            for (auto &bid : j["bids"])
                snap.bids.push_back({std::stod(bid[0].get<std::string>()), std::stod(bid[1].get<std::string>())});

            for (auto &ask : j["asks"])
                snap.asks.push_back({std::stod(ask[0].get<std::string>()), std::stod(ask[1].get<std::string>())});

            // Skip empty books
            if (snap.bids.empty() || snap.asks.empty())
                continue;

            // Feature Extraction
            auto slippageFeatures = featureExtractor::extractFeatures(snap, orderSizeUSD);
            double midPrice = (snap.asks[0].price + snap.bids[0].price) / 2;

            // Slippage Label
            double slippage = labelOrderbook::simulateSlippage(snap.asks, orderSizeUSD, midPrice);
            for (const auto &f : slippageFeatures)
                slipOut << f << ",";
            slipOut << slippage << "\n";

            // Maker/Taker Label
            makerTakerLabeler labeler;
            auto [feature, label] = labeler.labelSnapshot(snap, orderSizeUSD, true);
            for (const auto &f : feature)
                takerOut << f << ",";
            takerOut << label << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing JSON: " << e.what() << "\n";
            continue;
        }
    }

    in.close();
    slipOut.close();
    takerOut.close();
    std::cout << " Feature-label files created: \n"
              << "   Slippage: " << slippageOutputCSV << "\n"
              << "   Maker/Taker: " << makerTakerOutputCSV << "\n";
    return true;
}
