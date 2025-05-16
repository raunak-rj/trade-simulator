#ifndef TRAIN_H
#define TRAIN_H

#include <string>

void trainSlippageModel(const std::string &csvPath, const std::string &weightsOutputPath, size_t featureCount, double learningRate = 0.001, int epochs = 10);

#endif
