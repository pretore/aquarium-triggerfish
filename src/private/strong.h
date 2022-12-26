#ifndef _TRIGGERFISH_PRIVATE_STRONG_H_
#define _TRIGGERFISH_PRIVATE_STRONG_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <coral.h>

#define TRIGGERFISH_STRONG_ERROR_WEAK_ALREADY_REGISTERED            (-1)

struct triggerfish_weak;
struct triggerfish_strong {
    atomic_uintmax_t counter;
    void *instance;
    pthread_mutex_t lock;
    struct coral_red_black_tree_set weak_refs;

    void (*on_destroy)(void *instance);
};

bool triggerfish_strong_register(struct triggerfish_strong *object,
                                 struct triggerfish_weak *weak);

void triggerfish_strong_unregister(struct triggerfish_strong *object,
                                   struct triggerfish_weak *weak);

#endif /* _TRIGGERFISH_PRIVATE_STRONG_H_ */
