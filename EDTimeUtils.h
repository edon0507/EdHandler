#ifndef ED_TIME_UTILS_H_
#define ED_TIME_UTILS_H_
#include <chrono>
#include "Common.h"
/**
 * current time in milliseconds
 * */
uint64 NowUs();

uint64 NowS();

std::string NowDate();
/**
 * milliseconds to time point
 * */
std::chrono::time_point<std::chrono::system_clock>
  ClockTimeFromMilliseconds(int64_t ms);

std::chrono::time_point<std::chrono::system_clock>
  ClockTimeFromMicroseconds(int64_t us);
// void uSleep(int64 usec);

#endif