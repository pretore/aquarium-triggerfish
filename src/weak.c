#include <stdlib.h>
#include <assert.h>
#include <seagrass.h>
#include <triggerfish.h>

#include "private/strong.h"
#include "private/weak.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

static int init(struct triggerfish_weak *const object,
                struct triggerfish_strong *const strong) {
    assert(object);
    assert(strong);
    int error;
    *object = (struct triggerfish_weak) {0};
    if ((error = triggerfish_strong_register(strong, object))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID:
            case TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    atomic_store(&object->strong, (uintptr_t) strong);
    return 0;
}

int triggerfish_weak_of(struct triggerfish_strong *const strong,
                        struct triggerfish_weak **const out) {
    if (!strong) {
        return TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL;
    }
    if (!out) {
        return TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL;
    }
    struct triggerfish_weak *object = calloc(1, sizeof(*object));
    if (!object) {
        return TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    int error;
    if ((error = init(object, strong))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED:
            case TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID: {
                free(object);
            }
        }
    } else {
        *out = object;
    }
    return error;
}

int triggerfish_weak_destroy(struct triggerfish_weak *const object) {
    if (!object) {
        return TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL;
    }
    struct triggerfish_strong *strong = (void *) atomic_load(&object->strong);
    if (strong) {
        triggerfish_strong_unregister(strong, object);
    }
    free(object);
    return 0;
}

int triggerfish_weak_copy_of(const struct triggerfish_weak *const other,
                             struct triggerfish_weak **const out) {
    if (!other) {
        return TRIGGERFISH_WEAK_ERROR_OTHER_IS_NULL;
    }
    if (!out) {
        return TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL;
    }
    struct triggerfish_weak *object = calloc(1, sizeof(*object));
    if (!object) {
        return TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    int error;
    struct triggerfish_strong *strong;
    if ((error = triggerfish_weak_strong(other, &strong))) {
        seagrass_required_true(TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID
                               == error);
        *out = object;
        return 0;
    }
    if ((error = init(object, strong))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED:
            case TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID: {
                free(object);
            }
        }
    } else {
        *out = object;
    }
    seagrass_required_true(!triggerfish_strong_release(strong));
    return error == TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID
           ? 0
           : error;
}

int triggerfish_weak_strong(const struct triggerfish_weak *const object,
                            struct triggerfish_strong **const out) {
    if (!object) {
        return TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL;
    }
    struct triggerfish_strong *strong = (void *) atomic_load(&object->strong);
    if (!strong) {
        return TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID;
    }
    int error;
    if ((error = triggerfish_strong_retain(strong))) {
        seagrass_required_true(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID
                               == error);
        return TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID;
    }
    *out = strong;
    return 0;
}

