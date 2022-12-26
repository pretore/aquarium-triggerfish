#include <stdlib.h>
#include <seagrass.h>
#include <triggerfish.h>

#include "private/strong.h"
#include "private/weak.h"
#include "test/cmocka.h"

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
        free(object);
        return false;
    }
    atomic_store(&object->strong, (uintptr_t) strong);
    *out = object;
    return true;
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

bool triggerfish_weak_strong(struct triggerfish_weak *const object,
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

