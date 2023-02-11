#include <stdlib.h>
#include <assert.h>
#include <seagrass.h>
#include <triggerfish.h>

#include "private/strong.h"
#include "private/weak.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

static bool init(struct triggerfish_weak *const object,
                 struct triggerfish_strong *const strong) {
    assert(object);
    assert(strong);
    *object = (struct triggerfish_weak) {0};
    if (!triggerfish_strong_register(strong, object)) {
        switch (triggerfish_error) {
            case TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID: {
                triggerfish_error = TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID;
                break;
            }
            case TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED: {
                triggerfish_error =
                        TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED;
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
        return false;
    }
    atomic_store(&object->strong, (uintptr_t) strong);
    return true;
}

bool triggerfish_weak_of(struct triggerfish_strong *const strong,
                         struct triggerfish_weak **const out) {
    if (!strong) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_STRONG_IS_NULL;
        return false;
    }
    if (!out) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL;
        return false;
    }
    struct triggerfish_weak *object = calloc(1, sizeof(*object));
    if (!object) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    const bool result = init(object, strong);
    if (!result) {
        switch (triggerfish_error) {
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
    return result;
}

bool triggerfish_weak_destroy(struct triggerfish_weak *const object) {
    if (!object) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    struct triggerfish_strong *strong = (void *) atomic_load(&object->strong);
    if (strong) {
        triggerfish_strong_unregister(strong, object);
    }
    free(object);
    return true;
}

bool triggerfish_weak_copy_of(const struct triggerfish_weak *const other,
                              struct triggerfish_weak **const out) {
    if (!other) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_OTHER_IS_NULL;
        return false;
    }
    if (!out) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL;
        return false;
    }
    struct triggerfish_weak *object = calloc(1, sizeof(*object));
    if (!object) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    struct triggerfish_strong *strong;
    if (!triggerfish_weak_strong(other, &strong)) {
        *out = object;
        return true;
    }
    const bool result = init(object, strong);
    if (!result) {
        switch (triggerfish_error) {
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
    seagrass_required_true(triggerfish_strong_release(strong));
    return result;
}

bool triggerfish_weak_strong(const struct triggerfish_weak *const object,
                             struct triggerfish_strong **const out) {
    if (!object) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_OUT_IS_NULL;
        return false;
    }
    struct triggerfish_strong *strong = (void *) atomic_load(&object->strong);
    if (!strong) {
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID;
        return false;
    }
    if (!triggerfish_strong_retain(strong)) {
        seagrass_required_true(TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID
                               == triggerfish_error);
        triggerfish_error = TRIGGERFISH_WEAK_ERROR_STRONG_IS_INVALID;
        return false;
    }
    *out = strong;
    return true;
}

