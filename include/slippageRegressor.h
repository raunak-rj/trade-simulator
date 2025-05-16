#ifndef SLIPPAGEREGRESSOR_H
#define SLIPPAGEREGRESSOR_H

#include <vector>
#include <string>

class slippageRegressor
{
public:
    slippageRegressor(size_t featureCount, double learningRate = 0.01, int epochs = 1000);

    // Train using precomputed feature vectors and targets
    bool train(std::vector<std::vector<double>> &X,
               const std::vector<double> &y);

    // Predict from a feature vector
    double predict(std::vector<double> &features) const;

    // Load and save weights
    void saveWeights(const std::string &filename) const;
    void loadWeights(const std::string &filename);

private:
    std::vector<double> weights, means, stds;
    double learningRate;
    int epochs;

    double dot(const std::vector<double> &a, const std::vector<double> &b) const;
    void normalizeFeatures(std::vector<std::vector<double>> &X, bool skipFirstColumn = true);
};

#endif
