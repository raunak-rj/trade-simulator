#include "trainLogistic.h"
#include "logisticRegressor.h"
#include <fstream>
#include <sstream>
#include <iostream>

void trainMakerTakerModel(const std::string &csvPath, const std::string &weightsOutputPath, size_t featureCount, double learningRate, int epochs)
{
    std::vector<std::vector<double>> X;
    std::vector<double> y;

    std::ifstream file(csvPath);
    if (!file.is_open())
        return ;

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<double> row;
        std::string val;

        while (std::getline(ss, val, ','))
            row.push_back(std::stod(val));

        if (row.size() != featureCount + 1)
            continue;

        X.emplace_back(row.begin(), row.end() - 1);
        y.push_back(row.back());
    }

    file.close();

    LogisticRegressor model(featureCount,epochs,learningRate);
    
    bool success = model.train(X, y);

    if (success) {
        model.saveWeights(weightsOutputPath);
        std::cout << "Model trained and weights saved to " << weightsOutputPath << "\n";
    }
}
