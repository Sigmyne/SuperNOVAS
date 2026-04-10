/**
 * @file
 *
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 *
 *  Portable mutex function mappings for NOVAS
 */

#ifndef NOVAS_MUTEX_H_
#define NOVAS_MUTEX_H_

/// \cond PRIVATE
#if defined(SUPERNOVAS_USE_THREAD) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
#  include <pthread.h>

#  define novas_init_lock     pthread_mutex_init
#  define novas_lock          pthread_mutex_lock
#  define novas_unlock        pthread_mutex_unlock
#  define novas_destroy_lock  pthread_mutex_destroy
#  define THREAD_SAFE         1

typedef pthread_mutex_t       lock_type;

#elif __STDC_VERSION__ >= 201112L
#  include <threads.h>

#  define novas_init_lock     mtx_init
#  define novas_lock          mtx_lock
#  define novas_unlock        mtx_unlock
#  define novas_destroy_lock  mtx_destroy

#  define THREAD_SAFE         1

typedef mtx_t                 lock_type;

#elif defined(WIN32)
#include <windows.h>

#  define novas_init_lock(x, t)     InitializeSRWLock(x)
#  define novas_lock          AcquireSRWLockExclusive
#  define novas_unlock        ReleaseSRWLockExclusive
#  define novas_destroy_lock  (void)
#  define THREAD_SAFE         1

typedef SRWLOCK               lock_type;

#else
#  define novas_lock(x)
#  define novas_unlock(x)
#  define novas_destroy_lock(x)
#  define THREAD_SAFE         0

typedef int                   lock_type;

#endif


#endif /* NOVAS_MUTEX_H_ */
