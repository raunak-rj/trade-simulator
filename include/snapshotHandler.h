#ifndef SNAPSHOT_HANDLER_H
#define SNAPSHOT_HANDLER_H

#include <string>
#include "L2Snapshot.h"

class snapshotHandler
{
    public:
        static bool processSnapshots(const std::string &jsonFilePath, const std::string &slippageOutputCSV, const std::string &makerTakerOutputCSV, double orderSizeUSD);
        
};

#endif
