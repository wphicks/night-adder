#include <stdlib.h>
#include <pthread.h>
#include "nathread.h"

thread_t create_thread(void function(void *), void * args) {
  thread_t new_thread = malloc(sizeof(pthread_t));
  pthread_create(new_thread, NULL, (void*(*)(void *)) function, args);
  return new_thread;
}

void init_thread(thread_t * cur_thread, void function(void *), void * args) {
  *cur_thread = malloc(sizeof(pthread_t));
  pthread_create(*cur_thread, NULL, (void*(*)(void *)) function, args);
}

void join_thread(thread_t other_thread) {
  pthread_join(*other_thread, NULL);
}

void detach_thread(thread_t other_thread) {
  pthread_detach(*other_thread);
}

void cleanup_thread(thread_t other_thread) {
  free(other_thread);
}
