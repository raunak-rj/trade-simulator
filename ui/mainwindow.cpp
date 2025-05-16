#include "mainwindow.h"
#include "snapshotHandler.h"
#include "trainSlippageModel.h"
#include "trainLogistic.h"
#include "almgrenEstimator.h"
#include "almgrenChrissModel.h"
#include "slippageRegressor.h"
#include "logisticRegressor.h"
#include "featureExtractor.h"
#include "L2Snapshot.h"
#include "makerTakerLabel.h"
#include "latencyTracker.h"
#include <QMessageBox>
#include <QElapsedTimer>
#include <mutex>
#include <iostream>
#include <QDebug>

extern L2Snapshot latestSnapshot;
extern std::mutex snapshotMutex;
QElapsedTimer timer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout *layout = new QGridLayout(centralWidget);

    // --- Left Panel: Input ---
    exchangeCombo = new QComboBox();
    exchangeCombo->addItem("OKX");

    assetCombo = new QComboBox();
    assetCombo->addItems({"BTC-USDT", "ETH-USDT", "XRP-USDT"});

    orderTypeCombo = new QComboBox();
    orderTypeCombo->addItem("Market");

    feeTierCombo = new QComboBox();
    feeTierCombo->addItems({"Tier 1","Tier 2","Tier 3","Tier 4","Tier 5"});

    quantityEdit = new QLineEdit("100");

    volatilityEdit = new QLineEdit("0.025");

    

    simulateBtn = new QPushButton("Simulate Trade");
    connect(simulateBtn, &QPushButton::clicked, this, &MainWindow::onSimulateClicked);

    layout->addWidget(new QLabel("<b>Exchange:</b>"), 0, 0);
    layout->addWidget(exchangeCombo, 0, 1);
    layout->addWidget(new QLabel("<b>Asset:</b>"), 1, 0);
    layout->addWidget(assetCombo, 1, 1);
    layout->addWidget(new QLabel("<b>Order Type:</b>"), 2, 0);
    layout->addWidget(orderTypeCombo, 2, 1);
    layout->addWidget(new QLabel("<b>Quantity (USD):</b>"), 3, 0);
    layout->addWidget(quantityEdit, 3, 1);
    layout->addWidget(new QLabel("<b>Volatility:</b>"), 4, 0);
    layout->addWidget(volatilityEdit, 4, 1);
    layout->addWidget(new QLabel("<b>Fee Tier:</b>"), 5, 0);
    layout->addWidget(feeTierCombo, 5, 1);
    layout->addWidget(simulateBtn, 6, 0, 1, 2);

    // --- Right Panel: Output ---
    slippageVal = new QLabel("-");
    feesVal = new QLabel("-");
    impactVal = new QLabel("-");
    netCostVal = new QLabel("-");
    makerTakerVal = new QLabel("-");
    latencyVal = new QLabel("-");

    layout->addWidget(new QLabel("<b>Expected Slippage:</b>"), 0, 2);
    layout->addWidget(slippageVal, 0, 3);
    layout->addWidget(new QLabel("<b>Expected Fees:</b>"), 1, 2);
    layout->addWidget(feesVal, 1, 3);
    layout->addWidget(new QLabel("<b>Market Impact:</b>"), 2, 2);
    layout->addWidget(impactVal, 2, 3);
    layout->addWidget(new QLabel("<b>Net Cost:</b>"), 3, 2);
    layout->addWidget(netCostVal, 3, 3);
    layout->addWidget(new QLabel("<b>Maker/Taker %:</b>"), 4, 2);
    layout->addWidget(makerTakerVal, 4, 3);
    layout->addWidget(new QLabel("<b>Latency (µs):</b>"), 5, 2);
    layout->addWidget(latencyVal, 5, 3);

    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(3, 1);
    layout->setHorizontalSpacing(20);
    layout->setVerticalSpacing(15);
}

void MainWindow::onSimulateClicked()
{
    timer.restart();

    L2Snapshot snap;
    {
        std::lock_guard<std::mutex> lock(snapshotMutex);
        snap = latestSnapshot;
    }

    // Check if the snapshot is valid
    if (snap.bids.empty() || snap.asks.empty())
    {
        QMessageBox::warning(this, "Error", "No market data available yet.");
        return;
    }
    double quantity = getOrderQuantity();
    double volatility = getVolatility();
    double tier = getFeeTier();
    double dailyVolumeUSD = 2e6;

    // Slippage model
    slippageRegressor slippageModel(9);
    slippageModel.loadWeights("data/slippage_weights.txt");
    auto slippageFeatures = featureExtractor::extractFeatures(snap, quantity);

    // Maker/taker model
    LogisticRegressor makerModel(10);
    makerModel.loadWeights("data/maker_taker_weights.txt");
    makerTakerLabeler labeler;
    auto [feature, label] = labeler.labelSnapshot(snap, quantity, true);

    featureEndTime = Clock::now();

    double slippage = slippageModel.predict(slippageFeatures);
    double makerProb = makerModel.predictProbability(feature);

    // Almgren model
    almgrenParameterEstimator estimator;
    auto [eta, gamma] = estimator.estimate(snap, volatility, dailyVolumeUSD, quantity);
    almgrenChrissModel model(eta, gamma);
    double marketImpact = model.estimateMarketImpact(quantity, volatility, snap, true) * quantity;

    double fee = labeler.calculateFee(tier,makerProb>0.5?true:false,quantity);
    double netCost = fee + slippage + marketImpact;
    qint64 latency = timer.nsecsElapsed() / 1000;
    uiStartTime = Clock::now();
    updateFromSnapshot(slippage, fee, marketImpact, netCost, makerProb, latency);
    auto uiEnd = Clock::now();
    auto dataLatency = std::chrono::duration_cast<std::chrono::microseconds>(featureEndTime - snapshotStartTime).count();
    auto uiLatency = std::chrono::duration_cast<std::chrono::microseconds>(uiEnd - uiStartTime).count();
    auto endToEnd = std::chrono::duration_cast<std::chrono::microseconds>(uiEnd - tickTime).count();
    qDebug() << "Data Processing Latency:" << dataLatency << "µs";
    qDebug() << "UI Update Latency:" << uiLatency << "µs";
    qDebug() << "End-to-End Latency:" << endToEnd << "µs";
}

double MainWindow::getOrderQuantity() const
{
    return quantityEdit->text().toDouble();
}

double MainWindow::getVolatility() const
{
    return volatilityEdit->text().toDouble();
}

double MainWindow::getFeeTier() const
{
    return feeTierCombo->currentIndex();
}

QString MainWindow::getSelectedAsset() const
{
    return assetCombo->currentText();
}

// Result Updates 
void MainWindow::updateFromSnapshot(double slippage, double fee, double impact, double netCost, double makerProb, int latencyMicros)
{
    slippageVal->setText(QString("$%1").arg(slippage, 0, 'f', 4));
    feesVal->setText(QString("$%1").arg(fee, 0, 'f', 4));
    impactVal->setText(QString("$%1").arg(impact, 0, 'f', 4));
    netCostVal->setText(QString("$%1").arg(netCost, 0, 'f', 4));
    makerTakerVal->setText(QString("%1% Taker").arg(int((1.0 - makerProb) * 100)));
    latencyVal->setText(QString("%1 µs").arg(latencyMicros));
}
