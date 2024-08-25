#if defined(__linux__)
// For Linux, we need to implement our own our_gettimeofday() with clock_gettime()
#include <sys/time.h>
#include <time.h>
extern int gettimeofday(struct timeval * __restrict tp, void * /*tz*/) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    return -1;
  }
  tp->tv_sec = ts.tv_sec;
  tp->tv_usec = ts.tv_nsec / 1000;
  return 0;
}
#endif // __linux__
