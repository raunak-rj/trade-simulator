#include "logisticRegressor.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <random>
#include <numeric>
LogisticRegressor::LogisticRegressor(size_t featureCount, int epochs, double learningRate) : epochs(epochs), learningRate(learningRate)
{
    weights.resize(featureCount, 0.0);
    means.resize(featureCount, 0.0);
    stds.resize(featureCount, 1.0);
    std::default_random_engine generator;
    std::uniform_real_distribution<double> dist(-0.01, 0.01);
    for (auto &w : weights)
    {
        w = dist(generator);
    }
}

double LogisticRegressor::sigmoid(double z) const
{
    return 1.0 / (1.0 + std::exp(-z));
}

double LogisticRegressor::dot(const std::vector<double> &a, const std::vector<double> &b) const
{
    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
        result += a[i] * b[i];
    return result;
}

double LogisticRegressor::predictProbability(const std::vector<double> &features) const
{
    std::vector<double> normalizedfeatures(features.size());
    normalizedfeatures[0]=features[0];
    for (size_t j = 1; j < features.size(); ++j)
    {
        normalizedfeatures[j] = (features[j] - means[j]) / stds[j];
    }
    return sigmoid(dot(weights, normalizedfeatures));
}

int LogisticRegressor::predictClass(const std::vector<double> &features) const
{
    return sigmoid(dot(weights,features)) >= 0.5 ? 1 : 0;
}

void LogisticRegressor::updateWeights(const std::vector<double> &gradient, double lr, size_t batchSize)
{
    for (size_t j = 0; j < weights.size(); ++j)
        weights[j] -= lr * gradient[j] / static_cast<double>(batchSize);
}

void LogisticRegressor ::normalizeFeatures(std::vector<std::vector<double>> &X, bool skipFirstColumn)
{
    if (X.empty() || X[0].empty())
        return;

    size_t numSamples = X.size();
    size_t numFeatures = X[0].size();
    size_t startIdx = skipFirstColumn ? 1 : 0;

    // Compute means
    for (size_t j = startIdx; j < numFeatures; ++j)
    {
        for (size_t i = 0; i < numSamples; ++i)
        {
            means[j] += X[i][j];
        }
        means[j] /= numSamples;
    }

    // Compute standard deviations
    for (size_t j = startIdx; j < numFeatures; ++j)
    {
        double sumSq = 0.0;
        for (size_t i = 0; i < numSamples; ++i)
        {
            double diff = X[i][j] - means[j];
            sumSq += diff * diff;
        }
        stds[j] = std::sqrt(sumSq / numSamples);
        if (stds[j] < 1e-8)
            stds[j] = 1.0; // avoid divide-by-zero
    }

    // Normalize
    for (size_t i = 0; i < numSamples; ++i)
    {
        for (size_t j = startIdx; j < numFeatures; ++j)
        {
            X[i][j] = (X[i][j] - means[j]) / stds[j];
        }
    }
}

bool LogisticRegressor::train(std::vector<std::vector<double>> &X, const std::vector<double> &y)
{
    if (X.empty() || X[0].size() != weights.size() || X.size() != y.size())
    {
        std::cerr << "Training data mismatch.\n";
        return false;
    }
    normalizeFeatures(X, true);
    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        std::vector<double> gradient(weights.size(), 0.0);

        for (size_t i = 0; i < X.size(); ++i)
        {
            double z = sigmoid(dot(weights,X[i]));
            double error = z - y[i];

            for (size_t j = 0; j < weights.size(); ++j)
                gradient[j] += error * X[i][j];
        }

        for (size_t j = 0; j < weights.size(); ++j)
            weights[j] -= ((learningRate * gradient[j]) / X.size());

        if (epoch % (epochs / 10) == 0)
        {
            int correct = 0;
            for (size_t i = 0; i < X.size(); ++i)
            {
                int pred = predictClass(X[i]);
                if (pred == static_cast<int>(y[i]))
                    ++correct;
            }
            double acc = static_cast<double>(correct) / X.size();
            std::cout << "Epoch " << epoch << " | Accuracy: " << acc << "\n";
        }
    }
    return true;
}

void LogisticRegressor::saveWeights(const std::string &filename) const
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "Failed to open weight file for writing\n";
        return;
    }
    out << "# Weights\n";
    for (const auto &w : weights)
        out << w << "\n";

    out << "# Means\n";
    for (const auto &mu : means)
        out << mu << "\n";

    out << "# Stds\n";
    for (const auto &sigma : stds)
        out << sigma << "\n";

    out.close();
}

void LogisticRegressor::loadWeights(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "Failed to open weight file for reading\n";
        return;
    }

    weights.clear();
    means.clear();
    stds.clear();

    std::string line;
    enum Section
    {
        NONE,
        WEIGHT,
        MEAN,
        STD
    } current = NONE;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        if (line.find("#") == 0)
        {
            if (line.find("Weights") != std::string::npos)
                current = WEIGHT;
            else if (line.find("Means") != std::string::npos)
                current = MEAN;
            else if (line.find("Stds") != std::string::npos)
                current = STD;
            continue;
        }

        double value = std::stod(line);
        if (current == WEIGHT)
            weights.push_back(value);
        else if (current == MEAN)
            means.push_back(value);
        else if (current == STD)
            stds.push_back(value);
    }

    in.close();
}
