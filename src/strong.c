#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <errno.h>
#include <seagrass.h>
#include <triggerfish.h>

#include "private/strong.h"
#include "private/weak.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

static int compare(const void *a, const void *b) {
    struct triggerfish_weak **A = (void *) a;
    struct triggerfish_weak **B = (void *) b;
    return seagrass_void_ptr_compare(*A, *B);
}

int triggerfish_strong_of(void *const instance,
                          void (*const on_destroy)(void *instance),
                          struct triggerfish_strong **const out) {
    if (!instance) {
        return TRIGGERFISH_STRONG_ERROR_INSTANCE_IS_NULL;
    }
    if (!on_destroy) {
        return TRIGGERFISH_STRONG_ERROR_ON_DESTROY_IS_NULL;
    }
    if (!out) {
        return TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL;
    }
    struct triggerfish_strong *object = calloc(1, sizeof(*object));
    if (!object) {
        return TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    switch (pthread_mutex_init(&object->lock, NULL)) {
        default: {
            seagrass_required_true(false);
        }
        case ENOMEM: {
            free(object);
            return TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED;;
        }
        case 0: {
            break;
        }
    }
    seagrass_required_true(!coral_red_black_tree_container_init(
            &object->weak_refs, compare));
    object->instance = instance;
    object->on_destroy = on_destroy;
    atomic_store(&object->counter, 1);
    *out = object;
    return 0;
}

int triggerfish_strong_count(struct triggerfish_strong *const object,
                             uintmax_t *const out) {
    if (!object) {
        return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL;
    }
    *out = atomic_load(&object->counter);
    return 0;
}

int triggerfish_strong_retain(struct triggerfish_strong *const object) {
    if (!object) {
        return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
    }
    uintmax_t desired;
    uintmax_t expected = atomic_load(&object->counter);
    do {
        if (!expected) {
            return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID;
        }
        seagrass_required_true(!seagrass_uintmax_t_add(
                1, expected, &desired));
    } while (!atomic_compare_exchange_strong(&object->counter,
                                             &expected, desired));
    return 0;
}

int triggerfish_strong_release(struct triggerfish_strong *const object) {
    if (!object) {
        return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
    }
    uintmax_t desired;
    uintmax_t expected = atomic_load(&object->counter);
    do {
        seagrass_required_true(!seagrass_uintmax_t_subtract(
                expected, 1, &desired));
    } while (!atomic_compare_exchange_strong(&object->counter,
                                             &expected, desired));
    if (desired) {
        return 0;
    }
    int error;
    seagrass_required_true(!pthread_mutex_lock(&object->lock));
    uintptr_t check = (uintptr_t) object;
    union {
        struct coral_red_black_tree_container_entry *entry;
        struct triggerfish_weak **weak;
    } ptr;
    if (!(error = coral_red_black_tree_container_first(
            &object->weak_refs, &ptr.entry))) {
        do {
            seagrass_required_true(atomic_compare_exchange_strong(
                    &(*ptr.weak)->strong, &check, 0));
        } while (!(error = coral_red_black_tree_container_next(
                ptr.entry, &ptr.entry)));
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE
                == error);
    } else {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY
                == error);
    }
    seagrass_required_true(!pthread_mutex_unlock(&object->lock));
    seagrass_required_true(!pthread_mutex_destroy(&object->lock));
    seagrass_required_true(!coral_red_black_tree_container_invalidate(
            &object->weak_refs, NULL));
    object->on_destroy(object->instance);
    free(object->instance);
    free(object);
    return 0;
}

int triggerfish_strong_instance(const struct triggerfish_strong *const object,
                                void **const out) {
    if (!object) {
        return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return TRIGGERFISH_STRONG_ERROR_OUT_IS_NULL;
    }
    if (!atomic_load(&object->counter)) {
        return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID;
    }
    *out = object->instance;
    return 0;
}

int triggerfish_strong_register(struct triggerfish_strong *const object,
                                struct triggerfish_weak *weak) {
    assert(object);
    assert(weak);
    int error;
    if ((error = pthread_mutex_lock(&object->lock))) {
        seagrass_required_true(EINVAL == error);
        return TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID;
    }
    union {
        struct coral_red_black_tree_container_entry *entry;
        struct triggerfish_weak **weak;
    } ptr;
    if ((error = coral_red_black_tree_container_alloc(
            sizeof(struct triggerfish_weak **), &ptr.entry))) {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_CONTAINER_ERROR_MEMORY_ALLOCATION_FAILED
                == error);
    } else {
        *ptr.weak = weak;
        if ((error = coral_red_black_tree_container_add(
                &object->weak_refs, ptr.entry))) {
            seagrass_required_true(
                    CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXISTS
                    == error);
            seagrass_required_true(!coral_red_black_tree_container_free(
                    ptr.entry));
        }
    }
    seagrass_required_true(!pthread_mutex_unlock(&object->lock));
    return error;
}

void triggerfish_strong_unregister(struct triggerfish_strong *const object,
                                   const struct triggerfish_weak *const weak) {
    assert(object);
    assert(weak);
    int error;
    if ((error = pthread_mutex_lock(&object->lock))) {
        seagrass_required_true(EINVAL == error);
        return;
    }
    union {
        struct coral_red_black_tree_container_entry *entry;
        struct triggerfish_weak **weak;
    } ptr;
    if ((error = coral_red_black_tree_container_get(
            &object->weak_refs, &weak, &ptr.entry))) {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND
                == error);
    } else {
        seagrass_required_true(!coral_red_black_tree_container_remove(
                &object->weak_refs, ptr.entry));
        seagrass_required_true(!coral_red_black_tree_container_free(ptr.entry));
    }
    seagrass_required_true(!pthread_mutex_unlock(&object->lock));
}
