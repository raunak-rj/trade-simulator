#ifndef TRAIN_LOGISTIC_H
#define TRAIN_LOGISTIC_H

#include <string>

void trainMakerTakerModel(const std::string& csvPath,
                          const std::string& weightsOutputPath,
                          size_t featureCount,
                          double learningRate = 0.01,
                          int epochs = 1000);

#endif
