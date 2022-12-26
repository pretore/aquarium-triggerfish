#ifndef _TRIGGERFISH_PRIVATE_WEAK_H_
#define _TRIGGERFISH_PRIVATE_WEAK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

struct triggerfish_weak {
    atomic_uintptr_t strong;
};

#endif /* _TRIGGERFISH_PRIVATE_WEAK_H_ */
