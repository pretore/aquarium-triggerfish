#define _GNU_SOURCE
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <errno.h>
#include <dlfcn.h>

#include "cmocka.h"

bool pthread_mutex_init_is_overridden = false;
int cmocka_test_pthread_mutex_init(pthread_mutex_t *restrict mutex,
                                   const pthread_mutexattr_t *attr,
                                   const char *file, int line) {
    if (pthread_mutex_init_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_mutex_t *, const pthread_mutexattr_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_mutex_init");
    }
    return func(mutex, attr);
}

bool pthread_mutex_destroy_is_overridden = false;
int cmocka_test_pthread_mutex_destroy(pthread_mutex_t *mutex,
                                      const char *file, int line) {
    if (pthread_mutex_destroy_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_mutex_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_mutex_destroy");
    }
    return func(mutex);
}

bool pthread_mutex_lock_is_overridden = false;
int cmocka_test_pthread_mutex_lock(pthread_mutex_t *mutex,
                                   const char *file, int line) {
    if (pthread_mutex_lock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_mutex_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_mutex_lock");
    }
    return func(mutex);
}

bool pthread_mutex_trylock_is_overridden = false;
int cmocka_test_pthread_mutex_trylock(pthread_mutex_t *mutex,
                                      const char *file, int line) {
    if (pthread_mutex_trylock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_mutex_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_mutex_trylock");
    }
    return func(mutex);
}

bool pthread_mutex_unlock_is_overridden = false;
int cmocka_test_pthread_mutex_unlock(pthread_mutex_t *mutex,
                                     const char *file, int line) {
    if (pthread_mutex_unlock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_mutex_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_mutex_unlock");
    }
    return func(mutex);
}

bool pthread_rwlock_init_is_overridden = false;
int cmocka_test_pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                                    const pthread_rwlockattr_t *attr,
                                    const char *file, int line) {
    if (pthread_rwlock_init_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *, const pthread_rwlockattr_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_init");
    }
    return func(rwlock, attr);
}

bool pthread_rwlock_destroy_is_overridden = false;
int cmocka_test_pthread_rwlock_destroy(pthread_rwlock_t *rwlock,
                                       const char *file, int line) {
    if (pthread_rwlock_destroy_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_destroy");
    }
    return func(rwlock);
}

bool pthread_rwlock_rdlock_is_overridden = false;
int cmocka_test_pthread_rwlock_rdlock(pthread_rwlock_t *rwlock,
                                      const char *file, int line) {
    if (pthread_rwlock_rdlock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_rdlock");
    }
    return func(rwlock);
}

bool pthread_rwlock_tryrdlock_is_overridden = false;
int cmocka_test_pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock,
                                         const char *file, int line) {
    if (pthread_rwlock_tryrdlock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_tryrdlock");
    }
    return func(rwlock);
}

bool pthread_rwlock_wrlock_is_overridden = false;
int cmocka_test_pthread_rwlock_wrlock(pthread_rwlock_t *rwlock,
                                      const char *file, int line) {
    if (pthread_rwlock_wrlock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_wrlock");
    }
    return func(rwlock);
}

bool pthread_rwlock_trywrlock_is_overridden = false;
int cmocka_test_pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock,
                                         const char *file, int line) {
    if (pthread_rwlock_trywrlock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_trywrlock");
    }
    return func(rwlock);
}

bool pthread_rwlock_unlock_is_overridden = false;
int cmocka_test_pthread_rwlock_unlock(pthread_rwlock_t *rwlock,
                                      const char *file, int line) {
    if (pthread_rwlock_unlock_is_overridden) {
        return mock();
    }
    static int (*func)(pthread_rwlock_t *) = NULL;
    if (!func) {
        func = dlsym(RTLD_NEXT, u8"pthread_rwlock_unlock");
    }
    return func(rwlock);
}
