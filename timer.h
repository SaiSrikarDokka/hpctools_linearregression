#include <time.h>
#include <stdint.h>

static inline void timestamp(struct timespec *ts)
{
  __asm__ volatile("" ::: "memory");
  clock_gettime(CLOCK_MONOTONIC, ts);
  __asm__ volatile("" ::: "memory");
}

static inline int64_t timespec_to_ns(const struct timespec *t)
{
  return (int64_t)t->tv_sec * 1000000000LL + t->tv_nsec;
}

static inline int64_t diff_nano(const struct timespec *start,
                                const struct timespec *end)
{
  return timespec_to_ns(end) - timespec_to_ns(start); // ns
}

static inline double diff_micro(const struct timespec *start,
                                const struct timespec *end)
{
  return diff_nano(start, end) * 1e-3; // us
}

static inline double diff_milli(const struct timespec *start,
                                const struct timespec *end)
{
  return diff_nano(start, end) * 1e-6; // ms
}

static inline double diff_seconds(const struct timespec *start,
                                  const struct timespec *end)
{
  return diff_nano(start, end) * 1e-9; // s
}
