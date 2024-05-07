#include "thread.h"

#include <cassert>
#include <cstring>
#include <SDL.h>

#include "log.h"

bool sc_thread_create(sc_thread* thread, sc_thread_fn fn, const char* name,
    void* userdata) {
    // The thread name length is limited on some systems. Never use a name
    // longer than 16 bytes (including the final '\0')
    assert(std::strlen(name) <= 15);

    SDL_Thread* sdl_thread = SDL_CreateThread(fn, name, userdata);
    if (!sdl_thread) {
        LOG_OOM();
        return false;
    }

    thread->thread = sdl_thread;
    return true;
}

static SDL_ThreadPriority
to_sdl_thread_priority(enum sc_thread_priority priority) {
    switch (priority) {
    case sc_thread_priority::TIME_CRITICAL:
#ifdef SCRCPY_SDL_HAS_THREAD_PRIORITY_TIME_CRITICAL
        return SDL_THREAD_PRIORITY_TIME_CRITICAL;
#else
        // fall through
#endif
    case sc_thread_priority::HIGH:
        return SDL_THREAD_PRIORITY_HIGH;
    case sc_thread_priority::NORMAL:
        return SDL_THREAD_PRIORITY_NORMAL;
    case sc_thread_priority::LOW:
        return SDL_THREAD_PRIORITY_LOW;
    default:
        assert(!"Unknown thread priority");
        return SDL_THREAD_PRIORITY_LOW;
    }
}

bool sc_thread_set_priority(sc_thread_priority priority) {
    SDL_ThreadPriority sdl_priority = to_sdl_thread_priority(priority);
    int r = SDL_SetThreadPriority(sdl_priority);
    if (r) {
        LOGD("Could not set thread priority: %s", SDL_GetError());
        return false;
    }

    return true;
}

void sc_thread_join(sc_thread* thread, int* status) {
    SDL_WaitThread(thread->thread, status);
}

bool sc_mutex_init(sc_mutex* mutex) {
    mutex->mutex = SDL_CreateMutex();
    if (!mutex->mutex) {
        LOG_OOM();
        return false;
    }
    return true;
}

void sc_mutex_destroy(sc_mutex* mutex) {
    SDL_DestroyMutex(mutex->mutex);
}

void sc_mutex_lock(sc_mutex* mutex) {
    SDL_LockMutex(mutex->mutex);
}

void sc_mutex_unlock(sc_mutex* mutex) {
    SDL_UnlockMutex(mutex->mutex);
}

bool sc_cond_init(sc_cond* cond) {
    cond->cond = SDL_CreateCond();
    if (!cond->cond) {
        LOG_OOM();
        return false;
    }
    return true;
}

void sc_cond_destroy(sc_cond* cond) {
    SDL_DestroyCond(cond->cond);
}

void sc_cond_wait(sc_cond* cond, sc_mutex* mutex) {
    SDL_CondWait(cond->cond, mutex->mutex);
}

bool sc_cond_timedwait(sc_cond* cond, sc_mutex* mutex, uint32_t ms) {
    return SDL_CondWaitTimeout(cond->cond, mutex->mutex, ms) == 0;
}

void sc_cond_signal(sc_cond* cond) {
    SDL_CondSignal(cond->cond);
}

void sc_cond_broadcast(sc_cond* cond) {
    SDL_CondBroadcast(cond->cond);
}
