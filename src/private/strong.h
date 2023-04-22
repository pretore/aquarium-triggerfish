#ifndef _TRIGGERFISH_PRIVATE_STRONG_H_
#define _TRIGGERFISH_PRIVATE_STRONG_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <coral.h>
#include <sea-urchin.h>

#define TRIGGERFISH_STRONG_ERROR_WEAK_ALREADY_REGISTERED \
    SEA_URCHIN_ERROR_VALUE_ALREADY_EXISTS

struct triggerfish_weak;
struct triggerfish_strong {
    atomic_uintmax_t counter;
    void *instance;
    pthread_mutex_t lock;
    struct coral_red_black_tree_container weak_refs;

    void (*on_destroy)(void *instance);
};

/**
 * @brief Register weak reference for invalidation when strong reference is
 * destroyed.
 * @param [in] object strong reference.
 * @param [in] weak reference that will be invalidated.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws TRIGGERFISH_STRONG_ERROR_OBJECT_IS_INVALID if object has been
 * invalidated.
 * @throws TRIGGERFISH_STRONG_ERROR_WEAK_ALREADY_REGISTERED if weak reference
 * is already registered.
 * @throws TRIGGERFISH_STRONG_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to register weak reference.
 */
int triggerfish_strong_register(struct triggerfish_strong *object,
                                struct triggerfish_weak *weak);

/**
 * @brief Unregister weak reference for invalidation when strong reference is
 * destroyed.
 * @param [in] object strong reference.
 * @param [in] weak reference that will be invalidated.
 */
void triggerfish_strong_unregister(struct triggerfish_strong *object,
                                   const struct triggerfish_weak *weak);

#endif /* _TRIGGERFISH_PRIVATE_STRONG_H_ */
