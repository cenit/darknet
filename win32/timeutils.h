#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>
#include <stdint.h>
#include <time.h>

#define CLOCK_REALTIME (1)
#define BILLION (1E9)

#ifndef timersub
#define timersub(a, b, result)                       \
  do {                                               \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;    \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
    if ((result)->tv_usec < 0) {                     \
      --(result)->tv_sec;                            \
      (result)->tv_usec += 1000000;                  \
    }                                                \
  } while (0)
#endif // timersub

static unsigned char g_first_time = 1;
static long long int g_counts_per_sec;

int gettimeofday(struct timeval*, struct timezone*);
int clock_gettime(int, struct timespec*);
