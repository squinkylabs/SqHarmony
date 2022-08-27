
// just to get tests so compile in MS

struct pthread_rwlock_t {};

struct pthread_rwlockattr_t {};

inline int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
    return 0;
}

inline int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
    return 0;
}

inline int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
    return 0;
}

inline int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
    return 0;
}

inline int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) { return 0; }

inline int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) { return 0; }
//inline int pthread_rwlock_wrlock	(	pthread_rwlock_t * 	rwlock	){ return 0; }

inline int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock)  { return 0; }