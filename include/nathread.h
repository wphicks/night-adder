#pragma once


#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <pthread.h>
#define thread_t pthread_t *

#else
#include <windows.h>
#include <process.h>
#define thread_t uintptr_t

#endif

thread_t create_thread(void function(void *), void * args);

void join_thread(thread_t other_thread);

void detach_thread(thread_t other_thread);

void cleanup_thread(thread_t other_thread);
