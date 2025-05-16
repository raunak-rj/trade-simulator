#ifndef LOGISTIC_REGRESSOR_H
#define LOGISTIC_REGRESSOR_H

#include <vector>
#include <string>

class LogisticRegressor {
public:
    LogisticRegressor(size_t featureCount,int epochs=10, double learningRate=0.01);

    double predictProbability(const std::vector<double>& features) const;
    int predictClass(const std::vector<double>& features) const;
    bool train( std::vector<std::vector<double>>& X, const std::vector<double>& y);
    void saveWeights(const std::string& filename) const;
    void loadWeights(const std::string& filename);
    void updateWeights(const std::vector<double>& gradient, double lr, size_t batchSize);
    

private:
    std::vector<double> weights, means , stds; 
    double learningRate;
    int epochs;
    double sigmoid(double z) const;
    double dot(const std::vector<double>& a, const std::vector<double>& b) const;
    void normalizeFeatures(std::vector<std::vector<double>>& X, bool skipFirstColumn = true);
};

#endif
