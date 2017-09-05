#include <pthread.h>
#include "nathread.h"
thread_t create_thread(void function(void *), void * args) {
  thread_t new_thread = malloc(sizeof(pthread_t));
  pthread_create(new_thread, NULL, function, args);
  return new_thread;
}

void join_thread(thread_t other_thread) {
  pthread_join(other_thread, NULL);
}

void detach_thread(thread_t other_thread) {
  pthread_detach(other_thread);
}

void cleanup_thread(thread_t other_thread) {
  free(other_thread);
}
