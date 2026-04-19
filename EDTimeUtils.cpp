#include "EDTimeUtils.h"
#include <thread>
#include <time.h>
#include <cmath>
#define NANOS_IN_SECOND 1000000000
using namespace std::chrono;

#if (TARGET_OS_WINDOWS)
uint64 NowUs() {
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	uint64 usec;
	usec = ((uint64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
	// Convert to Unix 1970 time: subtract 100ns units from 1/1/1601 to 1/1/1970.
	usec -= UINT64_C(0x019db1ded53e8000);
	usec /= 10;								// Convert 100ns units to microseconds.
	return usec;
}

//#elif TARGET_OS_ANDROID
//uint64 NowUs() {
////  struct timespec res = {0, 0};
////  clock_gettime(CLOCK_REALTIME, &res);
////  return (res.tv_sec * NANOS_IN_SECOND) + res.tv_nsec;
//  return duration_cast<microseconds>(
//    system_clock::now().time_since_epoch()).count();
//}
#else
uint64 NowUs() {
	return duration_cast<microseconds>(
	  system_clock::now().time_since_epoch()).count();
}

std::string NowDate() {
//  time_t t = time(0);
//  char tmp[32] = { NULL };
//  strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
//  sprintf(tmp, "%s:%03d",tmp,(int)millis);
//  string date(tmp);

//  using std::chrono::system_clock;
//  auto currentTime = std::chrono::system_clock::now();
//  char buffer[80];
//
//  auto transformed = currentTime.time_since_epoch().count() / 1000000;
//
//  auto millis = transformed % 1000;
//
//  std::time_t tt;
//  tt = system_clock::to_time_t ( currentTime );
//  auto timeinfo = localtime (&tt);
//  strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
//  sprintf(buffer, "%s:%03d",buffer,(int)millis);
  char buffer[40];
  int64 millisec;
  struct tm* tm_info;
  struct timeval tv;

  gettimeofday(&tv, NULL);

  millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
  if (millisec>=1000) { // Allow for rounding up to nearest second
    millisec -=1000;
    tv.tv_sec++;
  }

  tm_info = localtime(&tv.tv_sec);

  strftime(buffer, 40, "%Y-%m-%d %H:%M:%S", tm_info);
  sprintf(buffer, "%s.%03d", buffer, millisec);
  return buffer;
}

#endif

uint64 NowS() {
  return NowUs()/1000000;
}

time_point<system_clock> ClockTimeFromMilliseconds(int64_t ms) {
  return time_point<system_clock>(milliseconds(ms));
}

time_point<system_clock> ClockTimeFromMicroseconds(int64_t us) {
  return time_point<system_clock, microseconds>(microseconds(us));
}

void uSleep(int64 usec) {
#if (TARGET_OS_WINDOWS)
	std::this_thread::sleep_for(std::chrono::microseconds(usec));
#else
  usleep(usec);
#endif
}
