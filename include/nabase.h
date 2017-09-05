#pragma once
#include "vbase.h"
/**
 * A union of a 64 bit int and a double, useful for atomic operations
 */
typedef union {
  uint64_t as_int;
  double as_double;
} atomic_double;
