#include <time.h>

#include "vbase.h"

uint64_t natime() {
  uint64_t time, freq;
  QueryPerformanceCounter(&time);
  QueryPerformanceFrequency(&freq);
  return (uint64_t) (time.QuadPart / (double) freq.QuadPart * 1000000);
}

