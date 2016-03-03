#include <common.h>

#define _TIMESPEC_DEFINED 1
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>

//-----------------------------------------------------------------------------
// PThread wrappers
//-----------------------------------------------------------------------------

static inline void PTHREAD_CREATE(pthread_t *const tid,	const pthread_attr_t *const attr, void *(PTW32_CDECL *start)(void *), void *const arg)
{
	if (pthread_create(tid, attr, start, arg))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void PTHREAD_JOIN(const pthread_t thread, void **const value_ptr)
{
	if (pthread_join(thread, value_ptr))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void MUTEX_INIT(pthread_mutex_t *const mtx)
{
	if (pthread_mutex_init(mtx, NULL))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void MUTEX_DESTROY(pthread_mutex_t *const mtx)
{
	if (pthread_mutex_destroy(mtx))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void MUTEX_LOCK(pthread_mutex_t *const mtx)
{
	if (pthread_mutex_lock(mtx))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void MUTEX_UNLOCK(pthread_mutex_t *const mtx)
{
	if (pthread_mutex_unlock(mtx))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void SEM_INIT(sem_t *const sema)
{
	if (sem_init(sema, 0, 0))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void SEM_DESTROY(sem_t *const sema)
{
	if (sem_destroy(sema))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline void SEM_POST(sem_t *const sema, const int n)
{
	if (sem_post_multiple(sema, n))
	{
		crit_exit("FATAL: PThread internal error!");
	}
}

static inline int SEM_TRYWAIT(sem_t *const sema)
{
	if (sem_trywait(sema))
	{
		if ((errno != 0) && (errno != EAGAIN))
		{
			fprintf(stderr, "ERRNO: %d", errno);
			crit_exit("FATAL: PThread internal error!");
		}
		return 0;
	}
	return 1;
}
