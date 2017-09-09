#include <sys/time.h>

#include "vbase.h"

uint64_t natime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
