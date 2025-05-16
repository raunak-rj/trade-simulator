#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include "L2Snapshot.h"
Q_DECLARE_METATYPE(L2Snapshot)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Getters (to be used in processTick)
    double getOrderQuantity() const;
    double getVolatility() const;
    double getFeeTier() const;
    QString getSelectedAsset() const;


public slots:
    void updateFromSnapshot(double slippage, double fee, double impact, double netCost, double makerProb, int latencyMs);

private slots:
    void onSimulateClicked();

private:
    // Input Components
    QComboBox *exchangeCombo;
    QComboBox *assetCombo;
    QComboBox *orderTypeCombo;
    QLineEdit *quantityEdit;
    QLineEdit *volatilityEdit;
    QComboBox *feeTierCombo;
    QPushButton *simulateBtn;

    // Output Labels
    QLabel *slippageVal;
    QLabel *feesVal;
    QLabel *impactVal;
    QLabel *netCostVal;
    QLabel *makerTakerVal;
    QLabel *latencyVal;

    void setupUI();
};

#endif // MAINWINDOW_H
