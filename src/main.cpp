#include <QApplication>
#include "mainwindow.h"
#include "webSocketClient.h"
#include <string>
#include <iostream>
#include "json/json.hpp"
#include <fstream>
#include "snapshotHandler.h"
#include "trainSlippageModel.h"
#include "trainLogistic.h"
#include "almgrenEstimator.h"
#include "almgrenChrissModel.h"
#include "slippageRegressor.h"
#include "logisticRegressor.h"
#include "featureExtractor.h"
#include "L2Snapshot.h"
#include "latencyTracker.h"
#include "makerTakerLabel.h"
#include <QMetaObject>
#include <mutex>

using json = nlohmann::json;
L2Snapshot latestSnapshot;
std::mutex snapshotMutex;
MainWindow *globalWindow = nullptr;
webSocketClient ws;
std::chrono::time_point<Clock> tickTime;
std::chrono::time_point<Clock> snapshotStartTime;
std::chrono::time_point<Clock> featureEndTime;
std::chrono::time_point<Clock> uiStartTime;
void processTick(const std::string &payload)
{
    if (!globalWindow)
        return;

    try
    {
        tickTime = Clock::now();
        json root = json::parse(payload);
        snapshotStartTime = Clock::now();
        L2Snapshot snap;

        for (const auto &ask : root["asks"])
            snap.asks.push_back({std::stod(ask[0].get<std::string>()), std::stod(ask[1].get<std::string>())});

        for (const auto &bid : root["bids"])
            snap.bids.push_back({std::stod(bid[0].get<std::string>()), std::stod(bid[1].get<std::string>())});

        {
            std::lock_guard<std::mutex> lock(snapshotMutex);
            latestSnapshot = snap;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Tick parse error: " << e.what() << "\n";
    }
}

int main(int argc, char *argv[])
{
    std::string jsonFile = "data/l2_orderbook.json";
    std::string slippageData = "data/slippage_training.csv";
    std::string makerData = "data/maker_taker_training.csv";
    std::string weightOutput = "data/slippage_weights.txt";
    std::string makerWeightOutput = "data/maker_taker_weights.txt";
    double simulatedOrderSize = 10000000.0;
    snapshotHandler::processSnapshots(jsonFile, slippageData, makerData, simulatedOrderSize);
    // Train Slippage Model
    try
    {
        std::cout << "Training slippage regression model/n";
        trainSlippageModel(slippageData, weightOutput, 9, 0.01, 100);
        std::cout << "Model training completed. Weights saved to: " << weightOutput << "/n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error during training: " << e.what() << "/n";
        return 1;
    }

    // Train MakerTaker Model
    try
    {
        std::cout << "Training makerTaker model/n";
        trainMakerTakerModel(makerData, makerWeightOutput, 10, 0.01, 100);
        std::cout << "Model training completed. Weights saved to: " << weightOutput << "/n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error during training: " << e.what() << "/n";
        return 1;
    }
    std::cout << "[INFO] App starting" << std::endl;
    QApplication app(argc, argv);
    MainWindow w;
    globalWindow = &w;
    w.resize(600, 250);
    w.show();
    std::cout << "[INFO] Show called" << std::endl;
    

    std::thread wsThread([]()
                         {
        ws.setMessageCallback(processTick);
        ws.enableLatencyLogging(true);
        ws.connect("wss://ws.gomarket-cpp.goquant.io/ws/l2-orderbook/okx/BTC-USDT-SWAP"); });
    wsThread.detach();
    return app.exec();
}
