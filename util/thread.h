#ifndef THREAD_H
#define THREAD_H

#include <iostream>
#include <SDL_thread.h> 
#include "log.h"

/* Typedef for thread function */
using sc_thread_fn = int(*)(void*);

/* Structure for thread */
struct sc_thread {
    SDL_Thread* thread;
};

/* Structure for mutex */
struct sc_mutex {
    SDL_mutex* mutex;
};

/* Structure for condition variable */
struct sc_cond {
    SDL_cond* cond;
};

/* Enum for thread priority */
enum class sc_thread_priority {
    LOW,
    NORMAL,
    HIGH,
    TIME_CRITICAL
};

bool sc_thread_create(sc_thread* thread, sc_thread_fn fn, const char* name, void* userdata);

bool sc_thread_set_priority(sc_thread_priority priority);

void sc_thread_join(sc_thread* thread, int* status);

bool sc_mutex_init(sc_mutex* mutex);

void sc_mutex_destroy(sc_mutex* mutex);

void sc_mutex_lock(sc_mutex* mutex);

void sc_mutex_unlock(sc_mutex* mutex);

bool sc_cond_init(sc_cond* cond);

void sc_cond_destroy(sc_cond* cond);

void sc_cond_wait(sc_cond* cond, sc_mutex* mutex);

bool sc_cond_timedwait(sc_cond* cond, sc_mutex* mutex, uint32_t ms);

void sc_cond_signal(sc_cond* cond);

void sc_cond_broadcast(sc_cond* cond);

#endif  // THREAD_H
