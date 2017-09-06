#include <windows.h>
#include <process.h>
#include "nathread.h"
thread_t create_thread(void function(void *), void * args) {
  return _beginthread(function, 0, args);
}

void init_thread(thread_t * cur_thread, void function(void *), void * args) {
  *cur_thread =  _beginthread(function, 0, args);
}

void join_thread(thread_t other_thread) {
  WaitForSingleObject(other_thread, INFINITE);
}

void detach_thread(thread_t other_thread) {
  CloseHandle(other_thread);
}

void cleanup_thread(thread_t other_thread) {
}

