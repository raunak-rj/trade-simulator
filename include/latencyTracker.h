#ifndef LATENCY_TRACKER_H
#define LATENCY_TRACKER_H

#include <chrono>

using Clock = std::chrono::high_resolution_clock;

extern std::chrono::time_point<Clock> tickTime;
extern std::chrono::time_point<Clock> snapshotStartTime;
extern std::chrono::time_point<Clock> featureEndTime;
extern std::chrono::time_point<Clock> uiStartTime;

#endif
