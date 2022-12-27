#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <seagrass.h>
#include <triggerfish.h>

#include "private/strong.h"
#include "private/weak.h"
#include "test/cmocka.h"

static int compare(const void *a, const void *b) {
    struct triggerfish_weak **A = (void *) a;
    struct triggerfish_weak **B = (void *) b;
    return seagrass_void_ptr_compare(*A, *B);
}

bool triggerfish_strong_of(void *const instance,
                           void (*const on_destroy)(void *instance),
                           struct triggerfish_strong **const out) {
    if (!instance) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL;
        return false;
    }
    if (!on_destroy) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL;
        return false;
    }
    if (!out) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL;
        return false;
    }
    struct triggerfish_strong *object = calloc(1, sizeof(*object));
    if (!object) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    switch (pthread_mutex_init(&object->lock, NULL)) {
        default: {
            seagrass_required_true(false);
        }
        case ENOMEM: {
            triggerfish_error =
                    TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED;
            free(object);
            return false;
        }
        case 0: {
            break;
        }
    }
    seagrass_required_true(coral_red_black_tree_set_init(
            &object->weak_refs,
            sizeof(struct triggerfish_weak *),
            compare));
    object->instance = instance;
    object->on_destroy = on_destroy;
    atomic_store(&object->counter, 1);
    *out = object;
    return true;
}

bool triggerfish_strong_retain(struct triggerfish_strong *const object) {
    if (!object) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
        return false;
    }
    uintmax_t expected;
    uintmax_t desired;
    do {
        expected = atomic_load(&object->counter);
        if (!expected) {
            triggerfish_error = TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID;
            return false;
        }
        seagrass_required_true(seagrass_uintmax_t_add(
                1, expected, &desired));
    } while (!atomic_compare_exchange_strong(&object->counter,
                                             &expected, desired));
    return true;
}

bool triggerfish_strong_release(struct triggerfish_strong *const object) {
    if (!object) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
        return false;
    }
    uintmax_t expected;
    uintmax_t desired;
    do {
        expected = atomic_load(&object->counter);
        seagrass_required_true(seagrass_uintmax_t_subtract(
                expected, 1, &desired));
    } while (!atomic_compare_exchange_strong(&object->counter,
                                             &expected, desired));
    if (desired) {
        return true;
    }
    seagrass_required_true(!pthread_mutex_lock(&object->lock));
    const uintptr_t check = (uintptr_t) object;
    struct triggerfish_weak **weak_ref;
    if (coral_red_black_tree_set_first(&object->weak_refs,
                                       (const void **) &weak_ref)) {
        do {
            const uintptr_t value = atomic_load(&(*weak_ref)->strong);
            seagrass_required_true(check == value);
            atomic_store(&(*weak_ref)->strong, 0);
        } while (coral_red_black_tree_set_next(weak_ref,
                                               (const void **) &weak_ref));
        seagrass_required_true(CORAL_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE
                               == coral_error);
    } else {
        seagrass_required_true(CORAL_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY
                               == coral_error);
    }
    seagrass_required_true(!pthread_mutex_unlock(&object->lock));
    do {
        switch (pthread_mutex_destroy(&object->lock)) {
            default: {
                seagrass_required_true(false);
            }
            case EBUSY: {
                const struct timespec delay = {
                        .tv_nsec = 1000
                };
                seagrass_required_true(!nanosleep(&delay, NULL)
                                       || errno == EINTR);
                continue;
            }
            case 0: {
                /* fall-through */
            }
        }
        break;
    } while (true);
    seagrass_required_true(coral_red_black_tree_set_invalidate(
            &object->weak_refs, NULL));
    object->on_destroy(object->instance);
    free(object->instance);
    free(object);
    return true;
}

bool triggerfish_strong_instance(struct triggerfish_strong *const object,
                                 void **const out) {
    if (!object) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!atomic_load(&object->counter)) {
        triggerfish_error = TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID;
        return false;
    }
    *out = object->instance;
    return true;
}

bool triggerfish_strong_register(struct triggerfish_strong *const object,
                                 struct triggerfish_weak *const weak) {
    assert(object);
    assert(weak);
    switch (pthread_mutex_lock(&object->lock)) {
        default: {
            seagrass_required_true(false);
        }
        case EINVAL: {
            triggerfish_error = TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID;
            return false;
        }
        case 0: {
            /* fall-through */
        }
    }
    const bool result = coral_red_black_tree_set_add(&object->weak_refs,
                                                     &weak);
    if (!result) {
        switch (coral_error) {
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS: {
                triggerfish_error =
                        TRIGGERFISH_STRONG_ERROR_WEAK_ALREADY_REGISTERED;
                break;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                triggerfish_error =
                        TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED;
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    seagrass_required_true(!pthread_mutex_unlock(&object->lock));
    return result;
}

void triggerfish_strong_unregister(struct triggerfish_strong *const object,
                                   struct triggerfish_weak *const weak) {
    assert(object);
    assert(weak);
    switch (pthread_mutex_lock(&object->lock)) {
        default: {
            seagrass_required_true(false);
        }
        case EINVAL: {
            return;
        }
        case 0: {
            /* fall-through */
        }
    }
    if (!coral_red_black_tree_set_remove(&object->weak_refs,
                                         &weak)) {
        seagrass_required_true(CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND
                               == coral_error);
    }
    seagrass_required_true(!pthread_mutex_unlock(&object->lock));
}
