#include "trainSlippageModel.h"
#include "slippageRegressor.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

void trainSlippageModel(const std::string& csvPath, const std::string& weightsOutputPath, size_t featureCount, double learningRate, int epochs)
{
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << csvPath << "\n";
        return;
    }

    std::vector<std::vector<double>> X;
    std::vector<double> y;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<double> row;
        std::string val;

        while (std::getline(ss, val, ',')) {
            row.push_back(std::stod(val));
        }

        if (row.size() != featureCount + 1) {
            std::cerr << "Warning: skipping malformed line in CSV\n";
            continue;
        }

        X.emplace_back(row.begin(), row.end() - 1); 
        y.push_back(row.back());                   
    }

    file.close();

    slippageRegressor model(featureCount, learningRate, epochs);
    bool success = model.train(X, y);

    if (success) {
        model.saveWeights(weightsOutputPath);
        std::cout << "Model trained and weights saved to " << weightsOutputPath << "\n";
    }

}
