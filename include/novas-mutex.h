/**
 * @file
 *
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 *
 *  Portable mutex function mappings for SuperNOVAS
 */

#ifndef NOVAS_MUTEX_H_
#define NOVAS_MUTEX_H_

/// \cond PROTECTED
#if defined(SUPERNOVAS_USE_PTHREAD) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
#  include <pthread.h>
#  include <stdlib.h>             // for NULL

#  define THREAD_SAFE             1

typedef pthread_mutex_t           lock_type;

#  define novas_init_lock(x)      pthread_mutex_init(x, NULL)
#  define novas_lock              pthread_mutex_lock
#  define novas_unlock            pthread_mutex_unlock
#  define novas_destroy_lock      pthread_mutex_destroy
#  define NOVAS_LOCK_INITIALIZER  PTHREAD_MUTEX_INITIALIZER

#elif defined(_MSC_VER)
#  include <windows.h>

#  define THREAD_SAFE             1
#  define NOVAS_LOCK_INITIALIZER  SRWLOCK_INIT

typedef SRWLOCK                   lock_type;

#  define novas_init_lock(x)      InitializeSRWLock(x)
#  define novas_lock              AcquireSRWLockExclusive
#  define novas_unlock            ReleaseSRWLockExclusive
#  define novas_destroy_lock(x)                           // no-op: SRWLOCK does not require explicit destruction

#elif __STDC_VERSION__ >= 201112L
#  include <threads.h>

#  define THREAD_SAFE             1

typedef mtx_t                     lock_type;

#  define novas_init_lock(x)      mtx_init(x, mtx_plain)
#  define novas_lock              mtx_lock
#  define novas_unlock            mtx_unlock
#  define novas_destroy_lock      mtx_destroy

#else

#  define THREAD_SAFE             0
#  define NOVAS_LOCK_INITIALIZER  (-1)

typedef int                       lock_type;

#  define novas_init_lock(x)
#  define novas_lock(x)
#  define novas_unlock(x)
#  define novas_destroy_lock(x)

#endif /* portable mutex definitions */
/// \endcond


#endif /* NOVAS_MUTEX_H_ */
